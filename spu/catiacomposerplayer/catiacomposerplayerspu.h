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
	SPUDispatchTable super;//this is the dispatch table of super spu 
    ScenegraphSpuData *superSpuState;//global variable's state of super spu
    FILE *fp;

	/* These handle marker signals */
	int marker_signal;
	char *marker_text;
	void (*old_signal_handler)(int);
} CatiaComposerPlayerSpu;
class CatiaComposerPlayerFunc :public Scenespufunc
{
    public:
        void getUpdatedScene();
        void changeScene();
        void funcNodeUpdate(void(*pt2Func)(void * context, osg::ref_ptr<osg::Group>), void *context);
        void resetClient();
};
extern CatiaComposerPlayerSpu catia_composer_player_spu;
extern void  resetColors();
extern void getUpdatedCatiaComposerPlayerSceneSC();
extern void printspuGatherConfiguration( const SPU *child_spu );
extern void catiaComposerPlayerSPUReset();

