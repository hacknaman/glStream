#include "cr_spu.h"
#include "cr_mem.h"
#include "tilesortspu.h"
#include <stdio.h>

extern SPUNamedFunctionTable tilesort_table[];
TileSortSPU tilesort_spu;

SPUFunctions the_functions = {
	NULL, // CHILD COPY
	NULL, // DATA
	tilesort_table // THE ACTUAL FUNCTIONS
};

SPUFunctions *SPUInit( int id, SPU *child, SPU *super,
		unsigned int context_id,
		unsigned int num_contexts )
{
	int i;

	(void) context_id;
	(void) num_contexts;
	(void) child;
	(void) super;


	tilesort_spu.id = id;
	tilesortspuCreateFunctions();
	tilesortspuGatherConfiguration();
	tilesortspuConnectToServers();

	// We need to mess with the pack size of the geometry buffer, since we
	// may be using BoundsInfo packes, etc, etc.  This is yucky.
	
	tilesort_spu.geom_pack_size = tilesort_spu.MTU;
	tilesort_spu.geom_pack_size -= sizeof( CRMessageOpcodes );
	tilesort_spu.geom_pack_size -= 4;
	if (tilesort_spu.sendBounds)
	{
		// Some server has multiple tiles, so we need to shrink everything
		// to fit in the DATA part of the server's send buffer.  Yuck.
		tilesort_spu.geom_pack_size = (tilesort_spu.geom_pack_size * 4) / 5;
		tilesort_spu.geom_pack_size -= (24 + 1); // 24 is the size of the BoundsInfo packet
	}

	// need to have the ctx first so we can give it as an argument o
	// crPackFlushArg.
	crStateInit();
	tilesort_spu.ctx = crStateCreateContext();

	crPackInitBuffer( &(tilesort_spu.geometry_pack), crAlloc( tilesort_spu.geom_pack_size ), 
			              tilesort_spu.geom_pack_size, END_FLUFF );
	crPackSetBuffer( &(tilesort_spu.geometry_pack) );
	crPackFlushFunc( tilesortspuFlush );
	crPackFlushArg( tilesort_spu.ctx );
	crPackSendHugeFunc( tilesortspuHuge );
	crPackResetBBOX();

	crStateMakeCurrent( tilesort_spu.ctx );
	crStateFlushArg( tilesort_spu.ctx );
	tilesortspuCreateDiffAPI();
	crStateSetCurrentPointers( tilesort_spu.ctx, &(cr_packer_globals.current) );
	tilesort_spu.ctx->current.current->vtx_count = 0;

	tilesort_spu.pinchState.numRestore = 0;
	tilesort_spu.pinchState.wind = 0;
	tilesort_spu.pinchState.isLoop = 0;

	for (i = 0 ; i < tilesort_spu.num_servers; i++)
	{
		TileSortSPUServer *server = tilesort_spu.servers + i;
		server->ctx = crStateCreateContext();
		server->ctx->current.rasterPos.x = server->ctx->current.rasterPosPre.x = (float) server->x1[0];
		server->ctx->current.rasterPos.y = server->ctx->current.rasterPosPre.y = (float) server->y1[0];
		crPackInitBuffer( &(server->pack), crNetAlloc( server->net.conn ), server->net.buffer_size, 0 );
	}

	tilesortspuBucketingInit();

	return &the_functions;
}

void SPUSelfDispatch(SPUDispatchTable *self)
{
	crSPUCopyDispatchTable( &(tilesort_spu.self), self );
}

int SPUCleanup(void)
{
	return 1;
}

int SPULoad( char **name, char **super, SPUInitFuncPtr *init,
	SPUSelfDispatchFuncPtr *self, SPUCleanupFuncPtr *cleanup )
{
	*name = "tilesort";
	*super = NULL;
	*init = SPUInit;
	*self = SPUSelfDispatch;
	*cleanup = SPUCleanup;
	
	return 1;
}
