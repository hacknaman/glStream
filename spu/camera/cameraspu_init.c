/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include <stdio.h>
#include "cr_spu.h"
#include "cameraspu.h"

/** Camera SPU descriptor */ 
CameraSPU camera_spu;

/** SPU functions */
static SPUFunctions camera_functions = {
	NULL, /**< CHILD COPY */
	NULL, /**< DATA */
	_cr_camera_table /**< THE ACTUAL FUNCTIONS - pointer to NamedFunction table */
};

/**
 * Camera spu init function
 * \param id
 * \param child
 * \param self
 * \param context_id
 * \param num_contexts
 */
static SPUFunctions *
cameraSPUInit( int id, SPU *child, SPU *self,
								 unsigned int context_id,
								 unsigned int num_contexts )
{

	(void) self;
	(void) context_id;
	(void) num_contexts;

    camera_spu.id = id;
    camera_spu.has_child = 0;
    camera_spu.server = NULL;
	if (child)
	{
        crSPUInitDispatchTable(&(camera_spu.child));
        crSPUCopyDispatchTable(&(camera_spu.child), &(child->dispatch_table));
        camera_spu.has_child = 1;
	}
    crSPUInitDispatchTable(&(camera_spu.super));
    crSPUCopyDispatchTable(&(camera_spu.super), &(self->superSPU->dispatch_table));
	cameraspuGatherConfiguration();

	return &camera_functions;
}

static void
cameraSPUSelfDispatch(SPUDispatchTable *self)
{
    crSPUInitDispatchTable(&(camera_spu.self));
    crSPUCopyDispatchTable(&(camera_spu.self), self);

    camera_spu.server = (CRServer *)(self->server);
}

static int
cameraSPUCleanup(void)
{
	return 1;
}

int
SPULoad( char **name, char **super, SPUInitFuncPtr *init,
				 SPUSelfDispatchFuncPtr *self, SPUCleanupFuncPtr *cleanup,
				 SPUOptionsPtr *options, int *flags )
{
	*name = "camera";
	*super = "passthrough";
	*init = cameraSPUInit;
	*self = cameraSPUSelfDispatch;
	*cleanup = cameraSPUCleanup;
	*options = cameraSPUOptions;
	*flags = (SPU_NO_PACKER|SPU_NOT_TERMINAL|SPU_MAX_SERVERS_ZERO);
	
	return 1;
}
