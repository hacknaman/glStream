#include "cr_unpack.h"
#include "cr_glwrapper.h"

void crUnpackPixelMapfv( void  )
{
	GLenum map = READ_DATA( sizeof( int ) + 0, GLenum );
	GLsizei mapsize = READ_DATA( sizeof( int ) + 4, GLsizei );
	GLfloat *values = DATA_POINTER( sizeof( int ) + 8, GLfloat );

	cr_unpackDispatch.PixelMapfv( map, mapsize, values );
	INCR_VAR_PTR();
}

void crUnpackPixelMapuiv( void  )
{
	GLenum map = READ_DATA( sizeof( int ) + 0, GLenum );
	GLsizei mapsize = READ_DATA( sizeof( int ) + 4, GLsizei );
	GLuint *values = DATA_POINTER( sizeof( int ) + 8, GLuint );
		
	cr_unpackDispatch.PixelMapuiv( map, mapsize, values );
	INCR_VAR_PTR();
}

void crUnpackPixelMapusv( void  )
{
	GLenum map = READ_DATA( sizeof( int ) + 0, GLenum );
	GLsizei mapsize = READ_DATA( sizeof( int ) + 4, GLsizei );
	GLushort *values = DATA_POINTER( sizeof( int ) + 8, GLushort );

	cr_unpackDispatch.PixelMapusv( map, mapsize, values );
	INCR_VAR_PTR();
}
