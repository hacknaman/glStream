/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "cr_applications.h"
#include "cr_spu.h"

#include <stdio.h>

extern SPU *stub_spu;
extern void StubInit(void);

void APIENTRY crCreateContext( void )
{
	StubInit();
	stub_spu->dispatch_table.CreateContext( NULL, NULL );
}

void APIENTRY crMakeCurrent( void )
{
	stub_spu->dispatch_table.MakeCurrent( );
}

void APIENTRY crSwapBuffers( void )
{
	stub_spu->dispatch_table.SwapBuffers( );
}
