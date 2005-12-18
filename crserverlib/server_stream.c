/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "server.h"
#include "cr_unpack.h"
#include "cr_error.h"
#include "cr_mem.h"
#include "server_dispatch.h"


/**
 * Accept a new client connection, create a new CRClient and add to run queue.
 */
void
crServerAddNewClient(void)
{
	CRClient *newClient = (CRClient *) crCalloc(sizeof(CRClient));

	if (newClient) {
		newClient->spu_id = cr_server.client_spu_id;
		newClient->conn = crNetAcceptClient( cr_server.protocol, NULL,
																				 cr_server.tcpip_port,
																				 cr_server.mtu, 1 );
		newClient->currentCtx = cr_server.DummyContext;
		newClient->currentTranslator = NULL;

		/* add to array */
		cr_server.clients[cr_server.numClients++] = newClient;

		crServerAddToRunQueue( newClient );
	}
}


/**
 * Check if client is in the run queue.
 */
static GLboolean
FindClientInQueue(CRClient *client)
{
	RunQueue *q = cr_server.run_queue;
	while (q) {
		if (q->client == client) {
			return 1;
		}
		q = q->next;
		if (q == cr_server.run_queue)
			return 0; /* back head */
	}
	return 0;
}


#if 0
static int
PrintQueue(void)
{
	RunQueue *q = cr_server.run_queue;
	int count = 0;
	crDebug("Queue entries:");
	while (q) {
		count++;
		crDebug("Entry: %p  client: %p", q, q->client);
		q = q->next;
		if (q == cr_server.run_queue)
			return count;
	}
	return count;
}
#endif


void crServerAddToRunQueue( CRClient *client )
{
	static int clientId = 1;
	RunQueue *q = (RunQueue *) crAlloc( sizeof( *q ) );

	/* give this client a unique number if needed */
	if (!client->number) {
		client->number = clientId++;
	}

	crDebug("Adding client %p to the run queue", client);

	if (FindClientInQueue(client)) {
		crError("CRServer: client %p already in the queue!", client);
	}

	q->client = client;
	q->blocked = 0;

	if (!cr_server.run_queue)
	{
		/* adding to empty queue */
		cr_server.run_queue = q;
		q->next = q;
		q->prev = q;
	}
	else
	{
		/* insert in doubly-linked list */
		q->next = cr_server.run_queue->next;
		cr_server.run_queue->next->prev = q;

		q->prev = cr_server.run_queue;
		cr_server.run_queue->next = q;
	}
}



static void crServerDeleteClient( CRClient *client )
{
	int i, j;

	crDebug("Deleting client %p (%d msgs left)", client,
					client->conn->messageList.numMessages);

	if (client->conn->messageList.numMessages > 0) {
		crDebug("Delay destroying client: message still pending");
		return;
	}

	if (!FindClientInQueue(client)) {
		crError("CRServer: client %p not found in the queue!", client);
	}

	/* remove from clients[] array */
	for (i = 0; i < cr_server.numClients; i++) {
		if (cr_server.clients[i] == client) {
			/* found it */
			for (j = i; j < cr_server.numClients - 1; j++)
				cr_server.clients[j] = cr_server.clients[j + 1];
			cr_server.numClients--;
			break;
		}
	}

	/* remove from the run queue */
	if (cr_server.run_queue)
	{
		RunQueue *q = cr_server.run_queue;
		RunQueue *qStart = cr_server.run_queue; 
		do {
			if (q->client == client)
			{
				/* this test seems a bit excessive */
				if ((q->next == q->prev) && (q->next == q) && (cr_server.run_queue == q))
				{
					/* We're removing/deleting the only client */
					CRASSERT(cr_server.numClients == 0);
					crFree(q);
					cr_server.run_queue = NULL;
					cr_server.curClient = NULL;
					crDebug("Last client deleted - empty run queue.");
					/* XXX add a config option to specify whether the crserver
					 * should exit when there's no more clients.
					 */
				} 
				else
				{
					/* remove from doubly linked list and free the node */
					if (cr_server.curClient == q->client)
						cr_server.curClient = NULL;
					if (cr_server.run_queue == q)
						cr_server.run_queue = q->next;
					q->prev->next = q->next;
					q->next->prev = q->prev;
					crFree(q);
				}
				break;
			}
			q = q->next;
		} while (q != qStart);
	}

	crMemZero(client, sizeof(CRClient)); /* just to be safe */

	crFree(client);
}


/**
 * Find the next client in the run queue that's not blocked and has a
 * waiting message.
 * Check if all clients are blocked (on barriers, semaphores), if so we've
 * deadlocked!
 * If no clients have a waiting message, call crNetRecv to get something
 * if 'block' is true, else return NULL if 'block' if false.
 */
static RunQueue *
getNextClient(GLboolean block)
{
	while (1)
	{
		if (cr_server.run_queue) 
		{
			GLboolean all_blocked = GL_TRUE;
			GLboolean done_something = GL_FALSE;
			RunQueue *start = cr_server.run_queue;

 			if (!cr_server.run_queue->client->conn
					 || (cr_server.run_queue->client->conn->type == CR_NO_CONNECTION
							 && cr_server.curClient->conn->messageList.numMessages == 0)) {
				crDebug("Delete client %p at %d", cr_server.run_queue->client, __LINE__);
 				crServerDeleteClient( cr_server.run_queue->client );
				start = cr_server.run_queue;
			}
 
 			if (cr_server.run_queue == NULL) {
				/* empty queue */
 				return NULL;
			}

			while (!done_something || cr_server.run_queue != start)
			{
				done_something = GL_TRUE;
				if (!cr_server.run_queue->blocked)
				{
					all_blocked = GL_FALSE;
				}
				if (!cr_server.run_queue->blocked
						&& cr_server.run_queue->client->conn
						&& cr_server.run_queue->client->conn->messageList.head)
				{
					/* OK, this client isn't blocked and has a queued message */
					return cr_server.run_queue;
				}
				cr_server.run_queue = cr_server.run_queue->next;
			}

			if (all_blocked)
			{
				 /* XXX crError is fatal?  Should this be an info/warning msg? */
				crError( "crserver: DEADLOCK! (numClients=%d, all blocked)",
								 cr_server.numClients );
				if (cr_server.numClients < (int) cr_server.maxBarrierCount) {
					crError("Waiting for more clients!!!");
					while (cr_server.numClients < (int) cr_server.maxBarrierCount) {
						crNetRecv();
					}
				}
			}
		}

		if (!block)
			 return NULL;

		/* no one had any work, get some! */
		crNetRecv();

	} /* while */

	/* UNREACHED */
	/* return NULL; */
}


/**
 * This function takes the given message (which should be a buffer of
 * rendering commands) and executes it.
 */
static void
crServerDispatchMessage(CRMessage *msg)
{
	 const CRMessageOpcodes *msg_opcodes;
	 int opcodeBytes;
	 const char *data_ptr;

	 CRASSERT(msg->header.type == CR_MESSAGE_OPCODES);

	 msg_opcodes = (const CRMessageOpcodes *) msg;
	 opcodeBytes = (msg_opcodes->numOpcodes + 3) & ~0x03;

	 data_ptr = (const char *) msg_opcodes + sizeof(CRMessageOpcodes) + opcodeBytes;
	 crUnpack(data_ptr,                 /* first command's operands */
						data_ptr - 1,             /* first command's opcode */
						msg_opcodes->numOpcodes,  /* how many opcodes */
						&(cr_server.dispatch));  /* the CR dispatch table */
}



typedef enum
{
	CLIENT_GONE = 1, /* the client has disconnected */
  CLIENT_NEXT = 2, /* we can advance to next client */
  CLIENT_MORE = 3  /* we need to keep servicing current client */
} ClientStatus;


static ClientStatus
crServerServiceClient(const RunQueue *qEntry)
{
	CRMessage *msg;

	/* set current client pointer */
	cr_server.curClient = qEntry->client;

	/* service current client as long as we can */
	while (1) {
		unsigned int len;

		/* Check if the current client connection has gone away */
		if (!cr_server.curClient->conn
				|| (cr_server.curClient->conn->type == CR_NO_CONNECTION
						&& cr_server.curClient->conn->messageList.numMessages == 0)) {
			crDebug("Delete client %p at %d", cr_server.run_queue->client, __LINE__);
			crServerDeleteClient( cr_server.curClient );
			return CLIENT_GONE;
		}

		/* Don't use GetMessage, because it pulls stuff off
		 * the network too quickly */
		len = crNetPeekMessage( cr_server.curClient->conn, &msg );
		if (len == 0) {
			/* No message ready at this time.
			 * See if we can advance to the next client, or if we're in the
			 * middle of something and need to stick with this client.
			 */
			if (cr_server.curClient->currentCtx &&
					(cr_server.curClient->currentCtx->lists.currentIndex != 0 ||
					 cr_server.curClient->currentCtx->current.inBeginEnd ||
					 cr_server.curClient->currentCtx->occlusion.currentQueryObject))
			{
				/* We're between glNewList/EndList or glBegin/End or inside a
				 * glBeginQuery/EndQuery sequence.
				 * We can't context switch because that'll screw things up.
				 */
				CRASSERT(!qEntry->blocked);
				crNetRecv();
#if 1
				continue; /* return CLIENT_MORE; */
#else
				return CLIENT_MORE;
#endif
			}
			else {
				/* get next client */
				return CLIENT_NEXT;
			}
		}
		else {
			/* Got a message of length 'len' bytes */
			/*crDebug("got %d bytes", len);*/
		}

		CRASSERT(len > 0);
		if (msg->header.type != CR_MESSAGE_OPCODES) {
			crError( "SPU %d sent me CRAP (type=0x%x)",
							 cr_server.curClient->spu_id, msg->header.type );
		}

		/* Do the context switch here.  No sense in switching before we
		 * really have any work to process.  This is a no-op if we're
		 * not really switching contexts.
		 *
		 * XXX This isn't entirely sound.  The crStateMakeCurrent() call
		 * will compute the state difference and dispatch it using
		 * the head SPU's dispatch table.
		 *
		 * This is a problem if this is the first buffer coming in,
		 * and the head SPU hasn't had a chance to do a MakeCurrent()
		 * yet (likely because the MakeCurrent() command is in the
		 * buffer itself).
		 *
		 * At best, in this case, the functions are no-ops, and
		 * are essentially ignored by the SPU.  In the typical
		 * case, things aren't too bad; if the SPU just calls
		 * crState*() functions to update local state, everything
		 * will work just fine.
		 *
		 * In the worst (but unusual) case where a nontrivial
		 * SPU is at the head of a crserver's SPU chain (say,
		 * in a multiple-tiered "tilesort" arrangement, as
		 * seen in the "multitilesort.conf" configuration), the
		 * SPU may rely on state set during the MakeCurrent() that
		 * may not be present yet, because no MakeCurrent() has
		 * yet been dispatched.
		 *
		 * This headache will have to be revisited in the future;
		 * for now, SPUs that could head a crserver's SPU chain
		 * will have to detect the case that their functions are
		 * being called outside of a MakeCurrent(), and will have
		 * to handle the situation gracefully.  (This is currently
		 * the case with the "tilesort" SPU.)
		 */

#if 0
		crStateMakeCurrent( cr_server.curClient->currentCtx );
#else
		crStateMakeCurrent( cr_server.curClient->currentCtx );

		/* Check if the current window is the one that the client wants to
		 * draw into.  If not, dispatch a MakeCurrent to active the proper
		 * window.
		 */
		if (cr_server.curClient) {
			 int clientWindow = cr_server.curClient->currentWindow;
			 int clientContext = cr_server.curClient->currentContextNumber;
			 if (clientWindow && clientWindow != cr_server.currentWindow) {
				 crServerDispatchMakeCurrent(clientWindow, 0, clientContext);
				 /*
				 CRASSERT(cr_server.currentWindow == clientWindow);
				 */
			 }
		}
#endif

		/* Force scissor, viewport and projection matrix update in
		 * crServerSetOutputBounds().
		 */
		cr_server.currentSerialNo = 0;

		/* Commands get dispatched here */
		crServerDispatchMessage(msg);

		crNetFree( cr_server.curClient->conn, msg );

		if (qEntry->blocked) {
			/* Note/assert: we should not be inside a glBegin/End or glNewList/
			 * glEndList pair at this time!
			 */
			return CLIENT_NEXT;
		}

	}
}



/**
 * Check if any of the clients need servicing.
 * If so, service one client and return.
 * Else, just return.
 */
void
crServerServiceClients(void)
{
	RunQueue *q;

	crNetRecv();  /* does not block */

	q = getNextClient(GL_FALSE); /* don't block */
	if (q) {
		ClientStatus stat = crServerServiceClient(q);
		if (stat == CLIENT_NEXT && cr_server.run_queue->next) {
			/* advance to next client */
			cr_server.run_queue = cr_server.run_queue->next;
		}
	}
}




/**
 * Main crserver loop.  Service connections from all connected clients.
 */
void
crServerSerializeRemoteStreams(void)
{
	while (1)
	{
#if 1 /** THIS CODE BLOCK SHOULD GO AWAY SOMEDAY **/
		ClientStatus stat;
		RunQueue *q = getNextClient(GL_TRUE); /* block */

		/* no more clients, quit */
		if (!q)
			return;

		stat = crServerServiceClient(q);

		/* check if all clients have gone away */
		if (!cr_server.run_queue)
			 return;

		/* advance to next client */
		cr_server.run_queue = cr_server.run_queue->next;
#else
		crServerServiceClients();
#endif
	}
}


/**
 * This will be called by the network layer when it's received a new message.
 */
int crServerRecv( CRConnection *conn, CRMessage *msg, unsigned int len )
{
	(void) len;

	switch( msg->header.type )
	{
		/* Called when using multiple threads */
		case CR_MESSAGE_NEWCLIENT:
			crServerAddNewClient();
			return 1;
		default:
			/*crWarning( "Why is the crserver getting a message of type 0x%x?",
				msg->header.type ); */
			;
	}
	return 0; /* not handled */
}
