/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "catiascenegraphspu.h"
#include "cr_mothership.h"
#include "cr_string.h"
#include "cr_error.h"
#include "cr_mem.h"
#include "cr_spu.h"

static void __setDefaults( void )
{
	catia_spu.fp = stderr;
}

static void set_marker_text( void *foo, const char *response )
{
	catia_spu.marker_text = crStrdup(response);
}
static void set_marker_signal( void *foo, const char *response )
{
	catia_spu.marker_signal = crStrToInt(response);
}

static void set_log_file( void *foo, const char *response )
{
	catia_spu.fp = stdout;
}


/* option, type, nr, default, min, max, title, callback
 */
SPUOptions printSPUOptions[] = {

	{ "log_file", CR_STRING, 1, "stderr", NULL, NULL, 
		"Log file name (or stdout,stderr)", (SPUOptionCB)set_log_file },

	{ "marker_signal", CR_INT, 1, "0", NULL, NULL, 
		"Signal number to issue marker text", (SPUOptionCB)set_marker_signal },
	{ "marker_text", CR_STRING, 1, 
		"Received marker signal $(signal) at $(date) $(time)",
		NULL, NULL, 
		"Marker text to issue on receipt of signal", (SPUOptionCB)set_marker_text },

	{ NULL, CR_BOOL, 0, NULL, NULL, NULL, NULL, NULL },
};


void printspuGatherConfiguration( const SPU *child_spu )
{
	CRConnection *conn;

	__setDefaults();

	/* Connect to the mothership and identify ourselves. */
	
	conn = crMothershipConnect( );
	if (!conn)
	{
		/* defaults are fine. */
		return;
	}
	crMothershipIdentifySPU( conn, catia_spu.id );

	crSPUGetMothershipParams( conn, &catia_spu, printSPUOptions );

	crMothershipDisconnect( conn );
}
