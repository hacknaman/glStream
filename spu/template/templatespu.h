/* Copyright (c) 2001, Stanford University
 * All rights reserved.
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#ifndef TEMPLATE_SPU_H
#define TEMPLATE_SPU_H

#ifdef WINDOWS
#define TEMPLATESPU_APIENTRY __stdcall
#else
#define TEMPLATESPU_APIENTRY
#endif

#include "cr_spu.h"

void templatespuGatherConfiguration( void );

typedef struct {
	int id;
	int has_child;
	SPUDispatchTable child, super;
} TemplateSPU;

extern TemplateSPU template_spu;

#endif /* TEMPLATE_SPU_H */
