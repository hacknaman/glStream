/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "cr_spu.h"
#include "cr_mem.h"
#include "cr_packfunctions.h"
#include "tilesortspu.h"
#include <stdio.h>

extern SPUNamedFunctionTable tilesort_table[];
TileSortSPU tilesort_spu;

SPUFunctions tilesort_functions = {
	NULL, /* CHILD COPY */
	NULL, /* DATA */
	tilesort_table /* THE ACTUAL FUNCTIONS */
};

#ifdef CHROMIUM_THREADSAFE
CRmutex _TileSortMutex;
CRtsd _ThreadTSD;
#endif

extern void _math_init_eval(void);

SPUFunctions *tilesortSPUInit( int id, SPU *child, SPU *super,
		unsigned int context_id,
		unsigned int num_contexts )
{
	ThreadInfo *thread0 = &(tilesort_spu.thread[0]);

	(void) context_id;
	(void) num_contexts;
	(void) child;
	(void) super;

	crMemZero( &tilesort_spu, sizeof(TileSortSPU) );

#ifdef CHROMIUM_THREADSAFE
	crSetTSD(&_ThreadTSD, thread0);
	crInitMutex(&_TileSortMutex);
#endif

	_math_init_eval();

	tilesort_spu.id = id;
	tilesortspuGatherConfiguration( child );
	tilesortspuConnectToServers(); /* set up thread0's server connection */

	tilesort_spu.swap = thread0->net[0].conn->swap;

	tilesortspuInitThreadPacking( thread0 );

	tilesortspuCreateFunctions();

	crStateInit();
	tilesortspuCreateDiffAPI();
	tilesortspuBucketingInit();

	return &tilesort_functions;
}

void tilesortSPUSelfDispatch(SPUDispatchTable *self)
{
	crSPUInitDispatchTable( &(tilesort_spu.self) );
	crSPUCopyDispatchTable( &(tilesort_spu.self), self );
}

int tilesortSPUCleanup(void)
{
	return 1;
}

int SPULoad( char **name, char **super, SPUInitFuncPtr *init,
	SPUSelfDispatchFuncPtr *self, SPUCleanupFuncPtr *cleanup )
{
	*name = "tilesort";
	*super = NULL;
	*init = tilesortSPUInit;
	*self = tilesortSPUSelfDispatch;
	*cleanup = tilesortSPUCleanup;
	
	return 1;
}
