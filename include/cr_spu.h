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
#include "state/cr_limits.h"
#include "cr_net.h"

#define SPU_ENTRY_POINT_NAME "SPULoad"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _SPUSTRUCT SPU;

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

typedef void (*SPUOptionCB)( void *spu, const char *response );

typedef struct {
	const char *option;
	enum { BOOL, INT, FLOAT, STRING } type; 
	int numValues;  /* usually 1 */
	const char *deflt;  /* default value, as a string */
	const char *min;
	const char *max;
	const char *description;
	SPUOptionCB cb;
} SPUOptions, *SPUOptionsPtr;


typedef SPUFunctions *(*SPUInitFuncPtr)(int id, SPU *child,
		SPU *super, unsigned int, unsigned int );
typedef void (*SPUSelfDispatchFuncPtr)(SPUDispatchTable *);
typedef int (*SPUCleanupFuncPtr)(void);
typedef int (*SPULoadFunction)(char **, char **, void *, void *, void *,
			       SPUOptionsPtr *);

struct _SPUSTRUCT {
	char *name;
	char *super_name;
	int id;
	struct _SPUSTRUCT *superSPU;
	CRDLL *dll;
	SPULoadFunction entry_point;
	SPUInitFuncPtr init;
	SPUSelfDispatchFuncPtr self;
	SPUCleanupFuncPtr cleanup;
	SPUFunctions *function_table;
	SPUOptions *options;
	SPUDispatchTable dispatch_table;
};

SPU *crSPULoad( SPU *child, int id, char *name, char *dir, void *server);
SPU *crSPULoadChain( int count, int *ids, char **names, char *dir, void *server );
void crSPUInitDispatchTable( SPUDispatchTable *table );
void crSPUCopyDispatchTable( SPUDispatchTable *dst, SPUDispatchTable *src );
void crSPUChangeInterface( SPUDispatchTable *table, void *origFunc, void *newFunc );


void crSPUSetDefaultParams( void *spu, SPUOptions *options );
void crSPUGetMothershipParams( CRConnection *conn, void *spu, SPUOptions *options );


SPUGenericFunction crSPUFindFunction( const SPUNamedFunctionTable *table, const char *fname );
void crSPUInitDispatch( SPUDispatchTable *dispatch, const SPUNamedFunctionTable *table );
void crSPUInitDispatchNops(SPUDispatchTable *table);

void crSPUInitGLLimits( CRLimitsState *limits );
void crSPUCopyGLLimits( CRLimitsState *dest, const CRLimitsState *src );
void crSPUQueryGLLimits( CRConnection *conn, int spu_id, CRLimitsState *limits );
void crSPUReportGLLimits( const CRLimitsState *limits, int spu_id );
void crSPUGetGLLimits( const SPUNamedFunctionTable *table, CRLimitsState *limits );
void crSPUMergeGLLimits( int n, const CRLimitsState *limits, CRLimitsState *merged );
void crSPUPropogateGLLimits( CRConnection *conn, int spu_id, const SPU *child_spu, CRLimitsState *limitsResult );

int crLoadOpenGL( crOpenGLInterface *crInterface, SPUNamedFunctionTable table[] );
int crLoadOpenGLExtensions( const crOpenGLInterface *crInterface, SPUNamedFunctionTable table[] );

#ifdef __cplusplus
}
#endif

#endif /* CR_SPU_H */
