#include "cr_unpack.h"
#include "cr_glwrapper.h"

void crUnpackFogfv( void  )
{
	GLenum pname = READ_DATA( sizeof( int ) + 0, GLenum );
	GLfloat *params = DATA_POINTER( sizeof( int ) + 4, GLfloat );

	cr_unpackDispatch.Fogfv( pname, params );
	INCR_VAR_PTR();
}

void crUnpackFogiv( void  )
{
	GLenum pname = READ_DATA( sizeof( int ) + 0, GLenum );
	GLint *params = DATA_POINTER( sizeof( int ) + 4, GLint );

	cr_unpackDispatch.Fogiv( pname, params );
	INCR_VAR_PTR();
}
