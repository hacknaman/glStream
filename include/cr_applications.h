/* Copyright (c) 2001, Stanford University
 * All rights reserved.
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#ifndef CR_APPLICATIONS_H
#define CR_APPLICATIONS_H

#include "cr_glwrapper.h"

#ifdef WINDOWS
#define CR_APIENTRY __stdcall
#else
#define CR_APIENTRY
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define CR_SCREEN_BBOX_HINT     0x10000001
#define CR_OBJECT_BBOX_HINT     0x10000002
#define CR_DEFAULT_BBOX_HINT    0x10000003
#define CR_PRINTSPU_STRING_HINT 0x10000004

/* Parallel API Extensions */

#ifndef WINGDIAPI
#define WINGDIAPI
#endif

#ifndef APIENTRY
#define APIENTRY
#endif

WINGDIAPI void APIENTRY glBarrierCreate (GLuint name, GLuint count);
WINGDIAPI void APIENTRY glBarrierDestroy (GLuint name);
WINGDIAPI void APIENTRY glBarrierExec (GLuint name);
WINGDIAPI void APIENTRY glSemaphoreCreate (GLuint name, GLuint count);
WINGDIAPI void APIENTRY glSemaphoreDestroy (GLuint name);
WINGDIAPI void APIENTRY glSemaphoreP (GLuint name);
WINGDIAPI void APIENTRY glSemaphoreV (GLuint name);

void APIENTRY crCreateContext(void);
void APIENTRY crMakeCurrent(void);
void APIENTRY crSwapBuffers(void);

typedef int (CR_APIENTRY *CR_PROC)();
CR_PROC APIENTRY crGetProcAddress( const char *name );

#ifdef __cplusplus
}
#endif

#endif /* CR_APPLICATIONS_H */
