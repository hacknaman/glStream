#include "tilesortspu.h"

#include "cr_mothership.h"
#include "cr_string.h"
#include "cr_error.h"
#include "cr_mem.h"

#include <stdio.h>

static void __setDefaults( void )
{
	tilesort_spu.num_servers = 0;
	tilesort_spu.servers = NULL;
	tilesort_spu.apply_viewtransform = 0;
	tilesort_spu.splitBeginEnd = 1;
}

void tilesortspuGatherConfiguration( void )
{
	CRConnection *conn;
	char response[8096];

	char **serverchain, **serverlist;
	int num_servers;
	int i, tile;

	__setDefaults();

	// Connect to the mothership and identify ourselves.
	
	conn = crMothershipConnect( );
	crMothershipIdentifySPU( conn, tilesort_spu.id );

	crMothershipGetMTU( conn, response );
	sscanf( response, "%d", &tilesort_spu.MTU );

	// The response to this tells us how many servers, what the
	// SPU id of their first SPU is, and where they are.
	//
	// For example:  2 1 tcpip://foo 2 tcpip://bar
	//
	// We need the SPU ID since the tiling information is actually
	// associated with the render SPU that must be at the head of the
	// server chain for tile/sorting to work properly.

	crMothershipGetServers( conn, response );

	serverchain = crStrSplitn( response, " ", 1 );
	num_servers = crStrToInt( serverchain[0] );

	if (num_servers == 0)
	{
		crError( "No servers specified for a tile/sort SPU?!" );
	}
	serverlist = crStrSplit( serverchain[1], "," );

	tilesort_spu.num_servers = num_servers;
	tilesort_spu.servers = (TileSortSPUServer *) crAlloc( num_servers * sizeof( *(tilesort_spu.servers) ) );

	crDebug( "Got %d servers!", num_servers );

	for (i = 0 ; i < num_servers ; i++)
	{
		char server_url[1024];
		char response[1024];
		int server_spuid;
		TileSortSPUServer *server = tilesort_spu.servers + i;
	
		sscanf( serverlist[i], "%d %s", &server_spuid, server_url );

		crDebug( "Server %d: (%d) %s", i+1, server_spuid, server_url );
		server->net.name = crStrdup( server_url );
		server->net.buffer_size = tilesort_spu.MTU;

		// Now, we want to query the mothership about the tile information,
		// which is stored with the server's head render SPU.  This is
		// a little gross, but no other assignment of tiling information made
		// more sense.

		crMothershipIdentifySPU( conn, server_spuid );

		if (crMothershipSPUParam( conn, response, "split_begin_end") )
		{
			sscanf( response, "%d", &(tilesort_spu.splitBeginEnd) );
		}

		if (!crMothershipSPUParam( conn, response, "num_tiles" ))
		{
			server->num_extents = 1;
		}
		else
		{
			sscanf( response, "%d", &(server->num_extents) );
		}
		if (server->num_extents > 1)
		{
			tilesort_spu.apply_viewtransform = 1;
		}
		for (tile = 0; tile < server->num_extents ; tile++)
		{
			if (!crMothershipSPUParam( conn, response, "tile%d", tile+1 ))
			{
				crWarning( "No tile extent information for tile %d, defaulting", tile );
				crWarning( "To 0,0,640,480" );
				crStrcpy( response, "0 0 640 480" );
			}
			sscanf( response, "%d %d %d %d", &(server->mural_x[tile]), 
					&(server->mural_y[tile]), &(server->mural_w[tile]), 
					&(server->mural_h[tile]) );

			crDebug( "Server %d, tile %d/%d: (%d, %d, %d, %d)",
					i+1, tile+1, server->num_extents,
					server->mural_x[tile], server->mural_y[tile],
					server->mural_w[tile], server->mural_h[tile] );
		}
	}

	crMothershipDisconnect( conn );
}
