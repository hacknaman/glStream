#include "cr_unpack.h"
#include "cr_glwrapper.h"
#include "cr_error.h"
#include "cr_mem.h"

void crUnpackMap2d( void  )
{
	GLenum target = READ_DATA( sizeof( int ) + 0, GLenum );
	GLdouble u1 = READ_DOUBLE( sizeof( int ) + 4 );
	GLdouble u2 = READ_DOUBLE( sizeof( int ) + 12 );
	GLint ustride = READ_DATA( sizeof( int ) + 20, GLint );
	GLint uorder = READ_DATA( sizeof( int ) + 24, GLint );
	GLdouble v1 = READ_DOUBLE( sizeof( int ) + 28 );
	GLdouble v2 = READ_DOUBLE( sizeof( int ) + 36 );
	GLint vstride = READ_DATA( sizeof( int ) + 44, GLint );
	GLint vorder = READ_DATA( sizeof( int ) + 48, GLint );

#ifdef WIREGL_UNALIGNED_ACCESS_OKAY
	GLdouble *points = DATA_POINTER( sizeof(int) + 52, GLdouble );

	cr_unpackDispatch.Map2d( target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, 
			 points );
#else
	int n_points = READ_DATA( 0, int ) - ( sizeof( int ) + 52 );
	GLdouble *points;

	if ( n_points & 0x7 )
		crError( "crUnpackMap2d: n_points=%d, expected multiple of 8\n", n_points );
	points = (GLdouble *) crAlloc( n_points );
	memcpy( points, DATA_POINTER( sizeof(int) + 52, GLdouble ), n_points );

	cr_unpackDispatch.Map2d( target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, 
			 points );
	crFree( points );
#endif

	INCR_VAR_PTR();
}

void crUnpackMap2f( void  )
{
	GLenum target = READ_DATA( sizeof( int ) + 0, GLenum );
	GLfloat u1 = READ_DATA( sizeof( int ) + 4, GLfloat );
	GLfloat u2 = READ_DATA( sizeof( int ) + 8, GLfloat );
	GLint ustride = READ_DATA( sizeof( int ) + 12, GLint );
	GLint uorder = READ_DATA( sizeof( int ) + 16, GLint );
	GLfloat v1 = READ_DATA( sizeof( int ) + 20, GLfloat );
	GLfloat v2 = READ_DATA( sizeof( int ) + 24, GLfloat );
	GLint vstride = READ_DATA( sizeof( int ) + 28, GLint );
	GLint vorder = READ_DATA( sizeof( int ) + 32, GLint );
	GLfloat *points = DATA_POINTER( sizeof( int ) + 36 , GLfloat );

	cr_unpackDispatch.Map2f( target, u1, u2, ustride, uorder, v1, v2, vstride, vorder,
			 points );
	INCR_VAR_PTR();
}

void crUnpackMap1d( void  )
{
	GLenum target = READ_DATA( sizeof( int ) + 0, GLenum );
	GLdouble u1 = READ_DOUBLE( sizeof( int ) + 4 );
	GLdouble u2 = READ_DOUBLE( sizeof( int ) + 12 );
	GLint stride = READ_DATA( sizeof( int ) + 20, GLint );
	GLint order = READ_DATA( sizeof( int ) + 24, GLint );

#ifdef WIREGL_UNALIGNED_ACCESS_OKAY
	GLdouble *points = DATA_POINTER( sizeof(int) + 28, GLdouble );
	cr_unpackDispatch.Map1d( target, u1, u2, stride, order, points );
#else
	int n_points = READ_DATA( 0, int ) - ( sizeof(int) + 28 );
	GLdouble *points;

	if ( n_points & 0x7 )
		crError( "crUnpackMap1d: n_points=%d, expected multiple of 8\n", n_points );
	points = (GLdouble *) crAlloc( n_points );
	memcpy( points, DATA_POINTER( sizeof(int) + 28, GLdouble ), n_points );
	
	cr_unpackDispatch.Map1d( target, u1, u2, stride, order, points );
	crFree( points );
#endif

	INCR_VAR_PTR();
}

void crUnpackMap1f( void  )
{
	GLenum target = READ_DATA( sizeof( int ) + 0, GLenum );
	GLfloat u1 = READ_DATA( sizeof( int ) + 4, GLfloat );
	GLfloat u2 = READ_DATA( sizeof( int ) + 8, GLfloat );
	GLint stride = READ_DATA( sizeof( int ) + 12, GLint );
	GLint order = READ_DATA( sizeof( int ) + 16, GLint );
	GLfloat *points = DATA_POINTER( sizeof( int ) + 20, GLfloat );

	cr_unpackDispatch.Map1f( target, u1, u2, stride, order, points );
	INCR_VAR_PTR();
}
