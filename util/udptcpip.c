/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#pragma warning( push, 3 )
#include <winsock2.h>
#include <ws2tcpip.h> /* for ipv6 */
#include <time.h>
#pragma warning( pop )
#pragma warning( disable : 4514 )
#pragma warning( disable : 4127 )
typedef int ssize_t;
#define write(a,b,c) send(a,b,c,0)
#else
#include <sys/types.h>
#ifdef DARWIN
typedef unsigned int socklen_t;
#elif defined(OSF1)
typedef int socklen_t;
#endif
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#ifdef LINUX
#define IP_MTU	14
#endif
#include <netinet/in.h>
#if !defined(IRIX) /* ip6.h might not be present on other unix varients either */
#include <netinet/ip6.h>
#endif
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#ifdef AIX
#include <strings.h>
#endif

#include "cr_error.h"
#include "cr_mem.h"
#include "cr_string.h"
#include "cr_bufpool.h"
#include "cr_net.h"
#include "cr_endian.h"
#include "cr_threads.h"
#include "net_internals.h"

#ifdef ADDRINFO
#define PF PF_UNSPEC
#endif

#ifndef MSG_TRUNC
#define MSG_TRUNC 0
#endif

#ifdef IRIX
/* IRIX defines this token, but appears to be missing the related types
 * and functions.  Turn it off.
 */
#undef AF_INET6
#endif

typedef enum {
	CRTCPIPMemory,
	CRTCPIPMemoryBig
} CRTCPIPBufferKind;

#define CR_TCPIP_BUFFER_MAGIC 0x89134532

typedef struct CRTCPIPBuffer {
	unsigned int          magic;
	CRTCPIPBufferKind     kind;
	unsigned int          len;
	unsigned int          allocated;
	unsigned int          pad;
} CRTCPIPBuffer;


extern void crTCPIPFree( CRConnection *conn, void *buf );
extern int crTCPIPErrno( void );
extern char *crTCPIPErrorString( int err );
extern void crCloseSocket( CRSocket sock );

extern struct {
	int                  initialized;
	int                  num_conns;
	CRConnection         **conns;
	CRBufferPool         bufpool;
#ifdef CHROMIUM_THREADSAFE
	CRmutex              mutex;
	CRmutex              recvmutex;
#endif
	CRNetReceiveFunc     recv;
	CRNetCloseFunc       close;
	CRSocket             server_sock;
} cr_tcpip;

extern int __read_exact( CRSocket sock, void *buf, unsigned int len );
extern void crTCPIPReadExact( CRSocket sock, void *buf, unsigned int len );
extern int __write_exact( CRSocket sock, void *buf, unsigned int len );
extern void crTCPIPWriteExact( CRConnection *conn, void *buf, unsigned int len );

#if defined( WINDOWS ) || defined( IRIX ) || defined( IRIX64 )
typedef int socklen_t;
#endif

void crUDPIPWriteExact( CRConnection *conn, void *buf, unsigned int len )
{
	int retval;
	if ( len > conn->mtu + sizeof(conn->seq) )
	{
		crWarning( "crUDPIPWriteExact(%d): too big a packet for mtu %d, dropping !", len, conn->mtu + sizeof(conn->seq) );
		return;
	}
	retval = sendto( conn->udp_socket, buf, len, 0,
		(struct sockaddr *) &(conn->remoteaddr), sizeof(conn->remoteaddr));
	if ( retval <= 0 )
	{
		int err = crTCPIPErrno( );
		crWarning( "crUDPIPWriteExact(%d): %s", len, crTCPIPErrorString( err ) );
#ifdef LINUX
		if ( err == EMSGSIZE )
		{
			int opt;
			socklen_t leno = sizeof(opt);
			crDebug( "Too big a UDP packet(%d), looking at new MTU...", len );
			if ( getsockopt( conn->udp_socket, SOL_IP, IP_MTU_DISCOVER, &opt, &leno) == -1)
			{
				err = crTCPIPErrno( );
				crWarning( "Couldn't determine whether PMTU discovery is used : %s", crTCPIPErrorString( err ) );
				return;
			}
			if ( leno != sizeof(opt) )
			{
				crWarning( "Unexpected length %d for PMTU_DISCOVERY option length", leno );
				return;
			}
			if ( opt == IP_PMTUDISC_DONT )
			{
				crWarning( "Uh, PMTU discovery isn't enabled ?!" );
				return;
			}
			if ( getsockopt( conn->udp_socket, SOL_IP, IP_MTU, &opt, &leno) == -1 )
			{
				err = crTCPIPErrno( );
				crWarning( "Couldn't determine the MTU : %s", crTCPIPErrorString( err ) );
				return;
			}
			if ( leno != sizeof(opt) )
			{
				crWarning( "Unexpected length %d for MTU option length", leno );
				return;
			}
			opt -= sizeof(conn->seq) + sizeof(struct udphdr) + sizeof(struct ip6_hdr);
			if (opt >= conn->mtu)
			{
				crWarning( "But MTU discovery is still bigger ! Narrowing it by hand to %d", conn->mtu = (conn->mtu * 2 / 3) & ~0x3 );
			}
			else
			{
				crDebug( "new MTU is %d", opt );
				conn->mtu = opt & ~0x3;
			}
		}
#endif
	}
}

extern void crTCPIPAccept( CRConnection *conn, unsigned short port );
static void crUDPTCPIPAccept( CRConnection *conn, unsigned short port )
{
	int err;
	socklen_t		addr_length;
#ifndef ADDRINFO
	struct sockaddr		addr;
	struct sockaddr_in	udpaddr;
#else
	struct sockaddr_storage addr;
	struct addrinfo         *res,*cur;
	struct addrinfo         hints;
#endif

	crTCPIPAccept( conn, port );

#ifndef ADDRINFO
	conn->udp_socket = socket( AF_INET, SOCK_DGRAM, 0 );
	if ( conn->udp_socket >= 0 )
	{
		memset(&udpaddr, 0, sizeof(udpaddr));
		udpaddr.sin_family = AF_INET;
		udpaddr.sin_addr.s_addr = INADDR_ANY;
		udpaddr.sin_port = htons(0);
		if ( bind( conn->udp_socket, (struct sockaddr *) &udpaddr, sizeof(udpaddr) ) )
		{
			err = crTCPIPErrno( );
			crWarning( "Couldn't bind socket: %s", crTCPIPErrorString( err ) );
			crCloseSocket( conn->udp_socket );
			conn->udp_socket = -1;
		}
	}
#else
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = PF;
	hints.ai_socktype = SOCK_DGRAM;

	err = getaddrinfo( NULL, "0", &hints, &res);
	if ( err )
		crError( "Couldn't find local UDP port: %s", gai_strerror(err));

	conn->udp_socket = -1;

	for (cur=res;cur;cur=cur->ai_next)
	{
		conn->udp_socket = socket( cur->ai_family, cur->ai_socktype, cur->ai_protocol );
		if ( conn->udp_socket == -1 )
		{
			err = crTCPIPErrno( );
			if (err != EAFNOSUPPORT)
				crWarning( "Couldn't create socket of family %i: %s, trying another one", cur->ai_family, crTCPIPErrorString( err ) );
			continue;
		}
		if ( bind( conn->udp_socket, cur->ai_addr, cur->ai_addrlen ) )
		{
			err = crTCPIPErrno( );
			crWarning( "Couldn't bind socket: %s", crTCPIPErrorString( err ) );
			crCloseSocket( conn->udp_socket );
			conn->udp_socket = -1;
			continue;
		}
		break;

	}
	freeaddrinfo(res);
#endif

	if ( conn->udp_socket < 0 )
		crError( "Couldn't find local UDP port" );
	
	addr_length = sizeof(addr);
	err = getsockname( conn->udp_socket, (struct sockaddr *) &addr, &addr_length );

	if ( err == -1 )
		crError( "Couldn't get our local UDP port: %s", crTCPIPErrorString( crTCPIPErrno( ) ) );

	switch (((struct sockaddr *) &addr)->sa_family) {
	case AF_INET:
		crTCPIPWriteExact( conn, &((struct sockaddr_in *)&addr)->sin_port,
				sizeof(((struct sockaddr_in *)&addr)->sin_port));
		break;
#ifdef AF_INET6
	case AF_INET6:
		crTCPIPWriteExact( conn, &((struct sockaddr_in6 *)&addr)->sin6_port,
				sizeof(((struct sockaddr_in6 *)&addr)->sin6_port));
		break;
#endif
	default:
		crError( "Unknown address family: %d", ((struct sockaddr *) &addr)->sa_family);
	}
}

extern void *crTCPIPAlloc (CRConnection *conn );

static unsigned int safelen=0;
static void crUDPTCPIPSend( CRConnection *conn, void **bufp,
				void *start, unsigned int len )
{
	static unsigned int safeblip=0;
	CRTCPIPBuffer *udptcpip_buffer;
	unsigned int      *lenp;

	if ( safelen+len > safelen )
	{
		safelen+=len;
		if (safelen-safeblip>100000)
		{
			safeblip=safelen;
			crDebug( "%dKo safe", safelen/1024 );
		}
	}

	conn->seq++;
	if ( bufp == NULL )
	{
		unsigned int len_swap = conn->swap ? SWAP32(len) : len;
		/* we are doing synchronous sends from user memory, so no need
		 * to get fancy.  Simply write the length & the payload and
		 * return. */
		crTCPIPWriteExact( conn, &len_swap, sizeof(len_swap) );
		crTCPIPWriteExact( conn, start, len );
		return;
	}

	udptcpip_buffer = (CRTCPIPBuffer *)(*bufp) - 1;

	CRASSERT( udptcpip_buffer->magic == CR_TCPIP_BUFFER_MAGIC );

	/* All of the buffers passed to the send function were allocated
	 * with crTCPIPAlloc(), which includes a header with a 4 byte
	 * length field, to insure that we always have a place to write
	 * the length field, even when start == *bufp. */
	lenp = (unsigned int *) start - 1;
	if (conn->swap)
	{
		*lenp = SWAP32(len);
	}
	else
	{
		*lenp = len;
	}

	if ( __write_exact( conn->tcp_socket, lenp, len + sizeof(*lenp) ) < 0 )
	{
		int err = crTCPIPErrno( );
		crError( "crUDPTCPIPSend: %s", crTCPIPErrorString( err ) );
	}

	/* reclaim this pointer for reuse and try to keep the client from
		 accidentally reusing it directly */
#ifdef CHROMIUM_THREADSAFE
	crLockMutex(&cr_tcpip.mutex);
#endif
	crBufferPoolPush( &cr_tcpip.bufpool, udptcpip_buffer );
#ifdef CHROMIUM_THREADSAFE
	crUnlockMutex(&cr_tcpip.mutex);
#endif
	*bufp = NULL;
}

static unsigned int barflen=0;
static void crUDPTCPIPBarf( CRConnection *conn, void **bufp,
				void *start, unsigned int len)
{
	static unsigned int barfblip=0;
	static const unsigned int sizes[]={
		0,10,50,100,500,1000,5000,10000,UINT_MAX
	};
#if 0
	/* is this valid C? */
	static unsigned int nbs[sizeof(sizes)/sizeof(int)] = { [sizeof(sizes)/sizeof(int)-1] 0 };
#else
	static unsigned int nbs[sizeof(sizes)/sizeof(int)];
#endif
	static unsigned int nb;
	unsigned int      *seqp;
	CRTCPIPBuffer *udptcpip_buffer;
	int i;

#if 1
	for (i = 0; i < sizeof(sizes) / sizeof(unsigned int); i++)
		nbs[i] = 0;
#endif

	if (!bufp) {
		crDebug("writing safely %d bytes because from user memory",len);
		crUDPTCPIPSend( conn, bufp, start, len);
		return;
	}
	if (len > conn->mtu) {
		crDebug("writing safely %d bytes because that is too much for MTU %d", len, conn->mtu);
		crUDPTCPIPSend( conn, bufp, start, len);
		return;
	}

	if ( barflen+len > barflen )
	{
		barflen+=len;
		nb++;
		for(i=0;;i++)
			if (len > sizes[i] && len <= sizes[(i+1)]) {
				nbs[i]++;
				break;
			}
		if (barflen-barfblip>1<<22) {
			barfblip=barflen;
			crDebug( "send traffic: %d%sMo barfed %dKo safe", barflen/(1024*1024), barflen?"":".0", safelen/1024 );
			if (nb) {
				for (i=0;i<sizeof(sizes)/sizeof(int)-1;i++)
					fprintf(stderr,"%u:%u%s%% ",sizes[i],(nbs[i]*100)/nb,nbs[i]==0?".0":"");
				fprintf(stderr,"\n");
			}
		}
	}

	udptcpip_buffer = (CRTCPIPBuffer *)(*bufp) - 1;

	CRASSERT( udptcpip_buffer->magic == CR_TCPIP_BUFFER_MAGIC );

	seqp = (unsigned int *) start - 1;
	if (conn->swap)
	{
		*seqp = SWAP32(conn->seq);
	}
	else
	{
		*seqp = conn->seq;
	}
	crUDPIPWriteExact( conn, seqp, len + sizeof(*seqp) );

	/* reclaim this pointer for reuse and try to keep the client from
		 accidentally reusing it directly */
#ifdef CHROMIUM_THREADSAFE
	crLockMutex(&cr_tcpip.mutex);
#endif
	crBufferPoolPush( &cr_tcpip.bufpool, udptcpip_buffer );
#ifdef CHROMIUM_THREADSAFE
	crUnlockMutex(&cr_tcpip.mutex);
#endif
	*bufp = NULL;
}

extern void __dead_connection( CRConnection *conn );
extern int __crSelect( int n, fd_set *readfds, struct timeval *timeout );

static void crUDPTCPIPReceive( CRConnection *conn, CRTCPIPBuffer *buf, int len )
{
	CRMessage *msg;
	CRMessageType cached_type;
#if 0
	crLogRead( len );
#endif

	conn->recv_credits -= len;

	conn->total_bytes_recv +=  len;

	msg = (CRMessage *) (buf + 1);
	cached_type = msg->header.type;
	if (conn->swap)
	{
		msg->header.type = (CRMessageType) SWAP32( msg->header.type );
		msg->header.conn_id = (CRMessageType) SWAP32( msg->header.conn_id );
	}
	if (!cr_tcpip.recv( conn, buf + 1, len ))
	{
		crNetDefaultRecv( conn, buf + 1, len );
	}

	/* CR_MESSAGE_OPCODES is freed in
	 * crserver/server_stream.c 
	 *
	 * OOB messages are the programmer's problem.  -- Humper 12/17/01 */
	if (cached_type != CR_MESSAGE_OPCODES && cached_type != CR_MESSAGE_OOB)
	{
		crTCPIPFree( conn, buf + 1 );
	}
}

int crUDPTCPIPRecv( void )
{
	int    num_ready, max_fd;
	fd_set read_fds;
	int i;
	/* ensure we don't get caught with a new thread connecting */
	int num_conns = cr_tcpip.num_conns;

#ifdef CHROMIUM_THREADSAFE
	crLockMutex(&cr_tcpip.recvmutex);
#endif

	max_fd = 0;
	FD_ZERO( &read_fds );
	for ( i = 0; i < num_conns; i++ )
	{
		CRConnection *conn = cr_tcpip.conns[i];
		if (!conn) continue;
		if ( conn->recv_credits > 0 || conn->type != CR_UDPTCPIP )
		{
			/* 
			 * NOTE: may want to always put the FD in the descriptor
               		 * set so we'll notice broken connections.  Down in the
               		 * loop that iterates over the ready sockets only peek
               		 * (MSG_PEEK flag to recv()?) if the connection isn't
               		 * enabled. 
			 */
			CRSocket sock = conn->tcp_socket;

			if ( (int) sock + 1 > max_fd )
				max_fd = (int) sock + 1;
			FD_SET( sock, &read_fds );

			sock = conn->udp_socket;
			if ( (int) sock + 1 > max_fd )
				max_fd = (int) sock + 1;
			FD_SET( sock, &read_fds );
		}
	}

	if (!max_fd) {
#ifdef CHROMIUM_THREADSAFE
		crUnlockMutex(&cr_tcpip.recvmutex);
#endif
		return 0;
	}

	if ( num_conns )
	{
		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 500;
		num_ready = __crSelect( max_fd, &read_fds, &timeout );
	}
	else
	{
		crWarning( "Waiting for first connection..." );
		num_ready = __crSelect( max_fd, &read_fds, NULL );
	}

	if ( num_ready == 0 ) {
#ifdef CHROMIUM_THREADSAFE
		crUnlockMutex(&cr_tcpip.recvmutex);
#endif
		return 0;
	}

	for ( i = 0; i < num_conns; i++ )
	{
		CRConnection     *conn = cr_tcpip.conns[i];
		CRTCPIPBuffer *buf;
		int read_ret;
		int len;
		int sock;

		if (!conn) continue;

		if ( conn->type != CR_UDPTCPIP )
			continue;

		if ( conn->udp_packet ) {
			unsigned int *seq;
			buf = conn->udp_packet;
			seq = (unsigned int *)(buf + 1) - 1;
			if ( *seq == conn->ack )
			{
				crUDPTCPIPReceive( conn, buf,
						conn->udp_packetlen );
				conn->udp_packet = NULL;
				i--;	/* can now read other packets */
				continue;
			}
			if ( *seq - conn->ack > (~(0U)) >> 1 )
			{
				crError( "%u is older than %u ?!", *seq, conn->ack );
				crTCPIPFree( conn, buf + 1 );
				conn->udp_packet = NULL;
				i--;	/* can now read other packets */
				continue;
			}
			/* still too early, wait for TCP data */
		}
		else if ( FD_ISSET(conn->udp_socket, &read_fds ) )
		{
			CRTCPIPBuffer *buf = ((CRTCPIPBuffer *) crTCPIPAlloc( conn )) - 1;
			unsigned int *seq = ((unsigned int *) (buf + 1)) - 1;
			int len;

			len = recv( conn->udp_socket, (void *)seq,
				buf->allocated + sizeof(*seq), MSG_TRUNC );

			CRASSERT( len > 0 );
			CRASSERT( (unsigned int)len <= buf->allocated + sizeof(*seq) );
			if ( len < sizeof(*seq) )
			{
				crWarning( "too short a UDP packet : %d", len);
				crTCPIPFree( conn, buf + 1 );
				continue;
			}

			buf->len = len;

			if ( *seq == conn->ack)
			{
				crUDPTCPIPReceive( conn, buf, len );
				continue;
			}

			if ( *seq - conn->ack > (~(0U)) >> 1 )
			{
				crWarning( "%u is older than %u, dropping", *seq, conn->ack );
				crTCPIPFree( conn, buf + 1 );
				continue;
			}
			conn->udp_packet = buf;
			conn->udp_packetlen = len;
		}

		sock = conn->tcp_socket;
		if ( !FD_ISSET( sock, &read_fds ) )
			continue;

		read_ret = __read_exact( sock, &len, sizeof(len) );
		if ( read_ret <= 0 )
		{
			__dead_connection( conn );
			i--;
			continue;
		}

		if (conn->swap)
		{
			len = SWAP32(len);
		}

		CRASSERT( len > 0 );

		if ( (unsigned int)len <= conn->buffer_size )
		{
			buf = (CRTCPIPBuffer *) crTCPIPAlloc( conn ) - 1;
		}
		else
		{
			buf = (CRTCPIPBuffer *) 
				crAlloc( sizeof(*buf) + len );
			buf->magic = CR_TCPIP_BUFFER_MAGIC;
			buf->kind  = CRTCPIPMemoryBig;
			buf->pad   = 0;
		}

		buf->len = len;

		read_ret = __read_exact( sock, buf + 1, len );
		if ( read_ret <= 0 )
		{
			crWarning( "Bad juju: %d %d", buf->allocated, len );
			crFree( buf );
			__dead_connection( conn );
			i--;
			continue;
		}

		crUDPTCPIPReceive( conn, buf, len );
		conn->ack++;
	}

#ifdef CHROMIUM_THREADSAFE
	crUnlockMutex(&cr_tcpip.recvmutex);
#endif

	return 1;
}

extern void crTCPIPInit( CRNetReceiveFunc recvFunc, CRNetCloseFunc closeFunc, unsigned int mtu );
void crUDPTCPIPInit( CRNetReceiveFunc recvFunc, CRNetCloseFunc closeFunc, unsigned int mtu )
{
	crTCPIPInit( recvFunc, closeFunc, mtu );
}

/* The function that actually connects.  This should only be called by clients 
 * Servers have another way to set up the socket. */

extern int crTCPIPDoConnect( CRConnection *conn );
static int crUDPTCPIPDoConnect( CRConnection *conn )
{
#ifdef WINDOWS
	unsigned short port;
#else
	in_port_t port;
#endif
#ifdef LINUX
	int err;
	int opt = IP_PMTUDISC_DO;
#endif
#ifndef ADDRINFO
	struct hostent *hp;
#else
	char port_s[NI_MAXSERV];
	struct addrinfo *res,*cur;
	struct addrinfo hints;
#endif
	
	/* first connect to its tcp port */
	if ( !crTCPIPDoConnect( conn ) )
		return 0;

	/* read its UDP port */
	crTCPIPReadExact( conn->tcp_socket, &port, sizeof( port ) );
	port = ntohs(port);

	crDebug( "Server's UDP port is %d", port);

	/* and connect to it */
#ifndef ADDRINFO
	hp = gethostbyname( conn->hostname );
	if ( !hp )
	{
		crWarning( "Unknown host: \"%s\"", conn->hostname );
		return 0;
	}
	conn->udp_socket = socket( AF_INET, SOCK_DGRAM, 0 );
	memset(&conn->remoteaddr, 0, sizeof(conn->remoteaddr));
	conn->remoteaddr.sin_family = AF_INET;
	conn->remoteaddr.sin_port = htons( (short) port );

	memcpy( (char *) &conn->remoteaddr.sin_addr, hp->h_addr,
			sizeof(conn->remoteaddr.sin_addr) );
	if ( conn->udp_socket >= 0 ) {
		if ( connect( conn->udp_socket, (struct sockaddr *) &conn->remoteaddr,
					sizeof(conn->remoteaddr) ) == -1 )
			crWarning( "Couldn't connect UDP socket : %s", crTCPIPErrorString( crTCPIPErrno( ) ) );
	} else {
#else
	sprintf(port_s, "%u", port);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF;
	hints.ai_socktype = SOCK_DGRAM;

	err = getaddrinfo( conn->hostname, port_s, &hints, &res);
	if ( err )
	{
		crWarning( "Unknown host: \"%s\": %s", conn->hostname, gai_strerror(err) );
		return 0;
	}

	for (cur=res;cur;)
	{
		conn->udp_socket = socket( cur->ai_family, cur->ai_socktype, cur->ai_protocol );
		if ( conn->udp_socket >= 0 )
		{
			if ( connect( conn->udp_socket, cur->ai_addr, cur->ai_addrlen ) == -1 )
				crWarning( "Couldn't connect UDP socket : %s", crTCPIPErrorString( crTCPIPErrno( ) ) );
			break;
		}
		err = crTCPIPErrno( );
		if (err != EAFNOSUPPORT)
			crWarning( "socket error: %s, trying another way", crTCPIPErrorString( err ) );
		cur=cur->ai_next;
	}
	if (!cur) {
		freeaddrinfo(res);
#endif
		crWarning( "Couldn't find any suitable way to connect to %s:%d", conn->hostname, port );
		return 0;
	}

#ifdef LINUX
	if ( setsockopt(conn->udp_socket, SOL_IP, IP_MTU_DISCOVER, &opt, sizeof(opt)) == -1 )
	{
		err = crTCPIPErrno( );
		crWarning( "MTU discovery can't be activated : %s", crTCPIPErrorString( err ) );
	}
	else
	{
		socklen_t len = sizeof(opt);
		if ( getsockopt(conn->udp_socket, SOL_IP, IP_MTU, &opt, &len) == -1 )
			crWarning( "MTU couldn't be got : %s", crTCPIPErrorString( crTCPIPErrno ( ) ) );
		else
			if ( len != sizeof(opt) )
				crWarning( "Unexpected length %d for MTU option length", len );
			else
			{
				opt -= sizeof(conn->seq) + sizeof(struct udphdr) + sizeof(struct ip6_hdr);
				crDebug( "MTU is (for now) %d", opt );
				conn->mtu = opt;
			}
	}
#endif
#ifdef ADDRINFO
	crMemcpy(&conn->remoteaddr, cur->ai_addr, cur->ai_addrlen);
	freeaddrinfo(res);
#endif
	return 1;
}

extern void crTCPIPDoDisconnect( CRConnection *conn );
static void crUDPTCPIPDoDisconnect( CRConnection *conn )
{
	crCloseSocket( conn->udp_socket );
	crTCPIPDoDisconnect( conn );
}

extern void crTCPIPConnection( CRConnection *conn );
void crUDPTCPIPConnection( CRConnection *conn )
{
	crTCPIPConnection( conn );

	conn->type  = CR_UDPTCPIP;
	conn->Send  = crUDPTCPIPSend;
	conn->Barf  = crUDPTCPIPBarf;
	conn->SendExact  = NULL;
	conn->Recv  = NULL;	/* none for UDP : *must* multiplex ! */
	conn->Accept = crUDPTCPIPAccept;
	conn->Connect = crUDPTCPIPDoConnect;
	conn->Disconnect = crUDPTCPIPDoDisconnect;
	conn->seq   = 0;
	conn->ack   = 0;
	conn->udp_packet = NULL;
	conn->mtu  -= sizeof(conn->seq); /* some room for seq */
}
