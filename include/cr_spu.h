/* Copyright (c) 2001, Stanford University
 * All rights reserved.
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#ifndef CR_SPU_H
#define CR_SPU_H

#ifdef WINDOWS
#define SPULOAD_APIENTRY __stdcall
#else
#define SPULOAD_APIENTRY
#endif

#include "cr_dll.h"
#include "spu_dispatch_table.h"

#define SPU_ENTRY_POINT_NAME "SPULoad"

typedef struct _SPUSTRUCT SPU;

typedef struct {
	char *name;
	int arg_type;
	int arg_length;
	union { 
		char *string;
		int *intarr;
		float fltarr;
	} value;
} SPUArgs; 

typedef void (*SPUGenericFunction)(void);

typedef struct {
	char *name;
	SPUGenericFunction fn;
} SPUNamedFunctionTable;

typedef struct {
	SPUDispatchTable *childCopy;
	void *data;
	SPUNamedFunctionTable *table;
} SPUFunctions;

typedef SPUFunctions *(*SPUInitFuncPtr)(int id, SPU *child,
		SPU *super, unsigned int, unsigned int );
typedef void (*SPUSelfDispatchFuncPtr)(SPUDispatchTable *);
typedef int (*SPUCleanupFuncPtr)(void);
typedef int (*SPULoadFunction)(char **, char **, void *, void *, void * );

struct _SPUSTRUCT {
	char *name;
	char *super_name;
	struct _SPUSTRUCT *superSPU;
	CRDLL *dll;
	SPULoadFunction entry_point;
	SPUInitFuncPtr init;
	SPUSelfDispatchFuncPtr self;
	SPUCleanupFuncPtr cleanup;
	SPUFunctions *function_table;
	SPUDispatchTable dispatch_table;
};

SPU *crSPULoad( SPU *child, int id, char *name, char *dir );
SPU *crSPULoadChain( int count, int *ids, char **names, char *dir );
void crSPUCopyDispatchTable( SPUDispatchTable *dst, SPUDispatchTable *src );

#endif /* CR_SPU_H */
