#include <stdio.h>
#include "cr_error.h"
#include "cr_spu.h"
#include "avevascenegraphspu.h"

void PRINT_APIENTRY printTexEnvf( GLenum target, GLenum pname, GLfloat param )
{
	switch(pname) {
	    case GL_TEXTURE_ENV_MODE:
        // MCH: This cast is needed to prevent warnings but might not be the right thing...    
		break;
	    default:
		break;
	}
}

void PRINT_APIENTRY printTexEnvfv( GLenum target, GLenum pname, const GLfloat * params )
{
	switch(pname) {
	    case GL_TEXTURE_ENV_MODE:
        // MCH: This cast is needed to prevent warnings but might not be the right thing...    
		break;
	    case GL_TEXTURE_ENV_COLOR:
		break;
	    default:
		break;
	}
}

void PRINT_APIENTRY printTexEnvi( GLenum target, GLenum pname, GLint param )
{
}

void PRINT_APIENTRY printTexEnviv( GLenum target, GLenum pname, const GLint * params )
{
	switch(pname) {
	    case GL_TEXTURE_ENV_MODE:
		break;
	    case GL_TEXTURE_ENV_COLOR:
		break;
	    default:
		break;
	}
}
