/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "cr_string.h"
#include "cr_error.h"
#include "cr_net.h"
#include "cr_mothership.h"
#include "cr_environment.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define MOTHERPORT 10000

CRConnection *crMothershipConnect( void )
{
	char *mother_server = NULL;

	crNetInit( NULL, NULL );

	mother_server = crGetenv( "CRMOTHERSHIP" );
	if (!mother_server)
	{
		crWarning( "Couldn't find the CRMOTHERSHIP environment variable, defaulting to localhost" );
		mother_server = "localhost";
	}

	return crNetConnectToServer( mother_server, MOTHERPORT, 8096, 0 );
}

void crMothershipDisconnect( CRConnection *conn )
{
	crMothershipSendString( conn, NULL, "quit" );
	crNetDisconnect( conn );
}

int crMothershipSendString( CRConnection *conn, char *response_buf, char *str, ... )
{
	va_list args;
	static char txt[8092];

	va_start(args, str);
	vsprintf( txt, str, args );
	va_end(args);

	crStrcat( txt, "\n" );
	crNetSendExact( conn, txt, crStrlen(txt) );
	if (response_buf)
	{
		return crMothershipReadResponse( conn, response_buf );
	}
	else
	{
		char devnull[1024];
		return crMothershipReadResponse( conn, devnull );
	}
}

int crMothershipReadResponse( CRConnection *conn, void *buf )
{
	char codestr[4];
	int code;

	crNetSingleRecv( conn, codestr, 4 );
	crNetReadline( conn, buf );

	code = crStrToInt( codestr );
	return (code == 200);
}

void crMothershipIdentifySPU( CRConnection *conn, int spu )
{
	if (!crMothershipSendString( conn, NULL, "spu %d", spu ))
	{
		crError( "Server said it hadn't heard of SPU %d", spu );
	}
}

int crMothershipGetSPUParam( CRConnection *conn, char *response, const char *param, ... )
{
	va_list args;
	char txt[8096];
	va_start( args, param );
	vsprintf( txt, param, args );
	va_end( args );

	return crMothershipSendString( conn, response, "spuparam %s", txt );
}

/*
 * Set an SPU parameter.
 * Added by BrianP
 */
int crMothershipSetSPUParam( CRConnection *conn, const char *param, const char *value)
{
	/* Ex: param = "GL_MAX_TEXTURE_SIZE", value = "1024" */
	return crMothershipSendString( conn, NULL, "setspuparam %s %s", param, value );
}

/*
 * Get a parameter from a named SPU.
 * Input: spu_id = the integer SPU number
 *        param = the parameter to query
 * Output: response
 */
int crMothershipGetNamedSPUParam( CRConnection *conn, int spu_id, const char *param, char *response )
{
	return crMothershipSendString( conn, response, "namedspuparam %d %s", spu_id, param );
}

int crMothershipGetServerParam( CRConnection *conn, char *response, const char *param, ... )
{
	va_list args;
	char txt[8096];
	va_start( args, param );
	vsprintf( txt, param, args );
	va_end( args );

	return crMothershipSendString( conn, response, "serverparam %s", txt );
}

int crMothershipGetFakerParam( CRConnection *conn, char *response, const char *param, ... )
{
	va_list args;
	char txt[8096];
	va_start( args, param );
	vsprintf( txt, param, args );
	va_end( args );

	return crMothershipSendString( conn, response, "fakerparam %s", txt );
}


void crMothershipReset( CRConnection *conn )
{
	if (!crMothershipSendString( conn, NULL, "reset" ))
	{
		crError( "Couldn't reset the server!" );
	}
}

#define INSIST(x) if (!x) crError( "Bad Mothership response: %s", response )

void crMothershipIdentifyFaker( CRConnection *conn, char *response )
{
	char hostname[1024];
	if ( crGetHostname( hostname, sizeof(hostname) ) )
	{
		crError( "Couldn't get my own hostname?" );
	}
	INSIST( crMothershipSendString( conn, response, "faker %s", hostname ));
}

void crMothershipIdentifyOpenGL( CRConnection *conn, char *response, char *app_id )
{
	char hostname[1024];
	if ( crGetHostname( hostname, sizeof(hostname) ) )
	{
		crError( "Couldn't get my own hostname?" );
	}
	INSIST( crMothershipSendString( conn, response, "opengldll %s %s", app_id, hostname ));
}

void crMothershipIdentifyServer( CRConnection *conn, char *response )
{
	char hostname[1024];
	if ( crGetHostname( hostname, sizeof(hostname) ) )
	{
		crError( "Couldn't get my own hostname?" );
	}
	INSIST( crMothershipSendString( conn, response, "server %s", hostname ));
}


void crMothershipSetParam( CRConnection *conn, const char *param, const char *value )
{
	(void) crMothershipSendString( conn, NULL, "setparam %s %s", param, value );
}

int crMothershipGetParam( CRConnection *conn, const char *param, char *response )
{
	return crMothershipSendString( conn, response, "getparam %s", param );
}

int crMothershipGetMTU( CRConnection *conn )
{
	char response[4096];
	int mtu;
	INSIST( crMothershipGetParam( conn, "MTU", response ) );
	sscanf( response, "%d", &mtu );
	return mtu;
}

int crMothershipGetRank( CRConnection *conn, char *response )
{
	return crMothershipSendString( conn, response, "rank" );
}

void crMothershipGetClients( CRConnection *conn, char *response )
{
	INSIST( crMothershipSendString( conn, response, "clients" ));
}

void crMothershipGetServers( CRConnection *conn, char *response )
{
	INSIST( crMothershipSendString( conn, response, "servers" ));
}

int crMothershipGetTiles( CRConnection *conn, char *response, int server_num )
{
	return crMothershipSendString( conn, response, "tiles %d", server_num );
}

int crMothershipGetDisplays( CRConnection *conn, char *response )
{
	return crMothershipSendString( conn, response, "displays");
}

int crMothershipGetDisplayTiles( CRConnection *conn, char *response, int server_num )
{
	return crMothershipSendString( conn, response, "display_tiles %d", server_num );
}

int crMothershipGetServerTiles( CRConnection *conn, char *response )
{
	return crMothershipSendString( conn, response, "servertiles" );
}

int crMothershipGetServerDisplayTiles( CRConnection *conn, char *response )
{
	return crMothershipSendString( conn, response, "serverdisplaytiles" );
}

int crMothershipRequestTileLayout( CRConnection *conn, char *response,
                             int muralWidth, int muralHeight )
{
	return crMothershipSendString( conn, response, "gettilelayout %d %d",
																 muralWidth, muralHeight );
}
