#include "server.h"
#include "server.h"
#include "cr_net.h"
#include "cr_unpack.h"
#include "cr_protocol.h"
#include "cr_error.h"
#include "cr_mem.h"

typedef struct __runqueue {
	CRClient *client;
	int blocked;
	struct __runqueue *next;
	struct __runqueue *prev;
} RunQueue;

RunQueue *run_queue = NULL;

void crServerAddToRunQueue( int index )
{
	CRClient *client = cr_server.clients + index;
	RunQueue *q = (RunQueue *) crAlloc( sizeof( *q ) );
	q->client = client;
	q->blocked = 0;
	if (!run_queue)
	{
		run_queue = q;
		q->next = q;
		q->prev = q;
	}
	else
	{
		q->next = run_queue->next;
		run_queue->next->prev = q;

		q->prev = run_queue;
		run_queue->next = q;
	}
}

static RunQueue *__getNextClient(void)
{
	for (;;)
	{
		if (run_queue)
		{
			int all_blocked = 1;
			RunQueue *start = run_queue;
			while (run_queue->next != start)
			{
				if (!run_queue->blocked)
				{
					all_blocked = 0;
				}
				if (!run_queue->blocked && run_queue->client->conn->messageList)
				{
					return run_queue;
				}
				run_queue = run_queue->next;
			}

			if (!run_queue->blocked)
			{
				all_blocked = 0;
			}
			if (!run_queue->blocked && run_queue->client->conn->messageList)
			{
				return run_queue;
			}
			if (all_blocked)
			{
				crError( "DEADLOCK!" );
			}
		}
		// no one had any work, get some!
		crNetRecv();
	}
	/* UNREACHED */
	/* return NULL; */
}

void crServerSerializeRemoteStreams(void)
{
	for (;;)
	{
		RunQueue *q = __getNextClient();
		CRClient *client = q->client;
		CRMessage *msg;
		int len;

		cr_server.current_connection = client->conn;
		crStateMakeCurrent( client->ctx ); 
		for( len = crNetGetMessage( cr_server.current_connection, &msg );
				 len;
				 len = crNetGetMessage( cr_server.current_connection, &msg ) )
		{
			CRMessageOpcodes *msg_opcodes;
			char *data_ptr;
			if (msg->type != CR_MESSAGE_OPCODES)
			{
				crError( "SPU %d sent me CRAP (type=%d)", client->spu_id, msg->type );
			}

			msg_opcodes = (CRMessageOpcodes *) msg;
			data_ptr = (char *) msg_opcodes + sizeof( *msg_opcodes) + ((msg_opcodes->numOpcodes + 3 ) & ~0x03);
			crUnpack( data_ptr, data_ptr-1, msg_opcodes->numOpcodes, &(cr_server.dispatch) );
			crNetFree( cr_server.current_connection, msg );
		}
	}
}

int crServerRecv( CRConnection *conn, void *buf, unsigned int len )
{
	return 0; // no handling -- let 'em queue up!
#if 0
	CRMessage *msg = (CRMessage *) buf;
	CRMessageOpcodes *ops;
	char *data_ptr;

	(void) len;
	switch(msg->type)
	{
		case CR_MESSAGE_OPCODES:
			ops = (CRMessageOpcodes *) msg;
			data_ptr = (char *) buf + sizeof(*ops) + (( ops->numOpcodes +3 ) & ~0x03);

			crUnpack( data_ptr,data_ptr-1,ops->numOpcodes,&(cr_server.dispatch) );
			crNetFree( conn, msg );
			break;
		default:
			crWarning( "Bad message type in crServerRecv: %d\n", msg->type );
			return 0; // NOT HANDLED
	}
	return 1; // HANDLED
#endif
}
