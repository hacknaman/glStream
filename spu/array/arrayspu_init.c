/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "cr_spu.h"
#include "arrayspu.h"
#include <stdio.h>

extern SPUNamedFunctionTable array_table[];

SPUFunctions array_functions = {
	NULL, /* CHILD COPY */
	NULL, /* DATA */
	array_table /* THE ACTUAL FUNCTIONS */
};

SPUFunctions *arraySPUInit( int id, SPU *child, SPU *super,
		unsigned int context_id,
		unsigned int num_contexts )
{

	(void) super;
	(void) context_id;
	(void) num_contexts;

	array_spu.id = id;
	array_spu.has_child = 0;
	if (child)
	{
		crSPUInitDispatchTable( &(array_spu.child) );
		crSPUCopyDispatchTable( &(array_spu.child), &(child->dispatch_table) );
		array_spu.has_child = 1;
	}
	crSPUInitDispatchTable( &(array_spu.super) );
	crSPUCopyDispatchTable( &(array_spu.super), &(super->dispatch_table) );
	arrayspuGatherConfiguration();

	crStateInit();
	array_spu.ctx = crStateCreateContext( NULL );
	crStateMakeCurrent( array_spu.ctx );

	return &array_functions;
}

void arraySPUSelfDispatch(SPUDispatchTable *self)
{
	crSPUInitDispatchTable( &(array_spu.self) );
	crSPUCopyDispatchTable( &(array_spu.self), self );
}

int arraySPUCleanup(void)
{
	return 1;
}

int SPULoad( char **name, char **super, SPUInitFuncPtr *init,
	SPUSelfDispatchFuncPtr *self, SPUCleanupFuncPtr *cleanup )
{
	*name = "array";
	*super = "passthrough";
	*init = arraySPUInit;
	*self = arraySPUSelfDispatch;
	*cleanup = arraySPUCleanup;
	
	return 1;
}
