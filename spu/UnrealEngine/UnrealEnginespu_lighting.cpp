/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "UnrealEnginespu.h"

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

