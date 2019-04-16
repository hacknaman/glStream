/* Copyright (c) 2001, Stanford University
 * All rights reserved.
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */


#include "spu_dispatch_table.h"
#include "cr_spu.h"
#include "scenegraph\scenegraphspu.h"
#if defined(WINDOWS)
#define PRINT_APIENTRY __stdcall
#define OSGEXPORT __declspec(dllexport)
#else
#define PRINT_APIENTRY
#endif

#include <stdio.h>
#include <osg/Group>
typedef struct {
	
    int id;
    SPUDispatchTable super;
    ScenegraphSpuData* superSpuState;//this will hold super spu global variable's states
    FILE *fp;

	/* These handle marker signals */
	int marker_signal;
	char *marker_text;
	void (*old_signal_handler)(int);
} CatiaSpu;
class CatiaSpufunc : public Scenespufunc{};
extern CatiaSpu catia_spu;
extern void printspuGatherConfiguration( const SPU *child_spu );
extern void scenegraphCatiaSPUReset();
















