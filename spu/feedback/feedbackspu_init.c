/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "cr_spu.h"
#include "cr_mothership.h"
#include "cr_environment.h"
#include "cr_string.h"
#include "cr_error.h"
#include "cr_mem.h"
#include "cr_server.h"
#include "feedbackspu.h"
#include <fcntl.h>
#ifndef WINDOWS
#include <unistd.h>
#endif

extern SPUNamedFunctionTable feedback_table[];
feedbackSPU feedback_spu;

SPUFunctions feedback_functions = {
	NULL, /* CHILD COPY */
	NULL, /* DATA */
	feedback_table /* THE ACTUAL FUNCTIONS */
};

SPUFunctions *feedbackSPUInit( int id, SPU *child, SPU *self,
		unsigned int context_id,
		unsigned int num_contexts )
{
	(void) context_id;
	(void) num_contexts;

	feedback_spu.id = id;
	feedback_spu.has_child = 0;
	if (child)
	{
		crSPUInitDispatchTable( &(feedback_spu.child) );
		crSPUCopyDispatchTable( &(feedback_spu.child), &(child->dispatch_table) );
		feedback_spu.has_child = 1;
	}
	crSPUInitDispatchTable( &(feedback_spu.super) );
	crSPUCopyDispatchTable( &(feedback_spu.super), &(self->superSPU->dispatch_table) );
	feedbackspuGatherConfiguration();

	crStateInit();

	return &feedback_functions;
}

void feedbackSPUSelfDispatch(SPUDispatchTable *self)
{
	crSPUInitDispatchTable( &(feedback_spu.self) );
	crSPUCopyDispatchTable( &(feedback_spu.self), self );
}

int feedbackSPUCleanup(void)
{
	return 1;
}

extern SPUOptions feedbackSPUOptions[];

int SPULoad( char **name, char **super, SPUInitFuncPtr *init,
	     SPUSelfDispatchFuncPtr *self, SPUCleanupFuncPtr *cleanup,
	     SPUOptionsPtr *options, int *flags )
{
	*name = "feedback";
	*super = "passthrough";
	*init = feedbackSPUInit;
	*self = feedbackSPUSelfDispatch;
	*cleanup = feedbackSPUCleanup;
	*options = feedbackSPUOptions;
	*flags = (SPU_NO_PACKER|SPU_NOT_TERMINAL|SPU_MAX_SERVERS_ZERO);
	
	return 1;
}
