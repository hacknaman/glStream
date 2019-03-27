/* Copyright (c) 2001, Stanford University
 * All rights reserved.
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#ifndef PRINTSPU_H
#define PRINTSPU_H

#include "spu_dispatch_table.h"
#include "cr_spu.h"
#include <vector>
#include <osg/LineWidth>
#include <osg/BlendFunc>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Node>
#include <osg/Geometry>
#include <osg/Polygonmode>
#include <osg/Material>
#include <osg/PositionAttitudeTransform>
#include <osg/PolygonStipple>
#include <OpenThreads/Mutex>
#include <osgDB/Export>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/Writefile>

#include <osg/LightSource>
#include <osg/LightModel>
#include <osg/Light>
#include <TransVizUtil.h>
#include <ctime>
#define PRINT_UNUSED(x) ((void)x)
// comment out this code to disable material / lights

//(127-(-128)) where 127 is max value for signed byte and -128 is minimum value

#define BYTEMINMAXDIFF 255
//32-bit signed integer is considered
#define INTMAXMINDIFF  4294967295
//16 bit is considered
#define SHORTMAXMINDIFF 65535
#if defined(WINDOWS)
#define PRINT_APIENTRY __stdcall
#define OSGEXPORT __declspec(dllexport)
#else
#define PRINT_APIENTRY
#endif

#include <stdio.h>
#include <osg/Group>

typedef struct {
	
     int g_ret_count;
   
     ServerAppContentApi::AppContentApi* current_app_instance;
     ServerAppContentApi::NameAndColorInfo* curr_node_color_info;
     std::string curr_geode_name;
     bool isNormalNormalizationEnabled;
     bool isNormalRescaleEnabled;

     osg::Matrix  g_camera_matrix;
     osg::ref_ptr<osg::Vec3Array> g_vertexArray;
     osg::ref_ptr<osg::Vec3Array> g_normalArray;
     osg::ref_ptr<osg::Vec3Array> g_colorArray;

     osg::Vec3 g_CurrentNormal;
     osg::Vec3 g_CurrentColor;
     osg::Matrix g_CurrentMatrix;

     osg::ref_ptr<osg::Geometry> g_geom;
     osg::ref_ptr<osg::Geode> g_geode;
     
     std::vector<osg::Matrix> g_matrix_stack;

     std::vector< osg::ref_ptr<osg::PositionAttitudeTransform> > g_PatArrayDisplayList;

     osg::ref_ptr<osg::StateSet> g_state;

     osg::ref_ptr<osg::Material> g_material;
     osg::ref_ptr<osg::LightSource> g_light[8];

     osg::ref_ptr<osg::Group> g_spuRootGroup;

     int g_geometryMode;
     int g_currentMatrixMode;

     int g_startReading;
     int g_isReading;
     int g_shouldStartReading;
     int g_hasTouchedBegin;
     int g_hasDrawnSomething;

     int g_isDisplayList;
     std::time_t g_time;
     void(*g_pt2Func)(void * context, osg::ref_ptr<osg::Group>);
     void *g_context;    
     int id;
	SPUDispatchTable super;
	FILE *fp;

	/* These handle marker signals */
	int marker_signal;
	char *marker_text;
	void (*old_signal_handler)(int);
} ScenegraphSpuData;
class Scenespufunc : public ISpufunc
{
public:
    OSGEXPORT void getUpdatedScene();
    OSGEXPORT void changeScene();
    OSGEXPORT void funcNodeUpdate(void(*pt2Func)(void * context, osg::ref_ptr<osg::Group>), void *context);
    OSGEXPORT void getContentTree(ServerAppContentApi::ServerContentNode*);
    OSGEXPORT  void resetClient();
};
extern ScenegraphSpuData scenegraph_spu_data;
extern OSGEXPORT ScenegraphSpuData* getScenegraphSpuData();
extern  void getUpdatedSceneSC();
extern void funcNodeUpdateSC(void(*pt2Func)(void * context, osg::ref_ptr<osg::Group>), void *context);

extern void printspuGatherConfiguration( const SPU *child_spu );
extern const char *printspuEnumToStr( GLenum e );
extern const char *printspuListToStr(GLsizei n, GLenum type, const GLvoid *list);
extern void PRINT_APIENTRY scenegraphSPUReset();

extern void PRINT_APIENTRY printGetIntegerv( GLenum pname, GLint *params );
extern void PRINT_APIENTRY printGetFloatv( GLenum pname, GLfloat *params );
extern void PRINT_APIENTRY printGetDoublev( GLenum pname, GLdouble *params );
extern void PRINT_APIENTRY printGetBooleanv( GLenum pname, GLboolean *params );

extern void PRINT_APIENTRY printMaterialfv( GLenum face, GLenum mode, const GLfloat *params );
extern void PRINT_APIENTRY printMaterialiv( GLenum face, GLenum mode, const GLint *params );
extern void PRINT_APIENTRY printLightfv( GLenum light, GLenum pname, const GLfloat *params );
extern void PRINT_APIENTRY printLightiv( GLenum light, GLenum pname, const GLint *params );

extern void PRINT_APIENTRY printLoadMatrixf( const GLfloat *m );
extern void PRINT_APIENTRY printLoadMatrixd( const GLdouble *m );
extern void PRINT_APIENTRY printMultMatrixf( const GLfloat *m );
extern void PRINT_APIENTRY printMultMatrixd( const GLdouble *m );

extern void PRINT_APIENTRY printChromiumParametervCR(GLenum target, GLenum type, GLsizei count, const GLvoid *values);
extern void PRINT_APIENTRY printChromiumParameteriCR(GLenum target, GLint value);
extern void PRINT_APIENTRY printGenTextures( GLsizei n, GLuint * textures );

extern void PRINT_APIENTRY printTexEnvf( GLenum target, GLenum pname, GLfloat param );
extern void PRINT_APIENTRY printTexEnvfv( GLenum target, GLenum pname, const GLfloat * params );
extern void PRINT_APIENTRY printTexEnvi( GLenum target, GLenum pname, GLint param );
extern void PRINT_APIENTRY printTexEnviv( GLenum target, GLenum pname, const GLint * params );
extern void PRINT_APIENTRY printCallLists( GLsizei n, GLenum type, const GLvoid * lists );

#endif /* PRINTSPU_H */
