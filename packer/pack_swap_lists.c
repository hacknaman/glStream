/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "packer.h"
#include "cr_glwrapper.h"
#include "cr_error.h"
#include <memory.h>

static int __gl_CallListsNumBytes( GLenum type )
{
	switch( type )
	{
		case GL_BYTE:
		case GL_UNSIGNED_BYTE:
		case GL_2_BYTES:
			return 1;
		case GL_SHORT:
		case GL_UNSIGNED_SHORT:
		case GL_3_BYTES:
			return 2;
		case GL_INT:
		case GL_UNSIGNED_INT:
		case GL_FLOAT:
		case GL_4_BYTES:
			return 4;
		default:
			crError( "Invalid type: %d", type );
	}
	return -1;
}

void PACK_APIENTRY crPackCallListsSWAP(GLint n, GLenum type, 
		const GLvoid *lists )
{
	unsigned char *data_ptr;
	int packet_length;
	GLshort *shortPtr;
	GLint   *intPtr;
	int i;

	int bytesPerList = __gl_CallListsNumBytes( type );
	int numBytes = bytesPerList * n;

	packet_length = sizeof( n ) + 
		sizeof( type ) + 
		numBytes;

	data_ptr = (unsigned char *) crPackAlloc( packet_length );
	WRITE_DATA( 0, GLint, SWAP32(n) );
	WRITE_DATA( 4, GLenum, SWAP32(type) );

	memcpy( data_ptr + 8, lists, numBytes );
	shortPtr = (GLshort *) (data_ptr + 8);
	intPtr   = (GLint *) (data_ptr + 8);

	if (bytesPerList > 1)
	{
		for ( i = 0 ; i < n ; i++)
		{
			switch( bytesPerList )
			{
				case 2:
					*(shortPtr++) = SWAP16(*shortPtr);
					break;
				case 4:
					*(intPtr++) = SWAP32(*intPtr);
					break;
			}
		}
	}

	crHugePacket( CR_CALLLISTS_OPCODE, data_ptr );
	crPackFree( data_ptr );
}
