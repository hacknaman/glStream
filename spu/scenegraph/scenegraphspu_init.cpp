/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "cr_spu.h"
#include "cr_error.h"
#include "cr_string.h"
#include "scenegraphspu.h"
#include <stdio.h>
#include <signal.h>
#ifndef WINDOWS
#include <sys/time.h>
#endif

extern SPUNamedFunctionTable _cr_print_table[];

static SPUFunctions print_functions = {
	NULL, /* CHILD COPY */
	NULL, /* DATA */
	_cr_print_table /* THE ACTUAL FUNCTIONS */
};

ScenegraphSpuData scenegraph_spu_data;


void Scenespufunc::getUpdatedScene()
{
		getUpdatedSceneSC();
}

void Scenespufunc::changeScene()
{
		
}

void Scenespufunc::funcNodeUpdate(void(*pt2Func)(void * context, osg::ref_ptr<osg::Group>), void *context)
{
		funcNodeUpdateSC(pt2Func, context);
}

 void Scenespufunc::getContentTree(ServerAppContentApi::ServerContentNode* root)
{
    
	scenegraph_spu_data.current_app_instance->createContentTree(root);

}
void Scenespufunc::resetClient()
{
    scenegraph_spu_data.current_app_instance->resetOriginalColors();
}
#ifndef WINDOWS
static void printspu_signal_handler(int signum)
{
	/* If we receive a signal here, we should issue a marker. */
	if (print_spu.fp != NULL && print_spu.marker_text != NULL) {
		char *text = print_spu.marker_text;
		char *nextVariable = crStrchr(text, '$');

		/* Some of the variables will need this information */
		struct timeval tv;
		struct timezone tz;
		struct tm *tm;

		gettimeofday(&tv, &tz);
		tm = localtime(&tv.tv_sec);

		/* While we still have variables, keep looking for more */
		while (nextVariable != NULL) {
			/* Print the constant text up to the variable. */
			if (nextVariable > text) {
				fprintf(print_spu.fp, "%.*s", (int) (nextVariable - text), text);
				text = nextVariable;
			}

			/* Make sure the '$' introduces a variable marker */
			if (nextVariable[1] != '(') {
				/* Not a variable marker at all */
				fprintf(print_spu.fp, "%c", '$');
				text = nextVariable + 1;
				nextVariable = crStrchr(text, '$');
			}
			else {
				/* Look for the last character */
				char *endOfVariable = crStrchr(nextVariable, ')');
				if (endOfVariable == NULL) {
					/* No end to the marker - spit it out as text */
					text = nextVariable;
					nextVariable = NULL;
				}
				else {
					/* Compare it to the known variables */
					/* Skip the initial '$(' and the final ')' */
					char *variableName = nextVariable + 2;
					int variableLength = endOfVariable - nextVariable - 3 + 1;

					if (crStrncmp(variableName, "signal", variableLength) == 0) {
						fprintf(print_spu.fp, "%d", signum);
					}
					else if (crStrncmp(variableName, "date", variableLength) == 0) {
						fprintf(print_spu.fp, "%d-%02d-%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);

					}
					else if (crStrncmp(variableName, "time", variableLength) == 0) {
						fprintf(print_spu.fp, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
					}
					else {
					    /* Unknown variable.  Print it verbatim. */
					    fprintf(print_spu.fp, "%.*s", (int) (endOfVariable - nextVariable + 1), nextVariable);
					}
					
					/* In all cases, advance our internal text pointer to after the endOfVariable indicator */
					text = endOfVariable + 1;

					/* And look for the next variable */
					nextVariable = crStrchr(text, '$');
				}
			} /* end of processing a valid variable */
		} /* end of processing a variable */

		/* Print out any remaining text, even if it's an empty string, and the terminal newline */
		fprintf(print_spu.fp, "%s\n", text);

		fflush(print_spu.fp);
	}

	/* Pass the signal through to the old signal handler, if any. */
	if (print_spu.old_signal_handler != NULL) {
		(*print_spu.old_signal_handler)(signum);
	}
}
#endif

static SPUFunctions *
printSPUInit( int id, SPU *child, SPU *self,
							unsigned int context_id,
							unsigned int num_contexts,
							int* ImpThreadID)
{
	(void) context_id;
	(void) num_contexts;
	(void) child;

    scenegraph_spu_data.id = id;
	printspuGatherConfiguration( child );

	// Interface class to call special fuctions
	Scenespufunc* func = new Scenespufunc();
	self->privatePtr = (void*)func;

    crSPUInitDispatchTable(&(scenegraph_spu_data.super));
    crSPUCopyDispatchTable(&(scenegraph_spu_data.super), &(self->superSPU->dispatch_table));

#ifndef WINDOWS
	/* If we were given a marker signal, install our signal handler. */
	if (print_spu.marker_signal) {
		print_spu.old_signal_handler = signal(print_spu.marker_signal, printspu_signal_handler);
	}
#endif
	return &print_functions;
}

static void
printSPUSelfDispatch(SPUDispatchTable *parent)
{
	(void)parent;
}

static int
printSPUCleanup(void)
{
    if (scenegraph_spu_data.fp != stderr || scenegraph_spu_data.fp != stdout)
        fclose(scenegraph_spu_data.fp);
	return 1;
}

extern SPUOptions printSPUOptions[];

int SPULoad( char **name, char **super, SPUInitFuncPtr *init,
	     SPUSelfDispatchFuncPtr *self, SPUCleanupFuncPtr *cleanup,
	     SPUOptionsPtr *options, int *flags )
{
	*name = "scenegraph";
	*super = NULL;
	*init = printSPUInit;
	*self = printSPUSelfDispatch;
	*cleanup = printSPUCleanup;
	*options = printSPUOptions;
	*flags = (SPU_NO_PACKER|SPU_IS_TERMINAL|SPU_MAX_SERVERS_ZERO);
	scenegraphSPUReset();
	return 1;
}
