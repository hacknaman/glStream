#include "packspu.h"
#include "cr_string.h"
#include "cr_error.h"
#include "cr_mothership.h"

#include <stdio.h>

static void __setDefaults( void )
{
	pack_spu.buffer_size = 1024*1024;
}

void packspuGatherConfiguration( void )
{
	CRConnection *conn;
	char response[8096];

	conn = crMothershipConnect();
	crMothershipIdentifySPU( conn, pack_spu.id );

	__setDefaults();

	if (crMothershipSPUParam( conn, response, "server" ) )
	{
		pack_spu.server_name = crStrdup( response );
	}
	else
	{
		crError( "No server specified for Pack SPU %d", pack_spu.id );
	}

	if (crMothershipSPUParam( conn, response, "buffer_size" ))
	{
		sscanf( response, "%d", &(pack_spu.buffer_size) );
	}
}
