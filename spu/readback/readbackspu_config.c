/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "readbackspu.h"
#include "cr_mothership.h"
#include "cr_string.h"

static void __setDefaults( ReadbackSPU *readback_spu )
{
	/* config options */
	readback_spu->extract_depth = 0;
	readback_spu->extract_alpha = 0;
	readback_spu->local_visualization = 0;
	readback_spu->visualize_depth = 0;
	readback_spu->resizable = 0;
	/* misc */
	readback_spu->barrierSize = 0;
}

static void set_extract_depth( ReadbackSPU *readback_spu, const char *response )
{
	readback_spu->extract_depth = crStrToInt( response );
}

static void set_extract_alpha( ReadbackSPU *readback_spu, const char *response )
{
	readback_spu->extract_alpha = crStrToInt( response );
}

static void set_local_visualization( ReadbackSPU *readback_spu, const char *response )
{
	readback_spu->local_visualization = crStrToInt( response );
}

static void set_visualize_depth( ReadbackSPU *readback_spu, const char *response )
{
	readback_spu->visualize_depth = crStrToInt( response );
}

static void set_gather_url( ReadbackSPU *readback_spu, const char *response )
{
	if (crStrlen(response) > 0)
		readback_spu->gather_url = crStrdup( response );
	else
		readback_spu->gather_url = NULL;
}

static void set_gather_mtu( ReadbackSPU *readback_spu, const char *response )
{
	sscanf( response, "%d", &readback_spu->gather_mtu );
}


/* option, type, nr, default, min, max, title, callback
 */
SPUOptions readbackSPUOptions[] = {

	/* Really a multiple choice:  Extract depth or alpha.
	 */
	{ "extract_depth", CR_BOOL, 1, "0", NULL, NULL,
	  "Extract Depth Values", (SPUOptionCB)set_extract_depth },

	{ "extract_alpha", CR_BOOL, 1, "0", NULL, NULL,
	  "Extract Alpha Values", (SPUOptionCB)set_extract_alpha },

	{ "local_visualization", CR_BOOL, 1, "1", NULL, NULL,
	  "Local Visualization", (SPUOptionCB)set_local_visualization },

	{ "visualize_depth", CR_BOOL, 1, "0", NULL, NULL,
	  "Visualize Depth as Grayscale", (SPUOptionCB)set_visualize_depth },

	{ "gather_url", CR_STRING, 1, "", NULL, NULL,
	  "URL to Connect to for Gathering", (SPUOptionCB)set_gather_url },

	{ "gather_mtu", CR_INT, 1, "1048576", "1024", NULL,
	  "MTU for Gathering", (SPUOptionCB)set_gather_mtu },

	{ NULL, CR_BOOL, 0, NULL, NULL, NULL, NULL, NULL },
};


void readbackspuGatherConfiguration( ReadbackSPU *readback_spu )
{
	CRConnection *conn;

	__setDefaults( readback_spu );

	/* Connect to the mothership and identify ourselves. */
	
	conn = crMothershipConnect( );
	if (!conn)
	{
		/* The mothership isn't running.  Some SPU's can recover gracefully, some 
		 * should issue an error here. */
	        crSPUSetDefaultParams( readback_spu, readbackSPUOptions );
		return;
	}
	crMothershipIdentifySPU( conn, readback_spu->id );

	crSPUGetMothershipParams( conn, (void *)readback_spu, readbackSPUOptions );

	/* we can either composite with alpha or Z, but not both */
	if (readback_spu->extract_depth && readback_spu->extract_alpha) {
		crWarning("Readback SPU can't extract both depth and alpha, using depth");
		readback_spu->extract_alpha = 0;
	}

	/* Get the render SPU's resizable setting */
	{
		char response[1000];
		if (crMothershipGetSPUParam( conn, response, "resizable" )) {
			int resizable = 0;
			sscanf(response, "%d", &resizable);
			readback_spu->resizable = resizable;
		}
	}

	crMothershipDisconnect( conn );
}
