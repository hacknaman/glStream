#include "cr_spu.h"
#include "renderspu.h"

#include <stdio.h>

extern SPUNamedFunctionTable render_table[];
extern void LoadSystemGL( SPUNamedFunctionTable * );

SPUFunctions the_functions = {
	NULL, // CHILD COPY
	NULL, // DATA
	render_table // THE ACTUAL FUNCTIONS
};

RenderSPU render_spu;

SPUFunctions *SPUInit( int id, SPU *child, SPU *super,
		unsigned int context_id, unsigned int num_contexts )
{
	(void) child;
	(void) super;
	(void) context_id;
	(void) num_contexts;

	LoadSystemGL( render_table );
	render_spu.id = id;
	render_spu.dispatch = NULL;
	renderspuGatherConfiguration();
	renderspuCreateWindow();
	return &the_functions;
}

void SPUSelfDispatch(SPUDispatchTable *self)
{
	render_spu.dispatch = self;
}

int SPUCleanup(void)
{
	return 1;
}

int SPULoad( char **name, char **super, SPUInitFuncPtr *init,
	SPUSelfDispatchFuncPtr *self, SPUCleanupFuncPtr *cleanup )
{
	*name = "render";
	*super = NULL;
	*init = SPUInit;
	*self = SPUSelfDispatch;
	*cleanup = SPUCleanup;
	
	return 1;
}
