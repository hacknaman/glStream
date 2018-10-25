/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "cr_error.h"
#include "avevascenegraphspu.h"

void PRINT_APIENTRY printChromiumParametervCR(GLenum target, GLenum type, GLsizei count, const GLvoid *values)
{
	int i ;
	switch (target) {
	case GL_PRINT_STRING_CR:
		break;
	default:
		switch( type ) {
			case GL_INT:
				for (i=0; i<count; i++)
				break ;
			case GL_UNSIGNED_INT:
				for (i=0; i<count; i++)
				break ;
			case GL_SHORT:
				for (i=0; i<count; i++)
				break ;
			case GL_UNSIGNED_SHORT:
				for (i=0; i<count; i++)
				break ;
			case GL_BYTE:
				for (i=0; i<count; i++)
				break ;
			case GL_UNSIGNED_BYTE:
				for (i=0; i<count; i++)
				break ;
			case GL_FLOAT:
				for (i=0; i<count; i++)
				break ;
			case GL_DOUBLE:
				for (i=0; i<count; i++)
				break ;
			default:
				break ;
		}
	}
}

void PRINT_APIENTRY printChromiumParameteriCR(GLenum target, GLint value)
{
	switch (target) {
	default:
		break;
	}
}

void PRINT_APIENTRY printGenTextures( GLsizei n, GLuint * textures )
{
	int i;
	for (i = 0; i < n; i++) {
	}
}
