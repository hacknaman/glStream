/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "server.h"
#include "cr_net.h"
#include "cr_unpack.h"
#include "cr_error.h"
#include "cr_glstate.h"
#include "cr_string.h"
#include "cr_mem.h"
#include "cr_hash.h"
#include <signal.h>
#include <stdlib.h>

CRServer cr_server;

int CRServerMain( int argc, char *argv[] );

static void DeleteBarrierCallback( void *data )
{
	CRServerBarrier *barrier = (CRServerBarrier *) data;
	crFree(barrier->waiting);
	crFree(barrier);
}


static void crServerTearDown( void )
{
	SPU *the_spu = cr_server.head_spu;
	unsigned int i;

	crStateSetCurrent( NULL );

	/* Free all context info */
	for (i = 0; i < CR_MAX_CONTEXTS; i++) {
		if (cr_server.context[i] != NULL) {
			crStateDestroyContext( cr_server.context[i] );
		}
	}

	cr_server.curClient = NULL;
	crFree( cr_server.clients );
	crFree( cr_server.overlap_intens );

	/* Deallocate all semaphores */
	crFreeHashtable(cr_server.semaphores, crFree);
 
	/* Deallocate all barriers */
	crFreeHashtable(cr_server.barriers, DeleteBarrierCallback);

	while (1) {
		if (the_spu && the_spu->cleanup) {
			crWarning("Cleaning up SPU %s",the_spu->name);
			the_spu->cleanup();
		} else 
			break;
		the_spu = the_spu->superSPU;
	}
}

static void crServerClose( unsigned int id )
{
	crError( "Client disconnected!" );
	(void) id;
}

static void crServerCleanup( int sigio )
{
	crServerTearDown();

	exit(0);
}

int CRServerMain( int argc, char *argv[] )
{
	int i;
	unsigned int j;
	char *mothership = NULL;
	CRMuralInfo *defaultMural;

	for (i = 1 ; i < argc ; i++)
	{
		if (!crStrcmp( argv[i], "-mothership" ))
		{
			if (i == argc - 1)
			{
				crError( "-mothership requires an argument" );
			}
			mothership = argv[i+1];
			i++;
		}
		if (!crStrcmp( argv[i], "-port" ))
		{
			if (i == argc - 1)
			{
				crError( "-port requires an argument" );
			}
			cr_server.tcpip_port = crStrToInt(argv[i+1]);
			i++;
		}
	}

	signal( SIGTERM, crServerCleanup );
	signal( SIGINT, crServerCleanup );
#ifndef WINDOWS
	signal( SIGPIPE, SIG_IGN );
#endif

	cr_server.firstCallCreateContext = GL_TRUE;
	cr_server.firstCallMakeCurrent = GL_TRUE;

	/*
	 * Create default mural info and hash table.
	 */
	cr_server.muralTable = crAllocHashtable();
	defaultMural = (CRMuralInfo *) crCalloc(sizeof(CRMuralInfo));
	crHashtableAdd(cr_server.muralTable, 0, defaultMural);

	crNetInit(crServerRecv, crServerClose);
	crStateInit();

	crServerGatherConfiguration(mothership);

	crStateLimitsInit( &(cr_server.limits) );

	for (j = 0 ; j < cr_server.numClients ; j++)
	{
		crServerAddToRunQueue( &cr_server.clients[j] );
	}

	crServerInitDispatch();
	crStateDiffAPI( &(cr_server.head_spu->dispatch_table) );

	crUnpackSetReturnPointer( &(cr_server.return_ptr) );
	crUnpackSetWritebackPointer( &(cr_server.writeback_ptr) );

	cr_server.barriers = crAllocHashtable();
	cr_server.semaphores = crAllocHashtable();

	crServerSerializeRemoteStreams();

	crServerTearDown();

	return 0;
}


#if 0
int main( int argc, char *argv[] )
{
	return CRServerMain( argc, argv );
}
#endif
