/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "scenegraphspu.h"

void PRINT_APIENTRY printMaterialiv( GLenum face, GLenum mode, const GLint *params )
{
	int i;
	int num_params = 4;

	if (mode == GL_SHININESS)
	{
		num_params = 1;
	}

	for (i = 0 ; i < num_params ; i++)
	{
		if (i != num_params -1)
		{
		}
	}
}

void PRINT_APIENTRY printLightfv( GLenum light, GLenum pname, const GLfloat *params )
{
	int i;
	int num_params = 4;

	if (pname == GL_SPOT_EXPONENT || pname == GL_SPOT_CUTOFF || pname == GL_CONSTANT_ATTENUATION || pname == GL_LINEAR_ATTENUATION || pname == GL_QUADRATIC_ATTENUATION )
	{
		num_params = 1;
	}

	for (i = 0 ; i < num_params ; i++)
	{
		if (i != num_params -1)
		{
		}
	}
}

void PRINT_APIENTRY printLightiv( GLenum light, GLenum pname, const GLint *params )
{
	int i;
	int num_params = 4;

	if (pname == GL_SPOT_EXPONENT || pname == GL_SPOT_CUTOFF || pname == GL_CONSTANT_ATTENUATION || pname == GL_LINEAR_ATTENUATION || pname == GL_QUADRATIC_ATTENUATION )
	{
		num_params = 1;
	}

	for (i = 0 ; i < num_params ; i++)
	{
		if (i != num_params -1)
		{
		}
	}
}

