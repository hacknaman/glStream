#include "cr_string.h"
#include "cr_error.h"
#include "cr_net.h"
#include "cr_mothership.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define MOTHERPORT 10000

CRConnection *crMothershipConnect( void )
{
	char *mother_server = NULL;
	int   mother_port = MOTHERPORT;
	CRConnection *conn;
	char mother_url[1024];

	mother_server = getenv( "CRMOTHERSHIP" );
	if (!mother_server)
	{
		crWarning( "Couldn't find the CRMOTHERSHIP environment variable, defaulting to localhost" );
		mother_server = "localhost";
	}

	sprintf( mother_url, "%s:%d", mother_server, mother_port );

	crNetInit( NULL, NULL );
	conn = crConnectToServer( mother_server, 10000, 8096 );
	return conn;
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
	crNetSendExact( conn, txt, strlen(txt) );
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

int crMothershipSPUParam( CRConnection *conn, char *response, char *param, ... )
{
	va_list args;
	char txt[8096];
	va_start( args, param );
	vsprintf( txt, param, args );
	va_end( args );

	return crMothershipSendString( conn, response, "spuparam %s", txt );
}

void crMothershipReset( CRConnection *conn )
{
	if (!crMothershipSendString( conn, NULL, "reset" ))
	{
		crError( "Couldn't reset the server!" );
	}

}
