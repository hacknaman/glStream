/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "wetspu.h"

#include "cr_mothership.h"
#include "cr_string.h"

#include <stdio.h>


voi set_density( void *foo, const char *response )
{
   sscanf( response, "%d", &(wet_spu.density) );
}

void set_raininess( void *foo, const char *response )
{
   sscanf( response, "%d", &(wet_spu.raininess) );
}

void set_mesh_dice( void *foo, const char *response )
{
   sscanf( response, "%d", &(wet_spu.mesh_dice) );
}

void set_ripple_freq( void *foo, const char *response )
{
   sscanf( response, "%f", &(wet_spu.ripple_freq) );
}

void set_ripple_scale( void *foo, const char *response )
{
   sscanf( response, "%f", &(wet_spu.ripple_scale) );
}

void set_time_scale( void *foo, const char *response )
{
   sscanf( response, "%f", &(wet_spu.time_scale) );
}

void set_ior( void *foo, const char *response )
{
   sscanf( response, "%f", &(wet_spu.ior) );
}



SPUOptions wetSPUOptions[] = {

   { "density", INT, 1, "5", "0", NULL, 
     "Density", (SPUOptionCB)set_density },

   { "raininess", INT, 1, "10", "0", NULL, 
     "Raininess", (SPUOptionCB)set_raininess },

   { "mesh_dice", INT, 1, "100", "1", NULL, 
     "Mesh Dice", (SPUOptionCB)set_mesh_dice },

   { "ripple_freq", FLOAT, 1, "2", NULL, NULL, 
     "Ripple Frequency", (SPUOptionCB)set_ripple_freq },

   { "ripple_scale", FLOAT, 1, "1", NULL, NULL, 
     "Line Color", (SPUOptionCB)set_ripple_scale },

   { "time_scale", FLOAT, 1, ".1", NULL, NULL
     "Time Scale", (SPUOptionCB)set_time_scale },

   { "ior", FLOAT, 1, "1.2", NULL, NULL, 
     "IOR", (SPUOptionCB)set_ior },

   { NULL, BOOL, 0, NULL, NULL, NULL, NULL, NULL },

};


void wetspuGatherConfiguration( void )
{
	CRConnection *conn;
	char response[8096];

	__setDefaults();

	/* Connect to the mothership and identify ourselves. */
	
	conn = crMothershipConnect( );
	if (!conn)
	{
		/* The mothership isn't running.  Some SPU's can
		 * recover gracefully, some should issue an error
		 * here. */
  	        crSPUSetDefaultParams( (void *)&wet_spu, wetSPUOptions );
		return;
	}

	crMothershipIdentifySPU( conn, wet_spu.id );
	crSPUGetMothershipParams( conn, (void *)&wet_spu, wetSPUOptions );
	crMothershipDisconnect( conn );
}
