/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include <stdio.h>
#include "cr_error.h"
#include "cr_version.h"
#include "state.h"
#include "state/cr_statetypes.h"


const GLubyte * STATE_APIENTRY crStateGetString( GLenum name )
{
	CRContext *g = GetCurrentContext();
	if (!g)
		return NULL;

	switch( name )
	{
		case GL_VENDOR:
			return (const GLubyte *) CR_VENDOR;
		case GL_RENDERER:
			return (const GLubyte *) CR_RENDERER;
		case GL_VERSION:
			return g->extensions.version;
		case GL_EXTENSIONS:
			/* This shouldn't normally be queried - the relevant SPU should
			 * catch this query and do all the extension string merging/mucking.
			 */
			return (const GLubyte *) __stateExtensionString;
			/*return g->limits.extensions;  OLD colde */
		default:
			crStateError( __LINE__, __FILE__, GL_INVALID_ENUM,
									"calling glGetString() with invalid name" );
			return NULL;
	}
}
