/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "unpacker.h"
#include "cr_glwrapper.h"

#include <stdio.h>

void crUnpackCreateContext( void )
{
	cr_unpackDispatch.CreateContext( NULL, NULL );
	INCR_DATA_PTR_NO_ARGS( );
}
