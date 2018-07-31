/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "cameraspu.h"

#include "cr_mothership.h"
#include "cr_string.h"


/**
 * Set default options for SPU
 */
static void setDefaults( void )
{
}
/** 
 * SPU options
 * option, type, nr, default, min, max, title, callback
 */
SPUOptions cameraSPUOptions[] = {
   { NULL, CR_BOOL, 0, NULL, NULL, NULL, NULL, NULL },
};


/**
 * Gather the config info for the SPU
 */
void cameraspuGatherConfiguration( void )
{
	CRConnection *conn;

	setDefaults();

	/* Connect to the mothership and identify ourselves. */
	
	conn = crMothershipConnect( );
	if (!conn)
	{
		/* The mothership isn't running.  Some SPU's can recover gracefully, some 
		 * should issue an error here. */
        crSPUSetDefaultParams(&camera_spu, cameraSPUOptions);
		return;
	}
    crMothershipIdentifySPU(conn, camera_spu.id);

    crSPUGetMothershipParams(conn, &camera_spu, cameraSPUOptions);

	crMothershipDisconnect( conn );
}
