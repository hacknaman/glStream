#include "cr_packfunctions.h"
#include "cr_pack.h"
#include "cr_opcodes.h"
#include <GL/gl.h>

static void __handleFogData( GLenum pname, const GLfloat *params )
{
	int packet_length = sizeof( int ) + sizeof( pname );
	unsigned char *data_ptr;
	switch( pname )
	{
		case GL_FOG_MODE:
		case GL_FOG_DENSITY:
		case GL_FOG_START:
		case GL_FOG_END:
		case GL_FOG_INDEX:
			packet_length += sizeof( *params );
			break;
		case GL_FOG_COLOR:
			packet_length += 4*sizeof( *params );
	}
	GET_BUFFERED_POINTER( packet_length );
	WRITE_DATA( 0, int, packet_length );
	WRITE_DATA( 4, GLenum, pname );
	WRITE_DATA( 8, GLfloat, params[0] );
	if (packet_length > 8) 
	{
		WRITE_DATA( 12, GLfloat, params[1] );
		WRITE_DATA( 16, GLfloat, params[2] );
		WRITE_DATA( 20, GLfloat, params[3] );
	}
}

void PACK_APIENTRY crPackFogfv(GLenum pname, const GLfloat *params)
{
	__handleFogData( pname, params );
	WRITE_OPCODE( CR_FOGFV_OPCODE );
}

void PACK_APIENTRY crPackFogiv(GLenum pname, const GLint *params)
{
	/* floats and ints are the same size, so the packing should be the same */
	__handleFogData( pname, (const GLfloat *) params );
	WRITE_OPCODE( CR_FOGIV_OPCODE );
}
