/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "packer.h"
#include "cr_pixeldata.h"
#include "cr_error.h"
#include "cr_mem.h"
#include "cr_version.h"


void PACK_APIENTRY crPackDrawPixels( GLsizei width, GLsizei height, 
																		 GLenum format, GLenum type,
																		 const GLvoid *pixels,
																		 const CRPixelPackState *unpackstate )
{
	unsigned char *data_ptr;
	int packet_length;

	if (pixels == NULL)
	{
		return;
	}

#if 0
	/* WHAT IS THIS FOR?  Disabled by Brian on 3 Dec 2003 */
	if (type == GL_BITMAP)
	{
		crPackBitmap( width, height, 0, 0, 0, 0,
									(const GLubyte *) pixels, unpackstate );
	}
#endif

	packet_length = 
		sizeof( width ) +
		sizeof( height ) +
		sizeof( format ) +
		sizeof( type );

	packet_length += crImageSize( format, type, width, height );

	data_ptr = (unsigned char *) crPackAlloc( packet_length );
	WRITE_DATA( 0, GLsizei, width );
	WRITE_DATA( 4, GLsizei, height );
	WRITE_DATA( 8, GLenum, format );
	WRITE_DATA( 12, GLenum, type );

	crPixelCopy2D( width, height,
								 (void *) (data_ptr + 16), format, type, NULL, /* dst */
								 pixels, format, type, unpackstate );  /* src */

	crHugePacket( CR_DRAWPIXELS_OPCODE, data_ptr );
}

void PACK_APIENTRY crPackReadPixels( GLint x, GLint y, GLsizei width, 
																		 GLsizei height, GLenum format,
																		 GLenum type, GLvoid *pixels,
																		 const CRPixelPackState *packstate,
																		 int *writeback)
{
	GET_PACKER_CONTEXT(pc);
	unsigned char *data_ptr;
	GLint stride = 0;
	GLint bytes_per_row;
	int bytes_per_pixel;
	(void)writeback;

	switch ( type )
	{
#ifdef CR_OPENGL_VERSION_1_2
	  case GL_UNSIGNED_BYTE_3_3_2:
	  case GL_UNSIGNED_BYTE_2_3_3_REV:
#endif
	  case GL_UNSIGNED_BYTE:
	  case GL_BYTE:
			 bytes_per_pixel = 1;
			 break;

#ifdef CR_OPENGL_VERSION_1_2
	  case GL_UNSIGNED_SHORT_5_6_5:
	  case GL_UNSIGNED_SHORT_5_6_5_REV:
	  case GL_UNSIGNED_SHORT_5_5_5_1:
	  case GL_UNSIGNED_SHORT_1_5_5_5_REV:
	  case GL_UNSIGNED_SHORT_4_4_4_4:
	  case GL_UNSIGNED_SHORT_4_4_4_4_REV:
#endif
	  case GL_UNSIGNED_SHORT:
	  case GL_SHORT:
			bytes_per_pixel = 2;
			break;

#ifdef CR_OPENGL_VERSION_1_2
	  case GL_UNSIGNED_INT_8_8_8_8:
	  case GL_UNSIGNED_INT_8_8_8_8_REV:
	  case GL_UNSIGNED_INT_10_10_10_2:
	  case GL_UNSIGNED_INT_2_10_10_10_REV:
#endif
	  case GL_UNSIGNED_INT:
	  case GL_INT:
	  case GL_FLOAT:
			 bytes_per_pixel = 4;
			 break;

	  default:
			__PackError( __LINE__, __FILE__, GL_INVALID_ENUM,
									 "crPackReadPixels(bad type)" );
			return;
	}

	switch ( format )
	{
	  case GL_COLOR_INDEX:
	  case GL_STENCIL_INDEX:
	  case GL_DEPTH_COMPONENT:
	  case GL_RED:
	  case GL_GREEN:
	  case GL_BLUE:
	  case GL_ALPHA:
	  case GL_LUMINANCE:
			 break;

	  case GL_LUMINANCE_ALPHA:
			 bytes_per_pixel *= 2;
			 break;

	  case GL_RGB:
			 bytes_per_pixel *= 3;
			 break;

	  case GL_RGBA:
			 bytes_per_pixel *= 4;
			 break;

	  default:
			__PackError( __LINE__, __FILE__, GL_INVALID_ENUM,
									 "crPackReadPixels(bad format)" );
			return;
	}

	/* default bytes_per_row so crserver can allocate memory */
	bytes_per_row = width * bytes_per_pixel;

	stride = bytes_per_row;
	if (packstate->alignment != 1) {
		 GLint remainder = bytes_per_row % packstate->alignment;
		 if (remainder)
				stride = bytes_per_row + (packstate->alignment - remainder);
	}

	GET_BUFFERED_POINTER(pc, 44 + sizeof(CRNetworkPointer) );
	WRITE_DATA( 0,  GLint,  x );
	WRITE_DATA( 4,  GLint,  y );
	WRITE_DATA( 8,  GLsizei,  width );
	WRITE_DATA( 12, GLsizei,  height );
	WRITE_DATA( 16, GLenum, format );
	WRITE_DATA( 20, GLenum, type );
	WRITE_DATA( 24, GLint,  stride );
	WRITE_DATA( 28, GLint, packstate->alignment );
	WRITE_DATA( 32, GLint, packstate->skipRows );
	WRITE_DATA( 36, GLint, packstate->skipPixels );
	WRITE_DATA( 40, GLint,  bytes_per_row );
	WRITE_NETWORK_POINTER( 44, (char *) pixels );
	WRITE_OPCODE( pc, CR_READPIXELS_OPCODE );
}

/* Round N up to the next multiple of 8 */
#define CEIL8(N)  (((N) + 7) & ~0x7)

void PACK_APIENTRY crPackBitmap( GLsizei width, GLsizei height, 
		GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove,
		const GLubyte *bitmap, const CRPixelPackState *unpack )
{
	const int isnull = (bitmap == NULL);
	unsigned char *data_ptr;
	int data_length = 0;
	GLubyte *destBitmap = NULL;
	int packet_length = 
		sizeof( width ) + 
		sizeof( height ) +
		sizeof( xorig ) + 
		sizeof( yorig ) + 
		sizeof( xmove ) + 
		sizeof( ymove ) +
		sizeof( GLuint );

	if ( bitmap )
	{
		data_length = CEIL8(width) * height / 8;
		packet_length += data_length;

		data_ptr = (unsigned char *) crPackAlloc( packet_length );
		destBitmap = data_ptr + 28;

		crBitmapCopy(width, height, destBitmap, bitmap, unpack);
		/*
		crMemcpy(destBitmap, bitmap, data_length);
		*/
	}
	else {
		data_ptr = (unsigned char *) crPackAlloc( packet_length );
	}

	WRITE_DATA( 0, GLsizei, width );
	WRITE_DATA( 4, GLsizei, height );
	WRITE_DATA( 8, GLfloat, xorig );
	WRITE_DATA( 12, GLfloat, yorig );
	WRITE_DATA( 16, GLfloat, xmove );
	WRITE_DATA( 20, GLfloat, ymove );
	WRITE_DATA( 24, GLuint, isnull );

	crHugePacket( CR_BITMAP_OPCODE, data_ptr );
}
/*
       ZPix - compressed DrawPixels
*/
void PACK_APIENTRY crPackZPix( GLsizei width, GLsizei height, 
        GLenum format, GLenum type,
        GLenum ztype, GLint zparm, GLint length,
        const GLvoid *pixels,
        const CRPixelPackState *unpackstate )
{
	unsigned char *data_ptr;
	int packet_length;
	(void)unpackstate;

	if (pixels == NULL)
	{
		return;
	}

	packet_length = 
		sizeof( int ) +  /* packet size */
		sizeof( GLenum ) +  /* extended opcode */
		sizeof( width ) +
		sizeof( height ) +
		sizeof( format ) +
		sizeof( type ) +
		sizeof( ztype ) +
		sizeof( zparm ) +
		sizeof( length );

	packet_length += length;

/* XXX JAG
  crDebug("PackZPix: fb %d x %d, state %d, zlen = %d, plen = %d",
                               width, height, ztype, length, packet_length);
*/
	data_ptr = (unsigned char *) crPackAlloc( packet_length );
	WRITE_DATA(  0, GLenum , CR_ZPIX_EXTEND_OPCODE );
	WRITE_DATA(  4, GLsizei, width );
	WRITE_DATA(  8, GLsizei, height );
	WRITE_DATA( 12, GLenum,  format );
	WRITE_DATA( 16, GLenum,  type );
	WRITE_DATA( 20, GLenum,  ztype );
	WRITE_DATA( 24, GLint,   zparm );
	WRITE_DATA( 28, GLint,   length );

        crMemcpy((void *) (data_ptr+32), pixels, length);

	crHugePacket( CR_EXTEND_OPCODE, data_ptr );
}
