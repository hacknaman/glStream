/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "cr_spu.h"
#include "readbackspu.h"
#include <stdio.h>

extern SPUNamedFunctionTable readback_table[];

SPUFunctions the_functions = {
	NULL, /* CHILD COPY */
	NULL, /* DATA */
	readback_table /* THE ACTUAL FUNCTIONS */
};

SPUFunctions *SPUInit( int id, SPU *child, SPU *super,
		unsigned int context_id,
		unsigned int num_contexts )
{

	(void) super;
	(void) context_id;
	(void) num_contexts;

	readback_spu.id = id;
	readback_spu.has_child = 0;
	if (child)
	{
		crSPUInitDispatchTable( &(readback_spu.child) );
		crSPUCopyDispatchTable( &(readback_spu.child), &(child->dispatch_table) );
		readback_spu.has_child = 1;
	}
	crSPUInitDispatchTable( &(readback_spu.super) );
	crSPUCopyDispatchTable( &(readback_spu.super), &(super->dispatch_table) );
	readbackspuGatherConfiguration();

	return &the_functions;
}

void SPUSelfDispatch(SPUDispatchTable *self)
{
	crSPUInitDispatchTable( &(readback_spu.self) );
	crSPUCopyDispatchTable( &(readback_spu.self), self );
}

int SPUCleanup(void)
{
	return 1;
}

int SPULoad( char **name, char **super, SPUInitFuncPtr *init,
	SPUSelfDispatchFuncPtr *self, SPUCleanupFuncPtr *cleanup )
{
	*name = "readback";
	*super = "render";
	*init = SPUInit;
	*self = SPUSelfDispatch;
	*cleanup = SPUCleanup;
	
	return 1;
}
