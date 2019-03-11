/* Copyright (c) 2001, Stanford University
 * All rights reserved.
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

/*#ifndef PRINTSPU_H
#define PRINTSPU_H*/

#include "spu_dispatch_table.h"
#include "cr_spu.h"
#include "scenegraph\scenegraphspu.h"
#include <ReviewCpp.h>
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
    ReviewCppWrapper::ReviewCppAPI rapi;
    ReviewCppWrapper::Element RootElement;
    std::vector<ReviewCppWrapper::Element*> ElementSequence;
    std::vector<osg::ref_ptr<osg::Group> > g_spuGroupMap;
    int sequence_index;
    int depth_value;
    std::string camerashakeapp;
	SPUDispatchTable super;
    ScenegraphSpuData *superSpuState;
    FILE *fp;

	/* These handle marker signals */
	int marker_signal;
	char *marker_text;
	void (*old_signal_handler)(int);
} AvevaSpu;

class Avevaspufunc : public Scenespufunc
{
public:
    void getUpdatedScene();
    void changeScene();
    void funcNodeUpdate(void(*pt2Func)(void * context, osg::ref_ptr<osg::Group>), void *context);
    void resetClient();
    
};
extern AvevaSpu aveva_spu;

extern void getUpdatedAvevaSceneASC();
extern  void resetClientASC();
extern void printspuGatherConfiguration(const SPU *child_spu);

extern void avevaSPUReset();
