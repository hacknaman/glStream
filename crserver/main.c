#include "server.h"
#include "cr_net.h"
#include "cr_unpack.h"
#include "cr_protocol.h"
#include "cr_error.h"
#include "cr_glstate.h"

CRServer cr_server;

void crServerClose( unsigned int id )
{
	crError( "Client disconnected!" );
	(void) id;
}

int main( int argc, char *argv[] )
{
	int i;
	crNetInit(crServerRecv, crServerClose);
	crStateInit();
	crServerGatherConfiguration();
	for ( i = 0 ; i < cr_server.num_clients ; i++)
	{
		crServerRecomputeBaseProjection( &(cr_server.clients[i].baseProjection) );
	}
	cr_server.head_spu->dispatch_table.MatrixMode( GL_PROJECTION );
	cr_server.head_spu->dispatch_table.LoadMatrixf( (GLfloat *) &(cr_server.clients[0].baseProjection) );
	crServerInitDispatch();
	crStateMakeCurrent( cr_server.clients[0].ctx );
	crStateSetCurrentPointers( &(cr_server.current) );
	crUnpackSetReturnPointer( &(cr_server.return_ptr) );
	crUnpackSetWritebackPointer( &(cr_server.writeback_ptr) );
	crServerSerializeRemoteStreams();
	return 0;
}
