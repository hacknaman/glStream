/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "cr_spu.h"
#include "wetspu.h"
#include <stdio.h>

extern SPUNamedFunctionTable wet_table[];
WetSPU wet_spu;

SPUFunctions wet_functions = {
	NULL, /* CHILD COPY */
	NULL, /* DATA */
	wet_table /* THE ACTUAL FUNCTIONS */
};

SPUFunctions *wetSPUInit( int id, SPU *child, SPU *super,
		unsigned int context_id,
		unsigned int num_contexts )
{

	(void) super;
	(void) context_id;
	(void) num_contexts;

	wet_spu.id = id;
	wet_spu.has_child = 0;
	wet_spu.server = NULL;
	if (child)
	{
		crSPUInitDispatchTable( &(wet_spu.child) );
		crSPUCopyDispatchTable( &(wet_spu.child), &(child->dispatch_table) );
		wet_spu.has_child = 1;
	}
	crSPUInitDispatchTable( &(wet_spu.super) );
	crSPUCopyDispatchTable( &(wet_spu.super), &(super->dispatch_table) );
	wetspuGatherConfiguration();

	wet_spu.frame_counter = 0;
	wet_spu.drops = NULL;

	return &wet_functions;
}

void wetSPUSelfDispatch(SPUDispatchTable *self)
{
	crSPUInitDispatchTable( &(wet_spu.self) );
	crSPUCopyDispatchTable( &(wet_spu.self), self );

	wet_spu.server = (CRServer *)(self->server);
}

int wetSPUCleanup(void)
{
	return 1;
}

extern SPUOptions wetSPUOptions[];

int SPULoad( char **name, char **super, SPUInitFuncPtr *init,
	     SPUSelfDispatchFuncPtr *self, SPUCleanupFuncPtr *cleanup,
	     SPUOptionsPtr *options )
{
	*name = "wet";
	*super = "passthrough";
	*init = wetSPUInit;
	*self = wetSPUSelfDispatch;
	*cleanup = wetSPUCleanup;
	*options = wetSPUOptions;
	
	return 1;
}
