#include "cr_unpack.h"
#include "cr_glwrapper.h"
#include <stdio.h>

void crUnpackDrawPixels( void )
{
	GLsizei width  = READ_DATA( sizeof( int ) + 0, GLsizei );
	GLsizei height = READ_DATA( sizeof( int ) + 4, GLsizei );
	GLenum format  = READ_DATA( sizeof( int ) + 8, GLenum );
	GLenum type    = READ_DATA( sizeof( int ) + 12, GLenum );
	GLvoid *pixels = DATA_POINTER( sizeof( int ) + 16, GLvoid );

	cr_unpackDispatch.PixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
	cr_unpackDispatch.PixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );
	cr_unpackDispatch.PixelStorei( GL_UNPACK_SKIP_ROWS, 0 );
	cr_unpackDispatch.PixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	
	cr_unpackDispatch.DrawPixels( width, height, format, type, pixels );

	INCR_VAR_PTR( );
}

void crUnpackBitmap( void )
{	
	GLsizei width   = READ_DATA( sizeof( int ) + 0, GLsizei );
	GLsizei height  = READ_DATA( sizeof( int ) + 4, GLsizei );
	GLfloat xorig   = READ_DATA( sizeof( int ) + 8, GLfloat );
	GLfloat yorig   = READ_DATA( sizeof( int ) + 12, GLfloat );
	GLfloat xmove   = READ_DATA( sizeof( int ) + 16, GLfloat );
	GLfloat ymove   = READ_DATA( sizeof( int ) + 20, GLfloat );
	GLuint  is_null = READ_DATA( sizeof( int ) + 24, GLuint );
	GLubyte *bitmap = NULL;

	if ( !is_null )
	{
		bitmap = (GLubyte *) DATA_POINTER( sizeof(int) + 28, GLvoid );
	}

	cr_unpackDispatch.PixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
	cr_unpackDispatch.PixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );
	cr_unpackDispatch.PixelStorei( GL_UNPACK_SKIP_ROWS, 0 );
	cr_unpackDispatch.PixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	cr_unpackDispatch.Bitmap( width, height, xorig, yorig, xmove, ymove, bitmap );

	INCR_VAR_PTR( );
}
