/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include <stdlib.h>
#include "cr_pack.h"
#include "cr_mem.h"
#include "cr_net.h"
#include "cr_pixeldata.h"
#include "cr_protocol.h"
#include "cr_error.h"
#include "packspu.h"
#include "packspu_proto.h"

static void
packspuWriteback( const CRMessageWriteback *wb )
{
	int *writeback;
	crMemcpy( &writeback, &(wb->writeback_ptr), sizeof( writeback ) );
	*writeback = 0;
}

/**
 * XXX Note that this routine is identical to crNetRecvReadback except
 * we set *writeback=0 instead of decrementing it.  Hmmm.
 */
static void
packspuReadback( const CRMessageReadback *rb, unsigned int len )
{
	/* minus the header, the destination pointer, 
	 * *and* the implicit writeback pointer at the head. */

	int payload_len = len - sizeof( *rb );
	int *writeback;
	void *dest_ptr; 
	crMemcpy( &writeback, &(rb->writeback_ptr), sizeof( writeback ) );
	crMemcpy( &dest_ptr, &(rb->readback_ptr), sizeof( dest_ptr ) );

	*writeback = 0;
	crMemcpy( dest_ptr, ((char *)rb) + sizeof(*rb), payload_len );
}

static void
packspuReadPixels( const CRMessageReadPixels *rp, unsigned int len )
{
	crNetRecvReadPixels( rp, len );
	--pack_spu.ReadPixels;
}

static int
packspuReceiveData( CRConnection *conn, CRMessage *msg, unsigned int len )
{
	switch( msg->header.type )
	{
		case CR_MESSAGE_READ_PIXELS:
			packspuReadPixels( &(msg->readPixels), len );
			break;
		case CR_MESSAGE_WRITEBACK:
			packspuWriteback( &(msg->writeback) );
			break;
		case CR_MESSAGE_READBACK:
			packspuReadback( &(msg->readback), len );
			break;
		default:
			/*crWarning( "Why is the pack SPU getting a message of type 0x%x?", msg->type ); */
			return 0; /* NOT HANDLED */
	}
	return 1; /* HANDLED */
}

static CRMessageOpcodes *
__prependHeader( CRPackBuffer *buf, unsigned int *len, unsigned int senderID )
{
	int num_opcodes;
	CRMessageOpcodes *hdr;

	CRASSERT( buf );
	CRASSERT( buf->opcode_current < buf->opcode_start );
	CRASSERT( buf->opcode_current >= buf->opcode_end );
	CRASSERT( buf->data_current > buf->data_start );
	CRASSERT( buf->data_current <= buf->data_end );

	num_opcodes = buf->opcode_start - buf->opcode_current;
	hdr = (CRMessageOpcodes *) 
		( buf->data_start - ( ( num_opcodes + 3 ) & ~0x3 ) - sizeof(*hdr) );

	CRASSERT( (void *) hdr >= buf->pack );

	if (pack_spu.swap)
	{
		hdr->header.type = (CRMessageType) SWAP32(CR_MESSAGE_OPCODES);
		hdr->numOpcodes  = SWAP32(num_opcodes);
	}
	else
	{
		hdr->header.type = CR_MESSAGE_OPCODES;
		hdr->numOpcodes  = num_opcodes;
	}

	*len = buf->data_current - (unsigned char *) hdr;

	return hdr;
}


/*
 * This is called from either the Pack SPU and the packer library whenever
 * we need to send a data buffer to the server.
 */
void
packspuFlush(void *arg )
{
	ThreadInfo *thread = (ThreadInfo *) arg;
	ContextInfo *ctx;
	unsigned int len;
	CRMessageOpcodes *hdr;
	CRPackBuffer *buf;

	/* we should _always_ pass a valid <arg> value */
	CRASSERT(thread);
	ctx = thread->currentContext;
	buf = &(thread->buffer);
	CRASSERT(buf);

	/* We're done packing into the current buffer, unbind it */
	crPackReleaseBuffer( thread->packer );

	/*
	printf("%s thread=%p thread->id = %d thread->pc=%p t2->id=%d t2->pc=%p packbuf=%p packbuf=%p\n",
		   __FUNCTION__, (void*) thread, (int) thread->id, thread->packer,
		   (int) t2->id, t2->packer,
		   buf->pack, thread->packer->buffer.pack);
	*/

	if ( buf->opcode_current == buf->opcode_start ) {
           /*
           printf("%s early return\n", __FUNCTION__);
           */
           /* XXX these calls seem to help, but might be appropriate */
           crPackSetBuffer( thread->packer, buf );
           crPackResetPointers(thread->packer);
           return;
	}

	hdr = __prependHeader( buf, &len, 0 );

	CRASSERT( thread->netServer.conn );

	if ( buf->holds_BeginEnd )
		crNetBarf( thread->netServer.conn, &(buf->pack), hdr, len );
	else
		crNetSend( thread->netServer.conn, &(buf->pack), hdr, len );

	buf->pack = crNetAlloc( thread->netServer.conn );

	/* The network may have found a new mtu */
	buf->mtu = thread->netServer.conn->mtu;

	crPackSetBuffer( thread->packer, buf );

	crPackResetPointers(thread->packer);
	(void) arg;
}


/**
 * XXX NOTE: there's a lot of duplicate code here common to the
 * pack, tilesort and replicate SPUs.  Try to simplify someday!
 */
void
packspuHuge( CROpcode opcode, void *buf )
{
	GET_THREAD(thread);
	unsigned int          len;
	unsigned char        *src;
	CRMessageOpcodes *msg;

	CRASSERT(thread);

	/* packet length is indicated by the variable length field, and
	   includes an additional word for the opcode (with alignment) and
	   a header */
	len = ((unsigned int *) buf)[-1];
	if (pack_spu.swap)
	{
		/* It's already been swapped, swap it back. */
		len = SWAP32(len);
	}
	len += 4 + sizeof(CRMessageOpcodes);

	/* write the opcode in just before the length */
	((unsigned char *) buf)[-5] = (unsigned char) opcode;

	/* fix up the pointer to the packet to include the length & opcode
       & header */
	src = (unsigned char *) buf - 8 - sizeof(CRMessageOpcodes);

	msg = (CRMessageOpcodes *) src;

	if (pack_spu.swap)
	{
		msg->header.type = (CRMessageType) SWAP32(CR_MESSAGE_OPCODES);
		msg->numOpcodes  = SWAP32(1);
	}
	else
	{
		msg->header.type = CR_MESSAGE_OPCODES;
		msg->numOpcodes  = 1;
	}

	CRASSERT( thread->netServer.conn );
	crNetSend( thread->netServer.conn, NULL, src, len );
}


static void
packspuCloseCallback(CRConnection *conn)
{
	GET_THREAD(thread);
	if (thread && conn == thread->netServer.conn) {
		crDebug("Pack SPU: Server connection closed - exiting.");
		exit(0);
	}
}


void
packspuConnectToServer( CRNetServer *server )
{
	crNetInit( packspuReceiveData, packspuCloseCallback );
	crNetServerConnect( server );
}
