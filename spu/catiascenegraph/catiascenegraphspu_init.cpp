/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "cr_spu.h"
#include "cr_error.h"
#include "cr_string.h"
#include "catiascenegraphspu.h"
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

CatiaSpu catia_spu;


#ifndef WINDOWS
static void printspu_signal_handler(int signum)
{
	/* If we receive a signal here, we should issue a marker. */
	if (catia_spu.fp != NULL && catia_spu.marker_text != NULL) {
		char *text = catia_spu.marker_text;
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
				fprintf(catia_spu.fp, "%.*s", (int) (nextVariable - text), text);
				text = nextVariable;
			}

			/* Make sure the '$' introduces a variable marker */
			if (nextVariable[1] != '(') {
				/* Not a variable marker at all */
				fprintf(catia_spu.fp, "%c", '$');
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
						fprintf(catia_spu.fp, "%d", signum);
					}
					else if (crStrncmp(variableName, "date", variableLength) == 0) {
						fprintf(catia_spu.fp, "%d-%02d-%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);

					}
					else if (crStrncmp(variableName, "time", variableLength) == 0) {
						fprintf(catia_spu.fp, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
					}
					else {
					    /* Unknown variable.  Print it verbatim. */
					    fprintf(catia_spu.fp, "%.*s", (int) (endOfVariable - nextVariable + 1), nextVariable);
					}
					
					/* In all cases, advance our internal text pointer to after the endOfVariable indicator */
					text = endOfVariable + 1;

					/* And look for the next variable */
					nextVariable = crStrchr(text, '$');
				}
			} /* end of processing a valid variable */
		} /* end of processing a variable */

		/* Print out any remaining text, even if it's an empty string, and the terminal newline */
		fprintf(catia_spu.fp, "%s\n", text);

		fflush(catia_spu.fp);
	}

	/* Pass the signal through to the old signal handler, if any. */
	if (catia_spu.old_signal_handler != NULL) {
		(*catia_spu.old_signal_handler)(signum);
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

	catia_spu.id = id;
    catia_spu.superSpuState = getScenegraphSpuData();
    /*1.this is the code to set current_app_instance and this current_app_instance will point to object corresponding to current running server app.so here we know that catia app will be running so we get current_app_instance pointed
        to ServerAppContentApi's CatiaApi object that is designed to handle catia app content.
      
      2.Basically this current_app_instance setting code will be removed from here in future because all serverapp content specific code is being shifted to scenegraphspu so catiascenegraphspu will not exist anymore as per current design of spu and server content api.

      3.If this catiascenegraph spu will not exist then why we are using this code here because this place of code is the only way to know which server app got connected with transviz. as soon as we will find another way then this below line will be removed 
      from here.

      4. After testing serverappcontentapi design with all cad softwares,detection of current connected app will be done by asking mothership that will write name of app in connection object so in future this below line of code will be shifted in scenegraphspu's 
         init function.
    */
    catia_spu.superSpuState->current_app_instance = ServerAppContentApi::AppContentApi::getAppContentInstance(ServerAppContentApi::AppNameEnum::TRANSVIZ_CATIA);
    printspuGatherConfiguration( child );

	// Interface class to call special fuctions
    CatiaSpufunc* func = new CatiaSpufunc();
	self->privatePtr = (void*)func;

	crSPUInitDispatchTable( &(catia_spu.super) );
	crSPUCopyDispatchTable( &(catia_spu.super), &(self->superSPU->dispatch_table) );

#ifndef WINDOWS
	/* If we were given a marker signal, install our signal handler. */
	if (catia_spu.marker_signal) {
		catia_spu.old_signal_handler = signal(catia_spu.marker_signal, printspu_signal_handler);
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
	if (catia_spu.fp != stderr || catia_spu.fp != stdout)
		fclose(catia_spu.fp);
	return 1;
}

extern SPUOptions printSPUOptions[];

int SPULoad( char **name, char **super, SPUInitFuncPtr *init,
	     SPUSelfDispatchFuncPtr *self, SPUCleanupFuncPtr *cleanup,
	     SPUOptionsPtr *options, int *flags )
{
	*name = "catiascenegraph";
	*super = "scenegraph";
	*init = printSPUInit;
	*self = printSPUSelfDispatch;
	*cleanup = printSPUCleanup;
	*options = printSPUOptions;
	*flags = (SPU_NO_PACKER|SPU_IS_TERMINAL|SPU_MAX_SERVERS_ZERO);
	scenegraphCatiaSPUReset();
	return 1;
}
