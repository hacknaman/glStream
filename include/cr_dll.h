/* Copyright (c) 2001, Stanford University
 * All rights reserved.
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#ifndef CR_DLL_H
#define CR_DLL_H

#if defined(WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

typedef struct {
	char *name;
#if defined(WINDOWS)
	HINSTANCE hinstLib;
#elif defined(IRIX) || defined(IRIX64) || defined(Linux) || defined(FreeBSD) || defined(__APPLE__)
	void *hinstLib;
#else
#error ARCHITECTURE DLL NOT IMPLEMENTED
#endif
} CRDLL;

typedef void (*CRDLLFunc)(void);
CRDLL *crDLLOpen( const char *dllname );
CRDLLFunc crDLLGetNoError( CRDLL *dll, const char *symname );
CRDLLFunc crDLLGet( CRDLL *dll, const char *symname );
void crDLLClose( CRDLL *dll );

#endif /* CR_DLL_H */
