/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "avevascenegraphspu.h"
#include "cr_mothership.h"
#include "cr_string.h"
#include "cr_error.h"
#include "cr_mem.h"
#include "cr_spu.h"

static void __setDefaults( void )
{
}

static void set_marker_text( void *foo, const char *response )
{
}
static void set_marker_signal( void *foo, const char *response )
{
}

static void set_log_file( void *foo, const char *response )
{
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
	crMothershipIdentifySPU( conn, aveva_spu.id );

	crSPUGetMothershipParams(conn, &aveva_spu, printSPUOptions);

	crMothershipDisconnect( conn );
}