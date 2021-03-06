/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "cr_mem.h"
#include "cr_spu.h"
#include "cr_glstate.h"
#include "passpackspu.h"
#include "cr_packfunctions.h"
#include <stdio.h>

extern SPUNamedFunctionTable _cr_pack_table[];

SPUFunctions pack_functions = {
	NULL, /* CHILD COPY */
	NULL, /* DATA */
	_cr_pack_table /* THE ACTUAL FUNCTIONS */
};

PackSPU pack_spu;

#ifdef CHROMIUM_THREADSAFE
CRtsd _PackTSD;
CRmutex _PackMutex;
#endif

static SPUFunctions *
packSPUInit( int id, SPU *child, SPU *self,
						 unsigned int context_id,
						 unsigned int num_contexts,
						 int *ImpThreadID)
{
	ThreadInfo *thread;

	(void) context_id;
	(void) num_contexts;
	(void) child;
	(void) self;

#ifdef CHROMIUM_THREADSAFE
	crInitTSD(&_PackTSD);
	crInitMutex(&_PackMutex);
#endif

	pack_spu.id = id;

	pack_spu.ImpThreadID = ImpThreadID;

	packspuGatherConfiguration( child );

	/* This connects to the server, sets up the packer, etc. */
	thread = packspuNewThread( crThreadID() );
	CRASSERT( thread == &(pack_spu.thread[0]) );

	packspuCreateFunctions();
	crStateInit();

	crSPUInitDispatchTable(&(pack_spu.super));
	crSPUCopyDispatchTable(&(pack_spu.super), &(self->superSPU->dispatch_table));

	return &pack_functions;
}

static void
packSPUSelfDispatch(SPUDispatchTable *self)
{
	(void)self;
}

static int
packSPUCleanup(void)
{
	return 1;
}

extern SPUOptions packSPUOptions[];

int SPULoad( char **name, char **super, SPUInitFuncPtr *init,
	     SPUSelfDispatchFuncPtr *self, SPUCleanupFuncPtr *cleanup,
	     SPUOptionsPtr *options, int *flags )
{
	*name = "passpack";
	*super = "passthrough";
	*init = packSPUInit;
	*self = packSPUSelfDispatch;
	*cleanup = packSPUCleanup;
	*options = packSPUOptions;
	*flags = (SPU_HAS_PACKER | SPU_NOT_TERMINAL | SPU_MAX_SERVERS_ONE);

	return 1;
}
