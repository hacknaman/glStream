/* Copyright (c) 2001, Stanford University
All rights reserved.

See the file LICENSE.txt for information on redistributing this software. */

#include <stdio.h>
#include "cr_error.h"
#include "cr_spu.h"
#include "scenegraphspu.h"
#include <osgUtil\Optimizer>

// Transform node is added so that geometry diffentiation could be done
// Secondly, Transform node help in setting the normals right.
// *** This has been disabled because the program crashes while useing avevascenegraphSPU
//     and some initial state seems wrong, which is why city app doesn't work well.
//     there is a block at the center. ***

// #define DISABLE_TRANSFORM_MULT_WITH_VERTICES


// This is enabled for part selection in aveva
#define GEODE_WITH_LM
#define ENABLE_MATERIAL
//#define ENABLE_LIGHTS

extern void PRINT_APIENTRY scenegraphSPUReset()
{
    scenegraph_spu_data.g_ret_count = 2000;

    scenegraph_spu_data.g_CurrentNormal = osg::Vec3(0.0, 1.0, 0.0);
    scenegraph_spu_data.g_CurrentColor = osg::Vec4(1.0, 1.0, 1.0, 1.0);

#ifdef ENABLE_LIGHTS
    for (auto light : scenegraph_spu_data.g_light)
    {
        scenegraph_spu_data.light = NULL;
    }
#endif

    scenegraph_spu_data.g_PatArrayDisplayList.clear();
    scenegraph_spu_data.isPartSelectionEnabled = true;
    scenegraph_spu_data.curr_geode_name = "";
    scenegraph_spu_data.g_spuRootGroup = new osg::Group;
    scenegraph_spu_data.g_state = new osg::StateSet;
#ifdef DISABLE_TRANSFORM_MULT_WITH_VERTICES
    scenegraph_spu_data.g_current_transform = NULL;
#endif
    scenegraph_spu_data.g_geometryMode = -1;
    scenegraph_spu_data.g_CurrentMatrix = osg::Matrix();
    scenegraph_spu_data.g_startReading = false;
    scenegraph_spu_data.g_isReading = false;
    scenegraph_spu_data.g_shouldStartReading = false;
    scenegraph_spu_data.g_hasTouchedBegin = false;

    scenegraph_spu_data.g_isDisplayList = false;

    scenegraph_spu_data.g_time = std::time(0);
}

std::string camerashakeapp;

extern void getUpdatedSceneSC(){
    scenegraph_spu_data.g_shouldStartReading = true;
}

void CreateNewGeode()
{
    if (scenegraph_spu_data.g_needToCreateNewGeode)
    {
        scenegraph_spu_data.g_geode = new osg::Geode();
#ifdef DISABLE_TRANSFORM_MULT_WITH_VERTICES
        scenegraph_spu_data.g_current_transform = new osg::MatrixTransform;
        scenegraph_spu_data.g_current_transform->setMatrix(scenegraph_spu_data.g_CurrentMatrix * scenegraph_spu_data.g_camera_matrix);
        scenegraph_spu_data.g_current_transform->addChild(scenegraph_spu_data.g_geode);
        scenegraph_spu_data.g_spuRootGroup->addChild(scenegraph_spu_data.g_current_transform);
#else

        scenegraph_spu_data.g_spuRootGroup->addChild(scenegraph_spu_data.g_geode);

#endif


        scenegraph_spu_data.g_needToCreateNewGeode = false;
    }
}
extern void funcNodeUpdateSC(void(*pt2Func)(void * context, osg::ref_ptr<osg::Group>), void * context){

    scenegraph_spu_data.g_pt2Func = pt2Func;
    scenegraph_spu_data.g_context = context;
}
extern OSGEXPORT ScenegraphSpuData* getScenegraphSpuData()
{
    return &scenegraph_spu_data;
}

extern OSGEXPORT void readConfigFile()
{
  
}

static void PRINT_APIENTRY printActiveTextureARB(GLenum texture)
{

}

static void PRINT_APIENTRY printAlphaFunc(GLenum func, GLclampf ref)
{
}

static GLboolean PRINT_APIENTRY printAreProgramsResidentNV(GLsizei n, const GLuint * ids, GLboolean * residences)
{
    return false;
}

static GLboolean PRINT_APIENTRY printAreTexturesResident(GLsizei n, const GLuint * textures, GLboolean * residences)
{
    return false;
}

static void PRINT_APIENTRY printArrayElement(GLint i)
{
}

static void PRINT_APIENTRY printBarrierCreateCR(GLuint name, GLuint count)
{
}

static void PRINT_APIENTRY printBarrierDestroyCR(GLuint name)
{
}

static void PRINT_APIENTRY printBarrierExecCR(GLuint name)
{
}

static void PRINT_APIENTRY printBegin(GLenum mode)
{
    if (scenegraph_spu_data.g_isReading)
    {
        CreateNewGeode();
        scenegraph_spu_data.g_hasTouchedBegin = true;
        scenegraph_spu_data.g_geometryMode = mode;
        scenegraph_spu_data.g_vertexArray = new osg::Vec3Array();
        scenegraph_spu_data.g_normalArray = new osg::Vec3Array();
        scenegraph_spu_data.g_colorArray = new osg::Vec4Array();

    }

    if (scenegraph_spu_data.g_isDisplayList)
    {
        scenegraph_spu_data.g_geometryMode = mode;
        scenegraph_spu_data.g_geode = new osg::Geode();
        scenegraph_spu_data.g_vertexArray = new osg::Vec3Array();
        scenegraph_spu_data.g_normalArray = new osg::Vec3Array();
        scenegraph_spu_data.g_colorArray = new osg::Vec4Array();
    }
}

static void PRINT_APIENTRY printBeginQueryARB(GLenum target, GLuint id)
{
}

static void PRINT_APIENTRY printBindBufferARB(GLenum target, GLuint buffer)
{
}

static void PRINT_APIENTRY printBindProgramARB(GLenum target, GLuint program)
{
}

static void PRINT_APIENTRY printBindProgramNV(GLenum target, GLuint id)
{
}

static void PRINT_APIENTRY printBindTexture(GLenum target, GLuint texture)
{
}

static void PRINT_APIENTRY printBitmap(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte * bitmap)
{
}

static void PRINT_APIENTRY printBlendColorEXT(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
}

static void PRINT_APIENTRY printBlendEquationEXT(GLenum mode)
{
}

// XXX:- need to handle blend function if gl_blend mode is on
static void PRINT_APIENTRY printBlendFunc(GLenum sfactor, GLenum dfactor)
{
}

static void PRINT_APIENTRY printBlendFuncSeparateEXT(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorA, GLenum dfactorA)
{
}

static void PRINT_APIENTRY printBoundsInfoCR(const CRrecti * bounds, const GLbyte * payload, GLint len, GLint num_opcodes)
{
}

static void PRINT_APIENTRY printBufferDataARB(GLenum target, GLsizeiptrARB size, const GLvoid * data, GLenum usage)
{
}

static void PRINT_APIENTRY printBufferSubDataARB(GLenum target, GLintptrARB offset, GLsizeiptrARB size, const GLvoid * data)
{
}

static void PRINT_APIENTRY printCallList(GLuint list)
{
    if (scenegraph_spu_data.g_isReading) {
        scenegraph_spu_data.g_hasTouchedBegin = true;
        int listIndexInInt = list;
        int displayListSizeLessOne = scenegraph_spu_data.g_PatArrayDisplayList.size() - 1;
        if (listIndexInInt <= displayListSizeLessOne)
        {
            //g_PatArray.back()->addChild(g_PatArrayDisplayList[list - 1].get());

        }
    }
}

static void PRINT_APIENTRY printChromiumParameterfCR(GLenum target, GLfloat value)
{
}

static void PRINT_APIENTRY printClear(GLbitfield mask)
{
}

static void PRINT_APIENTRY printClearAccum(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
}

static void PRINT_APIENTRY printClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
}

static void PRINT_APIENTRY printClearDepth(GLclampd depth)
{
}

static void PRINT_APIENTRY printClearIndex(GLfloat c)
{
}

static void PRINT_APIENTRY printClearStencil(GLint s)
{
}

static void PRINT_APIENTRY printClientActiveTextureARB(GLenum texture)
{
}

static void PRINT_APIENTRY printClipPlane(GLenum plane, const GLdouble * equation)
{
}

double last_color[4];

static void PRINT_APIENTRY printColor4d(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha)
{
    if (scenegraph_spu_data.g_isReading)
    {
        if (!(last_color[0] == red && last_color[1] == green && last_color[2] == blue && last_color[3] == alpha))
        {
            last_color[0] = red;
            last_color[1] = green;
            last_color[2] = blue;
            last_color[3] = alpha;
            scenegraph_spu_data.g_needToCreateNewGeode = true;
        }
        scenegraph_spu_data.g_CurrentColor = osg::Vec4(red, green, blue, alpha);
    }
}


static void PRINT_APIENTRY printColor3d(GLdouble red, GLdouble green, GLdouble blue)
{
    printColor4d(red, green, blue, 1.0);
}

static void PRINT_APIENTRY printColor3dv(const GLdouble * v)
{
    printColor3d(v[0], v[1], v[2]);
}

static void PRINT_APIENTRY printColor3b(GLbyte red, GLbyte green, GLbyte blue)
{
    printColor3d(red, green, blue);
}

static void PRINT_APIENTRY printColor3bv(const GLbyte * v)
{
    printColor3b(v[0], v[1], v[2]);
}

static void PRINT_APIENTRY printColor3f(GLfloat red, GLfloat green, GLfloat blue)
{
    printColor3d(red, green, blue);
}

static void PRINT_APIENTRY printColor3fv(const GLfloat * v)
{
    printColor3f(v[0], v[1], v[2]);
}

static void PRINT_APIENTRY printColor3i(GLint red, GLint green, GLint blue)
{
    printColor3d(red, green, blue);
}

static void PRINT_APIENTRY printColor3iv(const GLint * v)
{
    printColor3i(v[0], v[1], v[2]);
}

static void PRINT_APIENTRY printColor3s(GLshort red, GLshort green, GLshort blue)
{
    printColor3d(red, green, blue);
}

static void PRINT_APIENTRY printColor3sv(const GLshort * v)
{
    printColor3s(v[0], v[1], v[2]);
}

static void PRINT_APIENTRY printColor3ub(GLubyte red, GLubyte green, GLubyte blue)
{
    printColor3d(red, green, blue);
}

static void PRINT_APIENTRY printColor3ubv(const GLubyte * v)
{
    printColor3ub(v[0], v[1], v[2]);
}

static void PRINT_APIENTRY printColor3ui(GLuint red, GLuint green, GLuint blue)
{
    printColor3d(red, green, blue);
}

static void PRINT_APIENTRY printColor3uiv(const GLuint * v)
{
    printColor3ui(v[0], v[1], v[2]);
}

static void PRINT_APIENTRY printColor3us(GLushort red, GLushort green, GLushort blue)
{
    printColor3d(red, green, blue);
}

static void PRINT_APIENTRY printColor3usv(const GLushort * v)
{
    printColor3us(v[0], v[1], v[2]);
}

static void PRINT_APIENTRY printColor4b(GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha)
{
    printColor4d(red, green, blue, alpha);
}

static void PRINT_APIENTRY printColor4bv(const GLbyte * v)
{
    printColor4b(v[0], v[1], v[2], v[3]);
}

static void PRINT_APIENTRY printColor4dv(const GLdouble * v)
{
    printColor4d(v[0], v[1], v[2], v[3]);
}

static void PRINT_APIENTRY printColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    printColor4d(red, green, blue, alpha);
}

static void PRINT_APIENTRY printColor4fv(const GLfloat * v)
{
    printColor4f(v[0], v[1], v[2], v[3]);
}

static void PRINT_APIENTRY printColor4i(GLint red, GLint green, GLint blue, GLint alpha)
{
    double r = GLdouble(2 * red + 1) / GLdouble(INTMAXMINDIFF);
    double g = GLdouble(2 * green + 1) / GLdouble(INTMAXMINDIFF);
    double b = GLdouble(2 * blue + 1) / GLdouble(INTMAXMINDIFF);
    double a = GLdouble(2 * alpha + 1) / GLdouble(INTMAXMINDIFF);
    printColor4d(r, g, b, a);
}

static void PRINT_APIENTRY printColor4iv(const GLint * v)
{
    printColor4i(v[0], v[1], v[2], v[3]);
}

static void PRINT_APIENTRY printColor4s(GLshort red, GLshort green, GLshort blue, GLshort alpha)
{
    double r = GLdouble(2 * red + 1) / GLdouble(SHORTMAXMINDIFF);
    double g = GLdouble(2 * green + 1) / GLdouble(SHORTMAXMINDIFF);
    double b = GLdouble(2 * blue + 1) / GLdouble(SHORTMAXMINDIFF);
    double a = GLdouble(2 * alpha + 1) / GLdouble(SHORTMAXMINDIFF);
    printColor4d(r, g, b, a);
}

static void PRINT_APIENTRY printColor4sv(const GLshort * v)
{
    printColor4d(v[0], v[1], v[2], v[3]);
}

static void PRINT_APIENTRY printColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
    printColor4d((GLdouble)red / GLdouble(BYTEMINMAXDIFF), (GLdouble)green / GLdouble(BYTEMINMAXDIFF), (GLdouble)blue / GLdouble(BYTEMINMAXDIFF), (GLdouble)alpha / GLdouble(BYTEMINMAXDIFF));
}

static void PRINT_APIENTRY printColor4ubv(const GLubyte * v)
{
    printColor4ub(v[0], v[1], v[2], v[3]);
}

static void PRINT_APIENTRY printColor4ui(GLuint red, GLuint green, GLuint blue, GLuint alpha)
{
    double r = GLdouble(red) / GLdouble(INTMAXMINDIFF);
    double g = GLdouble(green) / GLdouble(INTMAXMINDIFF);
    double b = GLdouble(blue) / GLdouble(INTMAXMINDIFF);
    double a = GLdouble(alpha) / GLdouble(INTMAXMINDIFF);
    printColor4d(r, g, b, a);
}

static void PRINT_APIENTRY printColor4uiv(const GLuint * v)
{
    printColor4ui(v[0], v[1], v[2], v[3]);
}

static void PRINT_APIENTRY printColor4us(GLushort red, GLushort green, GLushort blue, GLushort alpha)
{
    printColor4d(red, green, blue, alpha);
}

static void PRINT_APIENTRY printColor4usv(const GLushort * v)
{
    printColor4us(v[0], v[1], v[2], v[3]);
}

static void PRINT_APIENTRY printColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
}

static void PRINT_APIENTRY printColorMaterial(GLenum face, GLenum mode)
{


    if (!scenegraph_spu_data.g_isReading)
    {
        return;
    }

#ifdef ENABLE_MATERIAL

    if (scenegraph_spu_data.g_material == NULL)
    {
        scenegraph_spu_data.g_material = new osg::Material();
    }

    switch (mode)
    {
    case GL_AMBIENT:
        scenegraph_spu_data.g_material->setColorMode(osg::Material::ColorMode::AMBIENT);
        break;
    case GL_DIFFUSE:
        scenegraph_spu_data.g_material->setColorMode(osg::Material::ColorMode::DIFFUSE);
        break;
    case GL_SPECULAR:
        scenegraph_spu_data.g_material->setColorMode(osg::Material::ColorMode::SPECULAR);
        break;
    case GL_AMBIENT_AND_DIFFUSE:
        scenegraph_spu_data.g_material->setColorMode(osg::Material::ColorMode::AMBIENT_AND_DIFFUSE);
        break;
    case GL_EMISSION:
        scenegraph_spu_data.g_material->setColorMode(osg::Material::ColorMode::EMISSION);
        break;
    }

#endif


}

static void PRINT_APIENTRY printColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{

    //no need to handle already handled in packSpu

}

static void PRINT_APIENTRY printCombinerInputNV(GLenum stage, GLenum portion, GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage)
{
}

static void PRINT_APIENTRY printCombinerOutputNV(GLenum stage, GLenum portion, GLenum abOutput, GLenum cdOutput, GLenum sumOutput, GLenum scale, GLenum bias, GLboolean abDotProduct, GLboolean cdDotProduct, GLboolean muxSum)
{
}

static void PRINT_APIENTRY printCombinerParameterfNV(GLenum pname, GLfloat param)
{
}

static void PRINT_APIENTRY printCombinerParameterfvNV(GLenum pname, const GLfloat * params)
{
}

static void PRINT_APIENTRY printCombinerParameteriNV(GLenum pname, GLint param)
{
}

static void PRINT_APIENTRY printCombinerParameterivNV(GLenum pname, const GLint * params)
{
}

static void PRINT_APIENTRY printCombinerStageParameterfvNV(GLenum stage, GLenum pname, const GLfloat * params)
{
}

static void PRINT_APIENTRY printCompressedTexImage1DARB(GLenum target, GLint level, GLenum internalFormat, GLsizei width, GLint border, GLsizei imagesize, const GLvoid * data)
{
}

static void PRINT_APIENTRY printCompressedTexImage2DARB(GLenum target, GLint level, GLenum internalFormat, GLsizei width, GLsizei height, GLint border, GLsizei imagesize, const GLvoid * data)
{
}

static void PRINT_APIENTRY printCompressedTexImage3DARB(GLenum target, GLint level, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imagesize, const GLvoid * data)
{
}

static void PRINT_APIENTRY printCompressedTexSubImage1DARB(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imagesize, const GLvoid * data)
{
}

static void PRINT_APIENTRY printCompressedTexSubImage2DARB(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imagesize, const GLvoid * data)
{
}

static void PRINT_APIENTRY printCompressedTexSubImage3DARB(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imagesize, const GLvoid * data)
{
}

static void PRINT_APIENTRY printCopyPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type)
{
}

static void PRINT_APIENTRY printCopyTexImage1D(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border)
{
}

static void PRINT_APIENTRY printCopyTexImage2D(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
}

static void PRINT_APIENTRY printCopyTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width)
{
}

static void PRINT_APIENTRY printCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
}

static void PRINT_APIENTRY printCopyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
}

static GLint PRINT_APIENTRY printCreateContext(const char * dpyName, GLint visual, GLint shareCtx)
{
    return  scenegraph_spu_data.g_ret_count++;
}

static void PRINT_APIENTRY printCullFace(GLenum mode)
{
}

static void PRINT_APIENTRY printDeleteBuffersARB(GLsizei n, const GLuint * buffer)
{
}

static void PRINT_APIENTRY printDeleteFencesNV(GLsizei n, const GLuint * fences)
{
}

static void PRINT_APIENTRY printDeleteLists(GLuint list, GLsizei range)
{
}

static void PRINT_APIENTRY printDeleteProgramsARB(GLsizei n, const GLuint * programs)
{
}

static void PRINT_APIENTRY printDeleteQueriesARB(GLsizei n, const GLuint * ids)
{
}

static void PRINT_APIENTRY printDeleteTextures(GLsizei n, const GLuint * textures)
{
}

static void PRINT_APIENTRY printDepthFunc(GLenum func)
{
}

static void PRINT_APIENTRY printDepthMask(GLboolean flag)
{
}

static void PRINT_APIENTRY printDepthRange(GLclampd zNear, GLclampd zFar)
{
}

static void PRINT_APIENTRY printDestroyContext(GLint ctx)
{
}

static void PRINT_APIENTRY printDisable(GLenum cap)
{
    if (!scenegraph_spu_data.g_isReading)
        return;

    switch (cap)
    {

    case GL_POLYGON_STIPPLE:
    {
#ifdef ENABLE_MATERIAL
        scenegraph_spu_data.g_needToCreateNewGeode = true;
        scenegraph_spu_data.g_state->removeAttribute(osg::StateAttribute::Type::POLYGONSTIPPLE, 0);
#endif
        break;
    }
    case GL_BLEND:
    {
#ifdef ENABLE_MATERIAL
        scenegraph_spu_data.g_state->setMode(GL_BLEND, osg::StateAttribute::OFF);
        break;
#endif
    }

    }
}
static void PRINT_APIENTRY printDisableClientState(GLenum array)
{
}

static void PRINT_APIENTRY printDisableVertexAttribArrayARB(GLuint index)
{
}

static void PRINT_APIENTRY printDrawArrays(GLenum mode, GLint first, GLsizei count)
{
}

static void PRINT_APIENTRY printDrawBuffer(GLenum mode)
{
}

static void PRINT_APIENTRY printDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices)
{

    //no need to handle already handled in packSpu

}

static void PRINT_APIENTRY printDrawPixels(GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pixels)
{
}

static void PRINT_APIENTRY printDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid * indices)
{
}

static void PRINT_APIENTRY printEdgeFlag(GLboolean flag)
{
}

static void PRINT_APIENTRY printEdgeFlagPointer(GLsizei stride, const GLvoid * pointer)
{
}

static void PRINT_APIENTRY printEdgeFlagv(const GLboolean * flag)
{
}

static void PRINT_APIENTRY printEnable(GLenum cap)
{
    if (!scenegraph_spu_data.g_isReading)
        return;
    switch (cap)
    {

    case GL_POLYGON_STIPPLE:
    {
#ifdef ENABLE_MATERIAL
        scenegraph_spu_data.g_needToCreateNewGeode = true;
        osg::PolygonStipple* polygonStipple = new osg::PolygonStipple(); // Memory leak
        scenegraph_spu_data.g_state->setAttributeAndModes(polygonStipple, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
#endif
        break;
    }

    // XXX: - need to handle blend function
    case GL_BLEND:
    {
#ifdef ENABLE_MATERIAL
        scenegraph_spu_data.g_state->setMode(GL_BLEND, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
        break;
#endif
    }

    }
}

static void PRINT_APIENTRY printEnableClientState(GLenum array)
{
}

static void PRINT_APIENTRY printEnableVertexAttribArrayARB(GLuint index)
{
}

// 
static void PRINT_APIENTRY printEnd(void)
{
    if (scenegraph_spu_data.g_isReading)
    {
        scenegraph_spu_data.g_geom = new osg::Geometry();
        // create a g_geode and add it to the pat
        if (scenegraph_spu_data.g_vertexArray->size() > 0)
        {
            scenegraph_spu_data.g_geom->addPrimitiveSet(new osg::DrawArrays(scenegraph_spu_data.g_geometryMode, 0, scenegraph_spu_data.g_vertexArray->size()));
            scenegraph_spu_data.g_geom->setVertexArray(scenegraph_spu_data.g_vertexArray);
            scenegraph_spu_data.g_geom->setColorArray(scenegraph_spu_data.g_colorArray, osg::Array::BIND_PER_VERTEX);
            scenegraph_spu_data.g_geom->setNormalArray(scenegraph_spu_data.g_normalArray, osg::Array::BIND_PER_VERTEX);
            scenegraph_spu_data.g_geom->setSupportsDisplayList(true);
            scenegraph_spu_data.g_geom->setUseDisplayList(true);

#ifdef ENABLE_MATERIAL

            if (scenegraph_spu_data.g_state != NULL)
            {
                scenegraph_spu_data.g_geode->setStateSet(new osg::StateSet(*scenegraph_spu_data.g_state, osg::CopyOp::DEEP_COPY_ALL));
            }

            if (scenegraph_spu_data.g_material != NULL)
            {
                scenegraph_spu_data.g_geode->getOrCreateStateSet()->setAttributeAndModes(new osg::Material(*(scenegraph_spu_data.g_material.get()), osg::CopyOp::DEEP_COPY_ALL), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
            }

#endif
            scenegraph_spu_data.g_geode->addDrawable(scenegraph_spu_data.g_geom);
        }

    }

    if (scenegraph_spu_data.g_isDisplayList)
    {
        scenegraph_spu_data.g_geom = new osg::Geometry();
        // create a g_geode and add it to the pat
        if (scenegraph_spu_data.g_vertexArray->size() > 0)
        {

#ifdef ENABLE_MATERIAL

            scenegraph_spu_data.g_geom->addPrimitiveSet(new osg::DrawArrays(scenegraph_spu_data.g_geometryMode, 0, scenegraph_spu_data.g_vertexArray->size()));
            scenegraph_spu_data.g_geom->setVertexArray(scenegraph_spu_data.g_vertexArray);
            scenegraph_spu_data.g_geom->setColorArray(scenegraph_spu_data.g_colorArray, osg::Array::BIND_PER_VERTEX);
            scenegraph_spu_data.g_geom->setNormalArray(scenegraph_spu_data.g_normalArray, osg::Array::BIND_PER_VERTEX);
            if (scenegraph_spu_data.g_state != NULL)
            {
                scenegraph_spu_data.g_geode->setStateSet(new osg::StateSet(*scenegraph_spu_data.g_state, osg::CopyOp::DEEP_COPY_ALL));
            }

            if (scenegraph_spu_data.g_material != NULL)
            {
                scenegraph_spu_data.g_geode->getOrCreateStateSet()->setAttributeAndModes(new osg::Material(*(scenegraph_spu_data.g_material.get()), osg::CopyOp::DEEP_COPY_ALL), osg::StateAttribute::ON);
            }

#endif
            scenegraph_spu_data.g_geode->addDrawable(scenegraph_spu_data.g_geom);
        }


    }

    if (scenegraph_spu_data.g_geode)
    {
        if (!scenegraph_spu_data.curr_geode_name.empty())
        {

            scenegraph_spu_data.g_geode->setName(scenegraph_spu_data.curr_geode_name);
            scenegraph_spu_data.curr_geode_name.clear();
        }
    }
}

static void PRINT_APIENTRY printEndList(void)
{
    scenegraph_spu_data.g_isDisplayList = false;
}

static void PRINT_APIENTRY printEndQueryARB(GLenum target)
{
}

static void PRINT_APIENTRY printEvalCoord1d(GLdouble u)
{
}

static void PRINT_APIENTRY printEvalCoord1dv(const GLdouble * u)
{
}

static void PRINT_APIENTRY printEvalCoord1f(GLfloat u)
{
}

static void PRINT_APIENTRY printEvalCoord1fv(const GLfloat * u)
{
}

static void PRINT_APIENTRY printEvalCoord2d(GLdouble u, GLdouble v)
{
}

static void PRINT_APIENTRY printEvalCoord2dv(const GLdouble * u)
{
}

static void PRINT_APIENTRY printEvalCoord2f(GLfloat u, GLfloat v)
{
}

static void PRINT_APIENTRY printEvalCoord2fv(const GLfloat * u)
{
}

static void PRINT_APIENTRY printEvalMesh1(GLenum mode, GLint i1, GLint i2)
{
}

static void PRINT_APIENTRY printEvalMesh2(GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2)
{
}

static void PRINT_APIENTRY printEvalPoint1(GLint i)
{
}

static void PRINT_APIENTRY printEvalPoint2(GLint i, GLint j)
{
}

static void PRINT_APIENTRY printExecuteProgramNV(GLenum target, GLuint id, const GLfloat * params)
{
}

static void PRINT_APIENTRY printFeedbackBuffer(GLsizei size, GLenum type, GLfloat * buffer)
{
}

static void PRINT_APIENTRY printFinalCombinerInputNV(GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage)
{
}

static void PRINT_APIENTRY printFinish(void)
{
}

static void PRINT_APIENTRY printFinishFenceNV(GLuint fence)
{
}

static void PRINT_APIENTRY printFlushg_vertexArrayRangeNV(void)
{
}

static void PRINT_APIENTRY printFogCoordPointerEXT(GLenum type, GLsizei stride, const GLvoid * pointer)
{
}

static void PRINT_APIENTRY printFogCoorddEXT(GLdouble coord)
{
}

static void PRINT_APIENTRY printFogCoorddvEXT(const GLdouble * coord)
{
}

static void PRINT_APIENTRY printFogCoordfEXT(GLfloat coord)
{
}

static void PRINT_APIENTRY printFogCoordfvEXT(const GLfloat * coord)
{
}

static void PRINT_APIENTRY printFogf(GLenum pname, GLfloat param)
{
}

static void PRINT_APIENTRY printFogfv(GLenum pname, const GLfloat * params)
{
}

static void PRINT_APIENTRY printFogi(GLenum pname, GLint param)
{
}

static void PRINT_APIENTRY printFogiv(GLenum pname, const GLint * params)
{
}

static void PRINT_APIENTRY printFrontFace(GLenum mode)
{
}

static void PRINT_APIENTRY printFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
}

static void PRINT_APIENTRY printGenBuffersARB(GLsizei n, GLuint * buffer)
{
}

static void PRINT_APIENTRY printGenFencesNV(GLsizei n, GLuint * fences)
{
}

static GLuint PRINT_APIENTRY printGenLists(GLsizei range)
{
    return scenegraph_spu_data.g_ret_count++;
}

static void PRINT_APIENTRY printGenProgramsARB(GLsizei n, GLuint * programs)
{
}

static void PRINT_APIENTRY printGenProgramsNV(GLsizei n, GLuint * ids)
{
}

static void PRINT_APIENTRY printGenQueriesARB(GLsizei n, GLuint * ids)
{
}

static void PRINT_APIENTRY printGetBufferParameterivARB(GLenum target, GLenum pname, GLint * params)
{
}

static void PRINT_APIENTRY printGetBufferPointervARB(GLenum target, GLenum pname, GLvoid ** params)
{
}

static void PRINT_APIENTRY printGetBufferSubDataARB(GLenum target, GLintptrARB offset, GLsizeiptrARB size, void * data)
{
}

static void PRINT_APIENTRY printGetChromiumParametervCR(GLenum target, GLuint index, GLenum type, GLsizei count, GLvoid * values)
{
}

static void PRINT_APIENTRY printGetClipPlane(GLenum plane, GLdouble * equation)
{
}

static void PRINT_APIENTRY printGetCombinerInputParameterfvNV(GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLfloat * params)
{
}

static void PRINT_APIENTRY printGetCombinerInputParameterivNV(GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLint * params)
{
}

static void PRINT_APIENTRY printGetCombinerOutputParameterfvNV(GLenum stage, GLenum portion, GLenum pname, GLfloat * params)
{
}

static void PRINT_APIENTRY printGetCombinerOutputParameterivNV(GLenum stage, GLenum portion, GLenum pname, GLint * params)
{
}

static void PRINT_APIENTRY printGetCombinerStageParameterfvNV(GLenum stage, GLenum pname, GLfloat * params)
{
}

static void PRINT_APIENTRY printGetCompressedTexImageARB(GLenum target, GLint level, GLvoid * img)
{
}

static GLenum PRINT_APIENTRY printGetError(void)
{
    return false;
}

static void PRINT_APIENTRY printGetFenceivNV(GLuint fence, GLenum pname, GLint * params)
{
}

static void PRINT_APIENTRY printGetFinalCombinerInputParameterfvNV(GLenum variable, GLenum pname, GLfloat * params)
{
}

static void PRINT_APIENTRY printGetFinalCombinerInputParameterivNV(GLenum variable, GLenum pname, GLint * params)
{
}

static void PRINT_APIENTRY printLightiv(GLenum light, GLenum pname, const GLint *params)
{
}

static void PRINT_APIENTRY printLightfv(GLenum light, GLenum pname, const GLfloat *params)
{
#ifdef ENABLE_LIGHTS

    if (scenegraph_spu_data.g_light[light - GL_LIGHT0] == NULL)
    {
        scenegraph_spu_data.g_light[light - GL_LIGHT0] = new osg::LightSource();
    }

    switch (pname)
    {

    case GL_AMBIENT:
    {
        scenegraph_spu_data.g_light[light - GL_LIGHT0]->getLight()->setAmbient(osg::Vec4(params[0], params[1], params[2], params[3]));
        break;
    }
    case GL_DIFFUSE:
    {
        scenegraph_spu_data.g_light[light - GL_LIGHT0]->getLight()->setDiffuse(osg::Vec4(params[0], params[1], params[2], params[3]));
        break;
    }
    case GL_SPECULAR:
    {
        scenegraph_spu_data.g_light[light - GL_LIGHT0]->getLight()->setSpecular(osg::Vec4(params[0], params[1], params[2], params[3]));
        break;
    }
    case GL_POSITION:
    {
        osg::Matrix mat = osg::Matrix::translate(osg::Vec3(params[0], params[1], params[2]));
        osg::Matrix matFinal = mat * scenegraph_spu_data.g_CurrentMatrix;
        osg::Vec3 vertexPoint = matFinal.getTrans();

        scenegraph_spu_data.g_light[light - GL_LIGHT0]->getLight()->setPosition(osg::Vec4(vertexPoint[0], vertexPoint[1], vertexPoint[2], params[3]));
        break;
    }

    }

#endif
}

static void PRINT_APIENTRY printGetLightfv(GLenum light, GLenum pname, GLfloat * params)
{
}

static void PRINT_APIENTRY printGetLightiv(GLenum light, GLenum pname, GLint * params)
{
}

static void PRINT_APIENTRY printGetMapdv(GLenum target, GLenum query, GLdouble * v)
{
}

static void PRINT_APIENTRY printGetMapfv(GLenum target, GLenum query, GLfloat * v)
{
}

static void PRINT_APIENTRY printGetMapiv(GLenum target, GLenum query, GLint * v)
{
}

static void PRINT_APIENTRY printGetMaterialfv(GLenum face, GLenum pname, GLfloat * params)
{
}

static void PRINT_APIENTRY printGetMaterialiv(GLenum face, GLenum pname, GLint * params)
{
}

static void PRINT_APIENTRY printGetPixelMapfv(GLenum map, GLfloat * values)
{
}

static void PRINT_APIENTRY printGetPixelMapuiv(GLenum map, GLuint * values)
{
}

static void PRINT_APIENTRY printGetPixelMapusv(GLenum map, GLushort * values)
{
}

static void PRINT_APIENTRY printGetPointerv(GLenum pname, GLvoid ** params)
{
}

static void PRINT_APIENTRY printGetPolygonStipple(GLubyte * mask)
{
}

static void PRINT_APIENTRY printGetProgramEnvParameterdvARB(GLenum target, GLuint index, GLdouble * params)
{
}

static void PRINT_APIENTRY printGetProgramEnvParameterfvARB(GLenum target, GLuint index, GLfloat * params)
{
}

static void PRINT_APIENTRY printGetProgramLocalParameterdvARB(GLenum target, GLuint index, GLdouble * params)
{
}

static void PRINT_APIENTRY printGetProgramLocalParameterfvARB(GLenum target, GLuint index, GLfloat * params)
{
}

static void PRINT_APIENTRY printGetProgramNamedParameterdvNV(GLuint id, GLsizei len, const GLubyte * name, GLdouble * params)
{
}

static void PRINT_APIENTRY printGetProgramNamedParameterfvNV(GLuint id, GLsizei len, const GLubyte * name, GLfloat * params)
{
}

static void PRINT_APIENTRY printGetProgramParameterdvNV(GLenum target, GLuint index, GLenum pname, GLdouble * params)
{
}

static void PRINT_APIENTRY printGetProgramParameterfvNV(GLenum target, GLuint index, GLenum pname, GLfloat * params)
{
}

static void PRINT_APIENTRY printGetProgramStringARB(GLenum target, GLenum pname, GLvoid * string)
{
}

static void PRINT_APIENTRY printGetProgramStringNV(GLuint id, GLenum pname, GLubyte * program)
{
}

static void PRINT_APIENTRY printGetProgramivARB(GLenum target, GLenum pname, GLint * params)
{
}

static void PRINT_APIENTRY printGetProgramivNV(GLuint id, GLenum pname, GLint * params)
{
}

static void PRINT_APIENTRY printGetQueryObjectivARB(GLuint id, GLenum pname, GLint * params)
{
}

static void PRINT_APIENTRY printGetQueryObjectuivARB(GLuint id, GLenum pname, GLuint * params)
{
}

static void PRINT_APIENTRY printGetQueryivARB(GLenum target, GLenum pname, GLint * params)
{
}

static const GLubyte * PRINT_APIENTRY printGetString(GLenum name)
{
    return NULL;
}

static void PRINT_APIENTRY printGetTexEnvfv(GLenum target, GLenum pname, GLfloat * params)
{
}

static void PRINT_APIENTRY printGetTexEnviv(GLenum target, GLenum pname, GLint * params)
{
}

static void PRINT_APIENTRY printGetTexGendv(GLenum coord, GLenum pname, GLdouble * params)
{
}

static void PRINT_APIENTRY printGetTexGenfv(GLenum coord, GLenum pname, GLfloat * params)
{
}

static void PRINT_APIENTRY printGetTexGeniv(GLenum coord, GLenum pname, GLint * params)
{
}

static void PRINT_APIENTRY printGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, GLvoid * pixels)
{
}

static void PRINT_APIENTRY printGetTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat * params)
{
}

static void PRINT_APIENTRY printGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint * params)
{
}

static void PRINT_APIENTRY printGetTexParameterfv(GLenum target, GLenum pname, GLfloat * params)
{
}

static void PRINT_APIENTRY printGetTexParameteriv(GLenum target, GLenum pname, GLint * params)
{
}

static void PRINT_APIENTRY printGetTrackMatrixivNV(GLenum target, GLuint address, GLenum pname, GLint * params)
{
}

static void PRINT_APIENTRY printGetVertexAttribPointervARB(GLuint index, GLenum pname, GLvoid ** pointer)
{
}

static void PRINT_APIENTRY printGetVertexAttribPointervNV(GLuint index, GLenum pname, GLvoid ** pointer)
{
}

static void PRINT_APIENTRY printGetVertexAttribdvARB(GLuint index, GLenum pname, GLdouble * params)
{
}

static void PRINT_APIENTRY printGetVertexAttribdvNV(GLuint index, GLenum pname, GLdouble * params)
{
}

static void PRINT_APIENTRY printGetVertexAttribfvARB(GLuint index, GLenum pname, GLfloat * params)
{
}

static void PRINT_APIENTRY printGetVertexAttribfvNV(GLuint index, GLenum pname, GLfloat * params)
{
}

static void PRINT_APIENTRY printGetVertexAttribivARB(GLuint index, GLenum pname, GLint * params)
{
}

static void PRINT_APIENTRY printGetVertexAttribivNV(GLuint index, GLenum pname, GLint * params)
{
}

static void PRINT_APIENTRY printHint(GLenum target, GLenum mode)
{
}

static void PRINT_APIENTRY printIndexMask(GLuint mask)
{
}

static void PRINT_APIENTRY printIndexPointer(GLenum type, GLsizei stride, const GLvoid * pointer)
{
}

static void PRINT_APIENTRY printIndexd(GLdouble c)
{
}

static void PRINT_APIENTRY printIndexdv(const GLdouble * c)
{
}

static void PRINT_APIENTRY printIndexf(GLfloat c)
{
}

static void PRINT_APIENTRY printIndexfv(const GLfloat * c)
{
}

static void PRINT_APIENTRY printIndexi(GLint c)
{
}

static void PRINT_APIENTRY printIndexiv(const GLint * c)
{
}

static void PRINT_APIENTRY printIndexs(GLshort c)
{
}

static void PRINT_APIENTRY printIndexsv(const GLshort * c)
{
}

static void PRINT_APIENTRY printIndexub(GLubyte c)
{
}

static void PRINT_APIENTRY printIndexubv(const GLubyte * c)
{
}

static void PRINT_APIENTRY printInitNames(void)
{
}

static void PRINT_APIENTRY printInterleavedArrays(GLenum format, GLsizei stride, const GLvoid * pointer)
{
}

static GLboolean PRINT_APIENTRY printIsBufferARB(GLuint buffer)
{
    return false;
}

static GLboolean PRINT_APIENTRY printIsEnabled(GLenum cap)
{
    return false;
}

static GLboolean PRINT_APIENTRY printIsFenceNV(GLuint fence)
{
    return false;
}

static GLboolean PRINT_APIENTRY printIsList(GLuint list)
{
    return false;
}

static GLboolean PRINT_APIENTRY printIsProgramARB(GLuint program)
{
    return false;
}

static GLboolean PRINT_APIENTRY printIsQueryARB(GLuint id)
{
    return false;
}

static GLboolean PRINT_APIENTRY printIsTexture(GLuint texture)
{
    return false;
}

static void PRINT_APIENTRY printLightModelf(GLenum pname, GLfloat param)
{

}

static void PRINT_APIENTRY printLightModelfv(GLenum pname, const GLfloat * params)
{
#ifdef ENABLE_LIGHTS
    {
        switch (pname)
        {

        case GL_LIGHT_MODEL_AMBIENT:
        {
            osg::ref_ptr<osg::LightModel> lm = new osg::LightModel();
            lm->setAmbientIntensity(osg::Vec4d(params[0], params[0], params[0], params[0]));
            scenegraph_spu_data.g_state->setAttribute(lm);
            break;
        }

        }
    }
#endif
}

static void PRINT_APIENTRY printLightModeli(GLenum pname, GLint param)
{
}

static void PRINT_APIENTRY printLightModeliv(GLenum pname, const GLint * params)
{
}

static void PRINT_APIENTRY printLightf(GLenum light, GLenum pname, GLfloat param)
{
    if (!scenegraph_spu_data.g_isReading)
    {
        return;
    }

#ifdef ENABLE_LIGHTS
    if (scenegraph_spu_data.g_light[light - GL_LIGHT0] == NULL){
        scenegraph_spu_data.g_light[light - GL_LIGHT0] = new osg::LightSource();
    }

    switch (pname)
    {
    case GL_SPOT_EXPONENT:
    {
        scenegraph_spu_data.g_light[light - GL_LIGHT0]->getLight()->setSpotExponent(param);
        break;
    }
    case GL_SPOT_CUTOFF:
    {
        scenegraph_spu_data.g_light[light - GL_LIGHT0]->getLight()->setSpotCutoff(param);
        break;
    }

    }
#endif
}

static void PRINT_APIENTRY printLighti(GLenum light, GLenum pname, GLint param)
{
}

static void PRINT_APIENTRY printLineStipple(GLint factor, GLushort pattern)
{
}

static void PRINT_APIENTRY printLineWidth(GLfloat width)
{
}

static void PRINT_APIENTRY printListBase(GLuint base)
{
}

static void PRINT_APIENTRY printLoadIdentity(void)
{
    if (scenegraph_spu_data.g_currentMatrixMode != GL_MODELVIEW)
        return;

    scenegraph_spu_data.g_CurrentMatrix.makeIdentity();
}

static void PRINT_APIENTRY printLoadMatrixd(const GLdouble * m)
{
    if (scenegraph_spu_data.g_currentMatrixMode != GL_MODELVIEW)
        return;

#ifdef GEODE_WITH_LM
    scenegraph_spu_data.g_needToCreateNewGeode = true;
#endif
    scenegraph_spu_data.g_CurrentMatrix.set(m);

}

static void PRINT_APIENTRY printLoadMatrixf(const GLfloat * m)
{
    if (scenegraph_spu_data.g_currentMatrixMode != GL_MODELVIEW)
        return;
#ifdef GEODE_WITH_LM
    scenegraph_spu_data.g_needToCreateNewGeode = true;
#endif
    scenegraph_spu_data.g_CurrentMatrix.set(m);
}

static void PRINT_APIENTRY printPushMatrix(void)
{
    if (scenegraph_spu_data.g_currentMatrixMode != GL_MODELVIEW)
        return;
    //pushing current matrix in stack
    scenegraph_spu_data.g_matrix_stack.push_back(scenegraph_spu_data.g_CurrentMatrix);
#ifdef GEODE_WITH_LM
    scenegraph_spu_data.g_needToCreateNewGeode = true;
#endif
}

static void PRINT_APIENTRY printLoadName(GLuint name)
{
}

static void PRINT_APIENTRY printLoadProgramNV(GLenum target, GLuint id, GLsizei len, const GLubyte * program)
{
}

static void PRINT_APIENTRY printLoadTransposeMatrixdARB(const GLdouble * m)
{
}

static void PRINT_APIENTRY printLoadTransposeMatrixfARB(const GLfloat * m)
{
}

static void PRINT_APIENTRY printLogicOp(GLenum opcode)
{
}

static void PRINT_APIENTRY printMap1d(GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble * points)
{
}

static void PRINT_APIENTRY printMap1f(GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat * points)
{
}

static void PRINT_APIENTRY printMap2d(GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble * points)
{
}

static void PRINT_APIENTRY printMap2f(GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat * points)
{
}

static void * PRINT_APIENTRY printMapBufferARB(GLenum target, GLenum access)
{
    return NULL;
}

static void PRINT_APIENTRY printMapGrid1d(GLint un, GLdouble u1, GLdouble u2)
{
}

static void PRINT_APIENTRY printMapGrid1f(GLint un, GLfloat u1, GLfloat u2)
{
}

static void PRINT_APIENTRY printMapGrid2d(GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2)
{
}

static void PRINT_APIENTRY printMapGrid2f(GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2)
{
}

static void PRINT_APIENTRY printMaterialf(GLenum face, GLenum pname, GLfloat param)
{

    if (!scenegraph_spu_data.g_isReading)
    {
        return;
    }

    osg::Material::Face osgface = osg::Material::Face::FRONT;

    switch (face){
    case GL_BACK:
        osgface = osg::Material::Face::BACK;
        break;
    case GL_FRONT_AND_BACK:
        osgface = osg::Material::Face::FRONT_AND_BACK;
        break;
    }

#ifdef ENABLE_MATERIAL

    if (scenegraph_spu_data.g_material == NULL)
    {
        scenegraph_spu_data.g_material = new osg::Material();
    }

    switch (pname)
    {
    case GL_SHININESS:

        scenegraph_spu_data.g_needToCreateNewGeode = true;
        scenegraph_spu_data.g_material->setShininess(osgface, param);
        break;
    }

#endif

}

static void PRINT_APIENTRY printMateriali(GLenum face, GLenum pname, GLint param)
{
}

static void PRINT_APIENTRY printMatrixMode(GLenum mode)
{
    scenegraph_spu_data.g_currentMatrixMode = mode;
}

static void PRINT_APIENTRY printMultTransposeMatrixdARB(const GLdouble * m)
{
}

static void PRINT_APIENTRY printMultMatrixf(const GLfloat * m)
{
    if (scenegraph_spu_data.g_currentMatrixMode != GL_MODELVIEW)
        return;

    osg::Matrix mat = osg::Matrix();
    mat.set(m);
    scenegraph_spu_data.g_CurrentMatrix = mat *  scenegraph_spu_data.g_CurrentMatrix;
}

static void PRINT_APIENTRY printMultMatrixd(const GLdouble * m)
{
    if (scenegraph_spu_data.g_currentMatrixMode != GL_MODELVIEW)
        return;

    osg::Matrix mat = osg::Matrix();
    mat.set(m);
    scenegraph_spu_data.g_CurrentMatrix = mat *  scenegraph_spu_data.g_CurrentMatrix;
}

static void PRINT_APIENTRY printMultTransposeMatrixfARB(const GLfloat * m)
{
}

static void PRINT_APIENTRY printMultiDrawArraysEXT(GLenum mode, GLint * first, GLsizei * count, GLsizei primcount)
{
}

static void PRINT_APIENTRY printMultiDrawElementsEXT(GLenum mode, const GLsizei * count, GLenum type, const GLvoid ** indices, GLsizei primcount)
{
}

static void PRINT_APIENTRY printMultiTexCoord1dARB(GLenum texture, GLdouble s)
{
}

static void PRINT_APIENTRY printMultiTexCoord1dvARB(GLenum texture, const GLdouble * t)
{
}

static void PRINT_APIENTRY printMultiTexCoord1fARB(GLenum texture, GLfloat s)
{
}

static void PRINT_APIENTRY printMultiTexCoord1fvARB(GLenum texture, const GLfloat * t)
{
}

static void PRINT_APIENTRY printMultiTexCoord1iARB(GLenum texture, GLint s)
{
}

static void PRINT_APIENTRY printMultiTexCoord1ivARB(GLenum texture, const GLint * t)
{
}

static void PRINT_APIENTRY printMultiTexCoord1sARB(GLenum texture, GLshort s)
{
}

static void PRINT_APIENTRY printMultiTexCoord1svARB(GLenum texture, const GLshort * t)
{
}

static void PRINT_APIENTRY printMultiTexCoord2dARB(GLenum texture, GLdouble s, GLdouble t)
{
}

static void PRINT_APIENTRY printMultiTexCoord2dvARB(GLenum texture, const GLdouble * t)
{
}

static void PRINT_APIENTRY printMultiTexCoord2fARB(GLenum texture, GLfloat s, GLfloat t)
{
}

static void PRINT_APIENTRY printMultiTexCoord2fvARB(GLenum texture, const GLfloat * t)
{
}

static void PRINT_APIENTRY printMultiTexCoord2iARB(GLenum texture, GLint s, GLint t)
{
}

static void PRINT_APIENTRY printMultiTexCoord2ivARB(GLenum texture, const GLint * t)
{
}

static void PRINT_APIENTRY printMultiTexCoord2sARB(GLenum texture, GLshort s, GLshort t)
{
}

static void PRINT_APIENTRY printMultiTexCoord2svARB(GLenum texture, const GLshort * t)
{
}

static void PRINT_APIENTRY printMultiTexCoord3dARB(GLenum texture, GLdouble s, GLdouble t, GLdouble r)
{
}

static void PRINT_APIENTRY printMultiTexCoord3dvARB(GLenum texture, const GLdouble * t)
{
}

static void PRINT_APIENTRY printMultiTexCoord3fARB(GLenum texture, GLfloat s, GLfloat t, GLfloat r)
{
}

static void PRINT_APIENTRY printMultiTexCoord3fvARB(GLenum texture, const GLfloat * t)
{
}

static void PRINT_APIENTRY printMultiTexCoord3iARB(GLenum texture, GLint s, GLint t, GLint r)
{
}

static void PRINT_APIENTRY printMultiTexCoord3ivARB(GLenum texture, const GLint * t)
{
}

static void PRINT_APIENTRY printMultiTexCoord3sARB(GLenum texture, GLshort s, GLshort t, GLshort r)
{
}

static void PRINT_APIENTRY printMultiTexCoord3svARB(GLenum texture, const GLshort * t)
{
}

static void PRINT_APIENTRY printMultiTexCoord4dARB(GLenum texture, GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
}

static void PRINT_APIENTRY printMultiTexCoord4dvARB(GLenum texture, const GLdouble * t)
{
}

static void PRINT_APIENTRY printMultiTexCoord4fARB(GLenum texture, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
}

static void PRINT_APIENTRY printMultiTexCoord4fvARB(GLenum texture, const GLfloat * t)
{
}

static void PRINT_APIENTRY printMultiTexCoord4iARB(GLenum texture, GLint s, GLint t, GLint r, GLint q)
{
}

static void PRINT_APIENTRY printMultiTexCoord4ivARB(GLenum texture, const GLint * t)
{
}

static void PRINT_APIENTRY printMultiTexCoord4sARB(GLenum texture, GLshort s, GLshort t, GLshort r, GLshort q)
{
}

static void PRINT_APIENTRY printMultiTexCoord4svARB(GLenum texture, const GLshort * t)
{
}

static void PRINT_APIENTRY printNewList(GLuint list, GLenum mode)
{
    scenegraph_spu_data.g_isDisplayList = true;
    osg::PositionAttitudeTransform* pat = new osg::PositionAttitudeTransform();
    scenegraph_spu_data.g_PatArrayDisplayList.push_back(pat);
}

static void PRINT_APIENTRY printNormal3d(GLdouble nx, GLdouble ny, GLdouble nz)
{
    scenegraph_spu_data.g_CurrentNormal = osg::Vec3(nx, ny, nz);
}

static void PRINT_APIENTRY printNormal3b(GLbyte nx, GLbyte ny, GLbyte nz)
{
    GLdouble ndx = ((2 * (GLdouble)nx + 1) / GLdouble(BYTEMINMAXDIFF));
    GLdouble ndy = ((2 * (GLdouble)ny + 1) / GLdouble(BYTEMINMAXDIFF));
    GLdouble ndz = ((2 * (GLdouble)nz + 1) / GLdouble(BYTEMINMAXDIFF));

    printNormal3d(ndx, ndy, ndz);
}

static void PRINT_APIENTRY printNormal3bv(const GLbyte * v)
{
    printNormal3b(v[0], v[1], v[2]);
}
static void PRINT_APIENTRY printNormal3dv(const GLdouble * v)
{
    scenegraph_spu_data.g_CurrentNormal = osg::Vec3(v[0], v[1], v[2]);
}

static void PRINT_APIENTRY printNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
    scenegraph_spu_data.g_CurrentNormal = osg::Vec3(nx, ny, nz);
}

static void PRINT_APIENTRY printNormal3fv(const GLfloat * v)
{
    scenegraph_spu_data.g_CurrentNormal = osg::Vec3(v[0], v[1], v[2]);
}

static void PRINT_APIENTRY printNormal3i(GLint nx, GLint ny, GLint nz)
{
    GLdouble ndx = ((2 * (GLdouble)nx + 1) / GLdouble(INTMAXMINDIFF));
    GLdouble ndy = ((2 * (GLdouble)ny + 1) / GLdouble(INTMAXMINDIFF));
    GLdouble ndz = ((2 * (GLdouble)nz + 1) / GLdouble(INTMAXMINDIFF));

    printNormal3d(ndx, ndy, ndz);
}

static void PRINT_APIENTRY printNormal3iv(const GLint * v)
{
    printNormal3i(v[0], v[1], v[2]);
}

static void PRINT_APIENTRY printNormal3s(GLshort nx, GLshort ny, GLshort nz)
{
    GLdouble ndx = ((2 * (GLdouble)nx + 1) / GLdouble(SHORTMAXMINDIFF));
    GLdouble ndy = ((2 * (GLdouble)ny + 1) / GLdouble(SHORTMAXMINDIFF));
    GLdouble ndz = ((2 * (GLdouble)nz + 1) / GLdouble(SHORTMAXMINDIFF));

    printNormal3d(ndx, ndy, ndz);
}

static void PRINT_APIENTRY printNormal3sv(const GLshort * v)
{
    printNormal3s(v[0], v[1], v[2]);
}

static void PRINT_APIENTRY printNormalPointer(GLenum type, GLsizei stride, const GLvoid * pointer)
{

}

static void PRINT_APIENTRY printOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{

}

static void PRINT_APIENTRY printPassThrough(GLfloat token)
{
}

static void PRINT_APIENTRY printPixelMapfv(GLenum map, GLsizei mapsize, const GLfloat * values)
{
}

static void PRINT_APIENTRY printPixelMapuiv(GLenum map, GLsizei mapsize, const GLuint * values)
{
}

static void PRINT_APIENTRY printPixelMapusv(GLenum map, GLsizei mapsize, const GLushort * values)
{
}

static void PRINT_APIENTRY printPixelStoref(GLenum pname, GLfloat param)
{
}

static void PRINT_APIENTRY printPixelStorei(GLenum pname, GLint param)
{
}

static void PRINT_APIENTRY printPixelTransferf(GLenum pname, GLfloat param)
{
}

static void PRINT_APIENTRY printPixelTransferi(GLenum pname, GLint param)
{
}

static void PRINT_APIENTRY printPixelZoom(GLfloat xfactor, GLfloat yfactor)
{
}

static void PRINT_APIENTRY printPointParameterfARB(GLenum pname, GLfloat param)
{
}

static void PRINT_APIENTRY printPointParameterfvARB(GLenum pname, const GLfloat * params)
{
}

static void PRINT_APIENTRY printPointParameteri(GLenum pname, GLint param)
{
}

static void PRINT_APIENTRY printPointParameteriv(GLenum pname, const GLint * param)
{
}

static void PRINT_APIENTRY printPointSize(GLfloat size)
{
}

static void PRINT_APIENTRY printPolygonMode(GLenum face, GLenum mode)
{
}

static void PRINT_APIENTRY printPolygonOffset(GLfloat factor, GLfloat units)
{
}

static void PRINT_APIENTRY printPolygonStipple(const GLubyte * mask)
{
}

static void PRINT_APIENTRY printPopAttrib(void)
{
}

static void PRINT_APIENTRY printPopClientAttrib(void)
{
}

static void PRINT_APIENTRY printPopMatrix(void)
{
    if (scenegraph_spu_data.g_currentMatrixMode != GL_MODELVIEW)
        return;

    osg::Matrix top_mat = scenegraph_spu_data.g_matrix_stack.back();
    scenegraph_spu_data.g_matrix_stack.pop_back();
    scenegraph_spu_data.g_CurrentMatrix = top_mat;
}

static void PRINT_APIENTRY printPopName(void)
{
}

static void PRINT_APIENTRY printPrioritizeTextures(GLsizei n, const GLuint * textures, const GLclampf * priorities)
{
}

static void PRINT_APIENTRY printProgramEnvParameter4dARB(GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
}

static void PRINT_APIENTRY printProgramEnvParameter4dvARB(GLenum target, GLuint index, const GLdouble * params)
{
}

static void PRINT_APIENTRY printProgramEnvParameter4fARB(GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
}

static void PRINT_APIENTRY printProgramEnvParameter4fvARB(GLenum target, GLuint index, const GLfloat * params)
{
}

static void PRINT_APIENTRY printProgramLocalParameter4dARB(GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
}

static void PRINT_APIENTRY printProgramLocalParameter4dvARB(GLenum target, GLuint index, const GLdouble * params)
{
}

static void PRINT_APIENTRY printProgramLocalParameter4fARB(GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
}

static void PRINT_APIENTRY printProgramLocalParameter4fvARB(GLenum target, GLuint index, const GLfloat * params)
{
}

static void PRINT_APIENTRY printProgramNamedParameter4dNV(GLuint id, GLsizei len, const GLubyte * name, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
}

static void PRINT_APIENTRY printProgramNamedParameter4dvNV(GLuint id, GLsizei len, const GLubyte * name, const GLdouble * v)
{
}

static void PRINT_APIENTRY printProgramNamedParameter4fNV(GLuint id, GLsizei len, const GLubyte * name, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
}

static void PRINT_APIENTRY printProgramNamedParameter4fvNV(GLuint id, GLsizei len, const GLubyte * name, const GLfloat * v)
{
}

static void PRINT_APIENTRY printProgramParameter4dNV(GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
}

static void PRINT_APIENTRY printProgramParameter4dvNV(GLenum target, GLuint index, const GLdouble * params)
{
}

static void PRINT_APIENTRY printProgramParameter4fNV(GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
}

static void PRINT_APIENTRY printProgramParameter4fvNV(GLenum target, GLuint index, const GLfloat * params)
{
}

static void PRINT_APIENTRY printProgramParameters4dvNV(GLenum target, GLuint index, GLuint num, const GLdouble * params)
{
}

static void PRINT_APIENTRY printProgramParameters4fvNV(GLenum target, GLuint index, GLuint num, const GLfloat * params)
{
}

static void PRINT_APIENTRY printProgramStringARB(GLenum target, GLenum format, GLsizei len, const GLvoid * string)
{
}

static void PRINT_APIENTRY printPushAttrib(GLbitfield mask)
{
}

static void PRINT_APIENTRY printPushClientAttrib(GLbitfield mask)
{
}

static void PRINT_APIENTRY printPushName(GLuint name)
{
}

static void PRINT_APIENTRY printRasterPos2d(GLdouble x, GLdouble y)
{
}

static void PRINT_APIENTRY printRasterPos2dv(const GLdouble * v)
{
}

static void PRINT_APIENTRY printRasterPos2f(GLfloat x, GLfloat y)
{
}

static void PRINT_APIENTRY printRasterPos2fv(const GLfloat * v)
{
}

static void PRINT_APIENTRY printRasterPos2i(GLint x, GLint y)
{
}

static void PRINT_APIENTRY printRasterPos2iv(const GLint * v)
{
}

static void PRINT_APIENTRY printRasterPos2s(GLshort x, GLshort y)
{
}

static void PRINT_APIENTRY printRasterPos2sv(const GLshort * v)
{
}

static void PRINT_APIENTRY printRasterPos3d(GLdouble x, GLdouble y, GLdouble z)
{
}

static void PRINT_APIENTRY printRasterPos3dv(const GLdouble * v)
{
}

static void PRINT_APIENTRY printRasterPos3f(GLfloat x, GLfloat y, GLfloat z)
{
}

static void PRINT_APIENTRY printRasterPos3fv(const GLfloat * v)
{
}

static void PRINT_APIENTRY printRasterPos3i(GLint x, GLint y, GLint z)
{
}

static void PRINT_APIENTRY printRasterPos3iv(const GLint * v)
{
}

static void PRINT_APIENTRY printRasterPos3s(GLshort x, GLshort y, GLshort z)
{
}

static void PRINT_APIENTRY printRasterPos3sv(const GLshort * v)
{
}

static void PRINT_APIENTRY printRasterPos4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
}

static void PRINT_APIENTRY printRasterPos4dv(const GLdouble * v)
{
}

static void PRINT_APIENTRY printRasterPos4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
}

static void PRINT_APIENTRY printRasterPos4fv(const GLfloat * v)
{
}

static void PRINT_APIENTRY printRasterPos4i(GLint x, GLint y, GLint z, GLint w)
{
}

static void PRINT_APIENTRY printRasterPos4iv(const GLint * v)
{
}

static void PRINT_APIENTRY printRasterPos4s(GLshort x, GLshort y, GLshort z, GLshort w)
{
}

static void PRINT_APIENTRY printRasterPos4sv(const GLshort * v)
{
}

static void PRINT_APIENTRY printReadBuffer(GLenum mode)
{
}

static void PRINT_APIENTRY printReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid * pixels)
{
}

static void PRINT_APIENTRY printRectd(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
{
}

static void PRINT_APIENTRY printRectdv(const GLdouble * v1, const GLdouble * v2)
{
}

static void PRINT_APIENTRY printRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
}

static void PRINT_APIENTRY printRectfv(const GLfloat * v1, const GLfloat * v2)
{
}

static void PRINT_APIENTRY printRecti(GLint x1, GLint y1, GLint x2, GLint y2)
{
}

static void PRINT_APIENTRY printRectiv(const GLint * v1, const GLint * v2)
{
}

static void PRINT_APIENTRY printRects(GLshort x1, GLshort y1, GLshort x2, GLshort y2)
{
}

static void PRINT_APIENTRY printRectsv(const GLshort * v1, const GLshort * v2)
{
}

static GLint PRINT_APIENTRY printRenderMode(GLenum mode)
{
    return scenegraph_spu_data.g_ret_count++;
}

static void PRINT_APIENTRY printRequestResidentProgramsNV(GLsizei n, const GLuint * ids)
{
}

static void PRINT_APIENTRY printRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z)
{
    if (scenegraph_spu_data.g_currentMatrixMode != GL_MODELVIEW)
        return;
    osg::Matrix mat;
    double angle_in_radian = (osg::PI * angle) / 180.0;
    mat.makeRotate(angle_in_radian, x, y, z);
    scenegraph_spu_data.g_CurrentMatrix = mat *  scenegraph_spu_data.g_CurrentMatrix;
}

static void PRINT_APIENTRY printRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    printRotated(angle, x, y, z);
}

static void PRINT_APIENTRY printSampleCoverageARB(GLclampf value, GLboolean invert)
{
}

static void PRINT_APIENTRY printScaled(GLdouble x, GLdouble y, GLdouble z)
{
    if (scenegraph_spu_data.g_currentMatrixMode != GL_MODELVIEW)
        return;
    osg::Matrix mat = osg::Matrix();
    mat.makeScale(osg::Vec3(x, y, z));
    scenegraph_spu_data.g_CurrentMatrix = mat *  scenegraph_spu_data.g_CurrentMatrix;
}

static void PRINT_APIENTRY printScalef(GLfloat x, GLfloat y, GLfloat z)
{
    printScaled(x, y, z);
}

static void PRINT_APIENTRY printScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
}

static void PRINT_APIENTRY printSecondaryColor3bEXT(GLbyte red, GLbyte green, GLbyte blue)
{
}

static void PRINT_APIENTRY printSecondaryColor3bvEXT(const GLbyte * v)
{
}

static void PRINT_APIENTRY printSecondaryColor3dEXT(GLdouble red, GLdouble green, GLdouble blue)
{
}

static void PRINT_APIENTRY printSecondaryColor3dvEXT(const GLdouble * v)
{
}

static void PRINT_APIENTRY printSecondaryColor3fEXT(GLfloat red, GLfloat green, GLfloat blue)
{
}

static void PRINT_APIENTRY printSecondaryColor3fvEXT(const GLfloat * v)
{
}

static void PRINT_APIENTRY printSecondaryColor3iEXT(GLint red, GLint green, GLint blue)
{
}

static void PRINT_APIENTRY printSecondaryColor3ivEXT(const GLint * v)
{
}

static void PRINT_APIENTRY printSecondaryColor3sEXT(GLshort red, GLshort green, GLshort blue)
{
}

static void PRINT_APIENTRY printSecondaryColor3svEXT(const GLshort * v)
{
}

static void PRINT_APIENTRY printSecondaryColor3ubEXT(GLubyte red, GLubyte green, GLubyte blue)
{
}

static void PRINT_APIENTRY printSecondaryColor3ubvEXT(const GLubyte * v)
{
}

static void PRINT_APIENTRY printSecondaryColor3uiEXT(GLuint red, GLuint green, GLuint blue)
{
}

static void PRINT_APIENTRY printSecondaryColor3uivEXT(const GLuint * v)
{
}

static void PRINT_APIENTRY printSecondaryColor3usEXT(GLushort red, GLushort green, GLushort blue)
{
}

static void PRINT_APIENTRY printSecondaryColor3usvEXT(const GLushort * v)
{
}

static void PRINT_APIENTRY printSecondaryColorPointerEXT(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
}

static void PRINT_APIENTRY printSelectBuffer(GLsizei size, GLuint * buffer)
{
}

static void PRINT_APIENTRY printSemaphoreCreateCR(GLuint name, GLuint count)
{
}

static void PRINT_APIENTRY printSemaphoreDestroyCR(GLuint name)
{
}

static void PRINT_APIENTRY printSemaphorePCR(GLuint name)
{
}

static void PRINT_APIENTRY printSemaphoreVCR(GLuint name)
{
}

static void PRINT_APIENTRY printSetFenceNV(GLuint fence, GLenum condition)
{
}

static void PRINT_APIENTRY printShadeModel(GLenum mode)
{
}

static void PRINT_APIENTRY printStencilFunc(GLenum func, GLint ref, GLuint mask)
{
}

static void PRINT_APIENTRY printStencilMask(GLuint mask)
{
}

static void PRINT_APIENTRY printStencilOp(GLenum fail, GLenum zfail, GLenum zpass)
{
}

static void PRINT_APIENTRY printSwapBuffers(GLint window, GLint flags)
{

    if (scenegraph_spu_data.g_isReading)
    {
        scenegraph_spu_data.g_startReading = false;

        if (scenegraph_spu_data.g_hasTouchedBegin == false)
        {
            scenegraph_spu_data.g_shouldStartReading = true;
        }

        if (scenegraph_spu_data.g_shouldStartReading == false)
        {
            scenegraph_spu_data.g_isReading = false;

#ifdef ENABLE_LIGHTS
            for (int i = GL_LIGHT0; i <= GL_LIGHT7; ++i)
            {
                if (scenegraph_spu_data.g_light[i - GL_LIGHT0] != NULL){
                    scenegraph_spu_data.g_spuRootGroup->addChild(scenegraph_spu_data.g_light[i - GL_LIGHT0]);
                }
            }
#endif
            // This can be optimized further if geode are mearged
            osgUtil::Optimizer optimizer;
            optimizer.optimize(scenegraph_spu_data.g_spuRootGroup);

            scenegraph_spu_data.g_pt2Func(scenegraph_spu_data.g_context, scenegraph_spu_data.g_spuRootGroup.get());
        }
    }

    if (scenegraph_spu_data.g_shouldStartReading)
    {
        scenegraph_spu_data.g_shouldStartReading = false;
        scenegraph_spu_data.g_startReading = true;
        scenegraph_spu_data.g_isReading = true;
        scenegraph_spu_data.g_hasTouchedBegin = false;
        scenegraph_spu_data.g_CurrentMatrix.makeIdentity();
    }

    if (scenegraph_spu_data.g_startReading)
    {
        scenegraph_spu_data.g_spuRootGroup = new osg::Group;

#ifdef ENABLE_MATERIAL
        scenegraph_spu_data.g_material = new osg::Material();
#endif
        last_color[0] = -1;
        last_color[1] = -1;
        last_color[2] = -1;
        last_color[3] = -1;
        scenegraph_spu_data.g_needToCreateNewGeode = false;
        scenegraph_spu_data.g_startReading = false;
    }
}

/*glFlush command is called as replacement of WglSwapBuffers in single Buffered rendering.since solid works draws directly to Front Buffer while software opengl option is enabled in solid works's
performance setting that is single buffered rendering.hence we handle here printFlush.*/
static void PRINT_APIENTRY printFlush(void)
{
    //need to be handled in separate spu for solid works
}
static void PRINT_APIENTRY printMakeCurrent(GLint window, GLint nativeWindow, GLint ctx)
{

}

static GLboolean PRINT_APIENTRY printTestFenceNV(GLuint fence)
{
    return false;
}

static void PRINT_APIENTRY printTexCoord1d(GLdouble s)
{
}

static void PRINT_APIENTRY printTexCoord1dv(const GLdouble * v)
{
}

static void PRINT_APIENTRY printTexCoord1f(GLfloat s)
{
}

static void PRINT_APIENTRY printTexCoord1fv(const GLfloat * v)
{
}

static void PRINT_APIENTRY printTexCoord1i(GLint s)
{
}

static void PRINT_APIENTRY printTexCoord1iv(const GLint * v)
{
}

static void PRINT_APIENTRY printTexCoord1s(GLshort s)
{
}

static void PRINT_APIENTRY printTexCoord1sv(const GLshort * v)
{
}

static void PRINT_APIENTRY printTexCoord2d(GLdouble s, GLdouble t)
{
}

static void PRINT_APIENTRY printTexCoord2dv(const GLdouble * v)
{
}

static void PRINT_APIENTRY printTexCoord2f(GLfloat s, GLfloat t)
{
}

static void PRINT_APIENTRY printTexCoord2fv(const GLfloat * v)
{
}

static void PRINT_APIENTRY printTexCoord2i(GLint s, GLint t)
{
}

static void PRINT_APIENTRY printTexCoord2iv(const GLint * v)
{
}

static void PRINT_APIENTRY printTexCoord2s(GLshort s, GLshort t)
{
}

static void PRINT_APIENTRY printTexCoord2sv(const GLshort * v)
{
}

static void PRINT_APIENTRY printTexCoord3d(GLdouble s, GLdouble t, GLdouble r)
{
}

static void PRINT_APIENTRY printTexCoord3dv(const GLdouble * v)
{
}

static void PRINT_APIENTRY printTexCoord3f(GLfloat s, GLfloat t, GLfloat r)
{
}

static void PRINT_APIENTRY printTexCoord3fv(const GLfloat * v)
{
}

static void PRINT_APIENTRY printTexCoord3i(GLint s, GLint t, GLint r)
{
}

static void PRINT_APIENTRY printTexCoord3iv(const GLint * v)
{
}

static void PRINT_APIENTRY printTexCoord3s(GLshort s, GLshort t, GLshort r)
{
}

static void PRINT_APIENTRY printTexCoord3sv(const GLshort * v)
{
}

static void PRINT_APIENTRY printTexCoord4d(GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
}

static void PRINT_APIENTRY printTexCoord4dv(const GLdouble * v)
{
}

static void PRINT_APIENTRY printTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
}

static void PRINT_APIENTRY printTexCoord4fv(const GLfloat * v)
{
}

static void PRINT_APIENTRY printTexCoord4i(GLint s, GLint t, GLint r, GLint q)
{
}

static void PRINT_APIENTRY printTexCoord4iv(const GLint * v)
{
}

static void PRINT_APIENTRY printTexCoord4s(GLshort s, GLshort t, GLshort r, GLshort q)
{
}

static void PRINT_APIENTRY printTexCoord4sv(const GLshort * v)
{
}

static void PRINT_APIENTRY printTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
}

static void PRINT_APIENTRY printTexGend(GLenum coord, GLenum pname, GLdouble param)
{
}

static void PRINT_APIENTRY printTexGendv(GLenum coord, GLenum pname, const GLdouble * params)
{
}

static void PRINT_APIENTRY printTexGenf(GLenum coord, GLenum pname, GLfloat param)
{
}

static void PRINT_APIENTRY printTexGenfv(GLenum coord, GLenum pname, const GLfloat * params)
{
}

static void PRINT_APIENTRY printTexGeni(GLenum coord, GLenum pname, GLint param)
{
}

static void PRINT_APIENTRY printTexGeniv(GLenum coord, GLenum pname, const GLint * params)
{
}

static void PRINT_APIENTRY printTexImage1D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid * pixels)
{
}

static void PRINT_APIENTRY printTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * pixels)
{
}

static void PRINT_APIENTRY printTexImage3D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid * pixels)
{
}

static void PRINT_APIENTRY printTexImage3DEXT(GLenum target, GLint level, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid * pixels)
{
}

static void PRINT_APIENTRY printTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
}

static void PRINT_APIENTRY printTexParameterfv(GLenum target, GLenum pname, const GLfloat * params)
{
}

static void PRINT_APIENTRY printTexParameteri(GLenum target, GLenum pname, GLint param)
{
}

static void PRINT_APIENTRY printTexParameteriv(GLenum target, GLenum pname, const GLint * params)
{
}

static void PRINT_APIENTRY printTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid * pixels)
{
}

static void PRINT_APIENTRY printTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pixels)
{
}

static void PRINT_APIENTRY printTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid * pixels)
{
}

static void PRINT_APIENTRY printTrackMatrixNV(GLenum target, GLuint address, GLenum matrix, GLenum transform)
{
}

static void PRINT_APIENTRY printTranslated(GLdouble x, GLdouble y, GLdouble z)
{
    if (scenegraph_spu_data.g_currentMatrixMode != GL_MODELVIEW)
        return;
    osg::Matrix mat;
    mat.makeTranslate(x, y, z);
    scenegraph_spu_data.g_CurrentMatrix = mat * scenegraph_spu_data.g_CurrentMatrix;

}

static void PRINT_APIENTRY printTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
    printTranslated(x, y, z);
}

static GLboolean PRINT_APIENTRY printUnmapBufferARB(GLenum target)
{
    return false;
}

static void PRINT_APIENTRY printVertex2d(GLdouble x, GLdouble y)
{
}

static void PRINT_APIENTRY printVertex2dv(const GLdouble * v)
{
}

static void PRINT_APIENTRY printVertex2f(GLfloat x, GLfloat y)
{
}

static void PRINT_APIENTRY printVertex2fv(const GLfloat * v)
{
}

static void PRINT_APIENTRY printVertex2i(GLint x, GLint y)
{
}

static void PRINT_APIENTRY printVertex2iv(const GLint * v)
{
}

static void PRINT_APIENTRY printVertex2s(GLshort x, GLshort y)
{
}

static void PRINT_APIENTRY printVertex2sv(const GLshort * v)
{
}

static void PRINT_APIENTRY printVertex3d(GLdouble x, GLdouble y, GLdouble z)
{
    if ((scenegraph_spu_data.g_isReading || scenegraph_spu_data.g_isDisplayList) && scenegraph_spu_data.g_vertexArray){
#ifdef DISABLE_TRANSFORM_MULT_WITH_VERTICES
        osg::Vec3 vertexPoint = osg::Vec3(x, y, z);
        osg::Vec3 normalPoint = scenegraph_spu_data.g_CurrentNormal;
#else
        // Math to transfrom vertex and normal to matrix mode
        osg::Matrix mat = osg::Matrix::translate(osg::Vec3(x, y, z));
        osg::Matrix matFinal = mat *  scenegraph_spu_data.g_CurrentMatrix * scenegraph_spu_data.g_camera_matrix;
        osg::Vec3 vertexPoint = matFinal.getTrans();
        osg::Matrix Normalmat = osg::Matrix::translate(scenegraph_spu_data.g_CurrentNormal);
        osg::Matrix CurrentMatrixNew = scenegraph_spu_data.g_CurrentMatrix * scenegraph_spu_data.g_camera_matrix;
        CurrentMatrixNew.setTrans(osg::Vec3(0, 0, 0));
        osg::Matrix NormalmatFinal = Normalmat * CurrentMatrixNew;
        osg::Vec3 normalPoint = NormalmatFinal.getTrans();

#endif
        scenegraph_spu_data.g_vertexArray->push_back(vertexPoint);
        scenegraph_spu_data.g_normalArray->push_back(normalPoint);
        scenegraph_spu_data.g_colorArray->push_back(scenegraph_spu_data.g_CurrentColor);
    }
}

static void PRINT_APIENTRY printVertex3dv(const GLdouble * v)
{
    printVertex3d(v[0], v[1], v[2]);
}

static void PRINT_APIENTRY printVertex3f(GLfloat x, GLfloat y, GLfloat z)
{
    printVertex3d(x, y, z);
}

static void PRINT_APIENTRY printVertex3fv(const GLfloat * v)
{
    printVertex3d(v[0], v[1], v[2]);
}

static void PRINT_APIENTRY printVertex3i(GLint x, GLint y, GLint z)
{
    printVertex3d(x, y, z);
}

static void PRINT_APIENTRY printVertex3iv(const GLint * v)
{
    printVertex3d(v[0], v[1], v[2]);
}

static void PRINT_APIENTRY printVertex3s(GLshort x, GLshort y, GLshort z)
{
    printVertex3d(x, y, z);
}

static void PRINT_APIENTRY printVertex3sv(const GLshort * v)
{
    printVertex3d(v[0], v[1], v[2]);
}

static void PRINT_APIENTRY printVertex4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    printVertex3d(x, y, z);
}

static void PRINT_APIENTRY printVertex4dv(const GLdouble * v)
{
    printVertex3d(v[0], v[1], v[2]);
}

static void PRINT_APIENTRY printVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    printVertex3d(x, y, z);
}

static void PRINT_APIENTRY printVertex4fv(const GLfloat * v)
{
    printVertex3d(v[0], v[1], v[2]);
}

static void PRINT_APIENTRY printVertex4i(GLint x, GLint y, GLint z, GLint w)
{
    printVertex3d(x, y, z);
}

static void PRINT_APIENTRY printVertex4iv(const GLint * v)
{
    printVertex3d(v[0], v[1], v[2]);
}

static void PRINT_APIENTRY printVertex4s(GLshort x, GLshort y, GLshort z, GLshort w)
{
    printVertex3d(x, y, z);
}

static void PRINT_APIENTRY printVertex4sv(const GLshort * v)
{
}

static void PRINT_APIENTRY printg_vertexArrayRangeNV(GLsizei length, const GLvoid * pointer)
{
}

static void PRINT_APIENTRY printVertexAttrib1dARB(GLuint index, GLdouble x)
{
}

static void PRINT_APIENTRY printVertexAttrib1dvARB(GLuint index, const GLdouble * v)
{
}

static void PRINT_APIENTRY printVertexAttrib1fARB(GLuint index, GLfloat x)
{
}

static void PRINT_APIENTRY printVertexAttrib1fvARB(GLuint index, const GLfloat * v)
{
}

static void PRINT_APIENTRY printVertexAttrib1sARB(GLuint index, GLshort x)
{
}

static void PRINT_APIENTRY printVertexAttrib1svARB(GLuint index, const GLshort * v)
{
}

static void PRINT_APIENTRY printVertexAttrib2dARB(GLuint index, GLdouble x, GLdouble y)
{
}

static void PRINT_APIENTRY printVertexAttrib2dvARB(GLuint index, const GLdouble * v)
{
}

static void PRINT_APIENTRY printVertexAttrib2fARB(GLuint index, GLfloat x, GLfloat y)
{
}

static void PRINT_APIENTRY printVertexAttrib2fvARB(GLuint index, const GLfloat * v)
{
}

static void PRINT_APIENTRY printVertexAttrib2sARB(GLuint index, GLshort x, GLshort y)
{
}

static void PRINT_APIENTRY printVertexAttrib2svARB(GLuint index, const GLshort * v)
{
}

static void PRINT_APIENTRY printVertexAttrib3dARB(GLuint index, GLdouble x, GLdouble y, GLdouble z)
{
}

static void PRINT_APIENTRY printVertexAttrib3dvARB(GLuint index, const GLdouble * v)
{
}

static void PRINT_APIENTRY printVertexAttrib3fARB(GLuint index, GLfloat x, GLfloat y, GLfloat z)
{
}

static void PRINT_APIENTRY printVertexAttrib3fvARB(GLuint index, const GLfloat * v)
{
}

static void PRINT_APIENTRY printVertexAttrib3sARB(GLuint index, GLshort x, GLshort y, GLshort z)
{
}

static void PRINT_APIENTRY printVertexAttrib3svARB(GLuint index, const GLshort * v)
{
}

static void PRINT_APIENTRY printVertexAttrib4NbvARB(GLuint index, const GLbyte * v)
{
}

static void PRINT_APIENTRY printVertexAttrib4NivARB(GLuint index, const GLint * v)
{
}

static void PRINT_APIENTRY printVertexAttrib4NsvARB(GLuint index, const GLshort * v)
{
}

static void PRINT_APIENTRY printVertexAttrib4NubARB(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w)
{
}

static void PRINT_APIENTRY printVertexAttrib4NubvARB(GLuint index, const GLubyte * v)
{
}

static void PRINT_APIENTRY printVertexAttrib4NuivARB(GLuint index, const GLuint * v)
{
}

static void PRINT_APIENTRY printVertexAttrib4NusvARB(GLuint index, const GLushort * v)
{
}

static void PRINT_APIENTRY printVertexAttrib4bvARB(GLuint index, const GLbyte * v)
{
}

static void PRINT_APIENTRY printVertexAttrib4dARB(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
}

static void PRINT_APIENTRY printVertexAttrib4dvARB(GLuint index, const GLdouble * v)
{
}

static void PRINT_APIENTRY printVertexAttrib4fARB(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
}

static void PRINT_APIENTRY printVertexAttrib4fvARB(GLuint index, const GLfloat * v)
{
}

static void PRINT_APIENTRY printVertexAttrib4ivARB(GLuint index, const GLint * v)
{
}

static void PRINT_APIENTRY printVertexAttrib4sARB(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w)
{
}

static void PRINT_APIENTRY printVertexAttrib4svARB(GLuint index, const GLshort * v)
{
}

static void PRINT_APIENTRY printVertexAttrib4ubvARB(GLuint index, const GLubyte * v)
{
}

static void PRINT_APIENTRY printVertexAttrib4uivARB(GLuint index, const GLuint * v)
{
}

static void PRINT_APIENTRY printVertexAttrib4usvARB(GLuint index, const GLushort * v)
{
}

static void PRINT_APIENTRY printVertexAttribPointerARB(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer)
{
}

static void PRINT_APIENTRY printVertexAttribPointerNV(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
}

static void PRINT_APIENTRY printVertexAttribs1dvNV(GLuint index, GLsizei n, const GLdouble * v)
{
}

static void PRINT_APIENTRY printVertexAttribs1fvNV(GLuint index, GLsizei n, const GLfloat * v)
{
}

static void PRINT_APIENTRY printVertexAttribs1svNV(GLuint index, GLsizei n, const GLshort * v)
{
}

static void PRINT_APIENTRY printVertexAttribs2dvNV(GLuint index, GLsizei n, const GLdouble * v)
{
}

static void PRINT_APIENTRY printVertexAttribs2fvNV(GLuint index, GLsizei n, const GLfloat * v)
{
}

static void PRINT_APIENTRY printVertexAttribs2svNV(GLuint index, GLsizei n, const GLshort * v)
{
}

static void PRINT_APIENTRY printVertexAttribs3dvNV(GLuint index, GLsizei n, const GLdouble * v)
{
}

static void PRINT_APIENTRY printVertexAttribs3fvNV(GLuint index, GLsizei n, const GLfloat * v)
{
}

static void PRINT_APIENTRY printVertexAttribs3svNV(GLuint index, GLsizei n, const GLshort * v)
{
}

static void PRINT_APIENTRY printVertexAttribs4dvNV(GLuint index, GLsizei n, const GLdouble * v)
{
}

static void PRINT_APIENTRY printVertexAttribs4fvNV(GLuint index, GLsizei n, const GLfloat * v)
{
}

static void PRINT_APIENTRY printVertexAttribs4svNV(GLuint index, GLsizei n, const GLshort * v)
{
}

static void PRINT_APIENTRY printVertexAttribs4ubvNV(GLuint index, GLsizei n, const GLubyte * v)
{
}

static void PRINT_APIENTRY printVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
    //no need to handle already handled in packSpu
}

static void PRINT_APIENTRY printViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{

}

static GLint PRINT_APIENTRY printWindowCreate(const char * dpyName, GLint visBits)
{
    return scenegraph_spu_data.g_ret_count++;
}

static void PRINT_APIENTRY printWindowDestroy(GLint window)
{
}

static void PRINT_APIENTRY printWindowPos2dARB(GLdouble x, GLdouble y)
{
}

static void PRINT_APIENTRY printWindowPos2dvARB(const GLdouble * v)
{
}

static void PRINT_APIENTRY printWindowPos2fARB(GLfloat x, GLfloat y)
{
}

static void PRINT_APIENTRY printWindowPos2fvARB(const GLfloat * v)
{
}

static void PRINT_APIENTRY printWindowPos2iARB(GLint x, GLint y)
{
}

static void PRINT_APIENTRY printWindowPos2ivARB(const GLint * v)
{
}

static void PRINT_APIENTRY printWindowPos2sARB(GLshort x, GLshort y)
{
}

static void PRINT_APIENTRY printWindowPos2svARB(const GLshort * v)
{
}

static void PRINT_APIENTRY printWindowPos3dARB(GLdouble x, GLdouble y, GLdouble z)
{
}

static void PRINT_APIENTRY printWindowPos3dvARB(const GLdouble * v)
{
}

static void PRINT_APIENTRY printWindowPos3fARB(GLfloat x, GLfloat y, GLfloat z)
{
}

static void PRINT_APIENTRY printWindowPos3fvARB(const GLfloat * v)
{
}

static void PRINT_APIENTRY printWindowPos3iARB(GLint x, GLint y, GLint z)
{
}

static void PRINT_APIENTRY printWindowPos3ivARB(const GLint * v)
{
}

static void PRINT_APIENTRY printWindowPos3sARB(GLshort x, GLshort y, GLshort z)
{
}

static void PRINT_APIENTRY printWindowPos3svARB(const GLshort * v)
{
}

static void PRINT_APIENTRY printWindowPosition(GLint window, GLint x, GLint y)
{
}

static void PRINT_APIENTRY printWindowShow(GLint window, GLint flag)
{
}

static void PRINT_APIENTRY printWindowSize(GLint window, GLint w, GLint h)
{
}

static void PRINT_APIENTRY printWriteback(GLint * writeback)
{
}

static void PRINT_APIENTRY printZPixCR(GLsizei width, GLsizei height, GLenum format, GLenum type, GLenum ztype, GLint zparm, GLint length, const GLvoid * pixels)
{
}

// material function copied here for color
static void PRINT_APIENTRY printMaterialfv(GLenum face, GLenum mode, const GLfloat *paramstemp)
{

    if (!scenegraph_spu_data.g_isReading)
    {
        return;
    }

#ifdef ENABLE_MATERIAL

    if (scenegraph_spu_data.g_material == NULL)
    {
        scenegraph_spu_data.g_material = new osg::Material();
    }
    osg::Material::Face osgface = osg::Material::Face::FRONT;

    switch (face){
    case GL_BACK:
        osgface = osg::Material::Face::BACK;
        break;
    case GL_FRONT_AND_BACK:
        osgface = osg::Material::Face::FRONT_AND_BACK;
        break;
    }

    GLfloat params[4];
    params[0] = paramstemp[0];
    params[1] = paramstemp[1];
    params[2] = paramstemp[2];
    params[3] = paramstemp[3];

    switch (mode)
    {
    case GL_AMBIENT:
        scenegraph_spu_data.g_needToCreateNewGeode = true;
        scenegraph_spu_data.g_material->setAmbient(osgface, osg::Vec4(params[0], params[1], params[2], params[3]));
        break;
    case GL_DIFFUSE:
        scenegraph_spu_data.g_needToCreateNewGeode = true;
        scenegraph_spu_data.g_material->setDiffuse(osgface, osg::Vec4(params[0], params[1], params[2], params[3]));
        break;
    case GL_SPECULAR:
        scenegraph_spu_data.g_needToCreateNewGeode = true;
        scenegraph_spu_data.g_material->setSpecular(osgface, osg::Vec4(params[0], params[1], params[2], params[3]));
        break;
    case GL_AMBIENT_AND_DIFFUSE:
        scenegraph_spu_data.g_needToCreateNewGeode = true;
        scenegraph_spu_data.g_material->setAmbient(osgface, osg::Vec4(params[0], params[1], params[2], params[3]));
        scenegraph_spu_data.g_material->setDiffuse(osgface, osg::Vec4(params[0], params[1], params[2], params[3]));
        break;
    case GL_EMISSION:
        scenegraph_spu_data.g_needToCreateNewGeode = true;
        scenegraph_spu_data.g_material->setEmission(osgface, osg::Vec4(params[0], params[1], params[2], params[3]));
        break;
    case GL_SHININESS:
        scenegraph_spu_data.g_needToCreateNewGeode = true;
        scenegraph_spu_data.g_material->setShininess(osgface, params[0]);
        break;
    }

#endif

}

// XXX: To make this SPU work for JT2GO
// TransViz gl library is calling glaccum for post processing effects.
// after rendering the frame. This causes applicaiton to re-render 
// the frame 3 more times and thus we get 4 geometries
// this avoid this we call swapbuffer in accum. 
static void PRINT_APIENTRY printAccum(GLenum op, GLfloat value)
{
    if (scenegraph_spu_data.g_isReading)
    {
        printSwapBuffers(0, 0);
    }
}

SPUNamedFunctionTable _cr_print_table[] = {
    { "Accum", (SPUGenericFunction)printAccum },
    { "ActiveTextureARB", (SPUGenericFunction)printActiveTextureARB },
    { "AlphaFunc", (SPUGenericFunction)printAlphaFunc },
    { "AreProgramsResidentNV", (SPUGenericFunction)printAreProgramsResidentNV },
    { "AreTexturesResident", (SPUGenericFunction)printAreTexturesResident },
    { "ArrayElement", (SPUGenericFunction)printArrayElement },
    { "BarrierCreateCR", (SPUGenericFunction)printBarrierCreateCR },
    { "BarrierDestroyCR", (SPUGenericFunction)printBarrierDestroyCR },
    { "BarrierExecCR", (SPUGenericFunction)printBarrierExecCR },
    { "Begin", (SPUGenericFunction)printBegin },
    { "BeginQueryARB", (SPUGenericFunction)printBeginQueryARB },
    { "BindBufferARB", (SPUGenericFunction)printBindBufferARB },
    { "BindProgramARB", (SPUGenericFunction)printBindProgramARB },
    { "BindProgramNV", (SPUGenericFunction)printBindProgramNV },
    { "BindTexture", (SPUGenericFunction)printBindTexture },
    { "Bitmap", (SPUGenericFunction)printBitmap },
    { "BlendColorEXT", (SPUGenericFunction)printBlendColorEXT },
    { "BlendEquationEXT", (SPUGenericFunction)printBlendEquationEXT },
    { "BlendFunc", (SPUGenericFunction)printBlendFunc },
    { "BlendFuncSeparateEXT", (SPUGenericFunction)printBlendFuncSeparateEXT },
    { "BoundsInfoCR", (SPUGenericFunction)printBoundsInfoCR },
    { "BufferDataARB", (SPUGenericFunction)printBufferDataARB },
    { "BufferSubDataARB", (SPUGenericFunction)printBufferSubDataARB },
    { "CallList", (SPUGenericFunction)printCallList },
    { "CallLists", (SPUGenericFunction)printCallLists },
    { "ChromiumParameterfCR", (SPUGenericFunction)printChromiumParameterfCR },
    { "ChromiumParameteriCR", (SPUGenericFunction)printChromiumParameteriCR },
    { "ChromiumParametervCR", (SPUGenericFunction)printChromiumParametervCR },
    { "Clear", (SPUGenericFunction)printClear },
    { "ClearAccum", (SPUGenericFunction)printClearAccum },
    { "ClearColor", (SPUGenericFunction)printClearColor },
    { "ClearDepth", (SPUGenericFunction)printClearDepth },
    { "ClearIndex", (SPUGenericFunction)printClearIndex },
    { "ClearStencil", (SPUGenericFunction)printClearStencil },
    { "ClientActiveTextureARB", (SPUGenericFunction)printClientActiveTextureARB },
    { "ClipPlane", (SPUGenericFunction)printClipPlane },
    { "Color3b", (SPUGenericFunction)printColor3b },
    { "Color3bv", (SPUGenericFunction)printColor3bv },
    { "Color3d", (SPUGenericFunction)printColor3d },
    { "Color3dv", (SPUGenericFunction)printColor3dv },
    { "Color3f", (SPUGenericFunction)printColor3f },
    { "Color3fv", (SPUGenericFunction)printColor3fv },
    { "Color3i", (SPUGenericFunction)printColor3i },
    { "Color3iv", (SPUGenericFunction)printColor3iv },
    { "Color3s", (SPUGenericFunction)printColor3s },
    { "Color3sv", (SPUGenericFunction)printColor3sv },
    { "Color3ub", (SPUGenericFunction)printColor3ub },
    { "Color3ubv", (SPUGenericFunction)printColor3ubv },
    { "Color3ui", (SPUGenericFunction)printColor3ui },
    { "Color3uiv", (SPUGenericFunction)printColor3uiv },
    { "Color3us", (SPUGenericFunction)printColor3us },
    { "Color3usv", (SPUGenericFunction)printColor3usv },
    { "Color4b", (SPUGenericFunction)printColor4b },
    { "Color4bv", (SPUGenericFunction)printColor4bv },
    { "Color4d", (SPUGenericFunction)printColor4d },
    { "Color4dv", (SPUGenericFunction)printColor4dv },
    { "Color4f", (SPUGenericFunction)printColor4f },
    { "Color4fv", (SPUGenericFunction)printColor4fv },
    { "Color4i", (SPUGenericFunction)printColor4i },
    { "Color4iv", (SPUGenericFunction)printColor4iv },
    { "Color4s", (SPUGenericFunction)printColor4s },
    { "Color4sv", (SPUGenericFunction)printColor4sv },
    { "Color4ub", (SPUGenericFunction)printColor4ub },
    { "Color4ubv", (SPUGenericFunction)printColor4ubv },
    { "Color4ui", (SPUGenericFunction)printColor4ui },
    { "Color4uiv", (SPUGenericFunction)printColor4uiv },
    { "Color4us", (SPUGenericFunction)printColor4us },
    { "Color4usv", (SPUGenericFunction)printColor4usv },
    { "ColorMask", (SPUGenericFunction)printColorMask },
    { "ColorMaterial", (SPUGenericFunction)printColorMaterial },
    { "ColorPointer", (SPUGenericFunction)printColorPointer },
    { "CombinerInputNV", (SPUGenericFunction)printCombinerInputNV },
    { "CombinerOutputNV", (SPUGenericFunction)printCombinerOutputNV },
    { "CombinerParameterfNV", (SPUGenericFunction)printCombinerParameterfNV },
    { "CombinerParameterfvNV", (SPUGenericFunction)printCombinerParameterfvNV },
    { "CombinerParameteriNV", (SPUGenericFunction)printCombinerParameteriNV },
    { "CombinerParameterivNV", (SPUGenericFunction)printCombinerParameterivNV },
    { "CombinerStageParameterfvNV", (SPUGenericFunction)printCombinerStageParameterfvNV },
    { "CompressedTexImage1DARB", (SPUGenericFunction)printCompressedTexImage1DARB },
    { "CompressedTexImage2DARB", (SPUGenericFunction)printCompressedTexImage2DARB },
    { "CompressedTexImage3DARB", (SPUGenericFunction)printCompressedTexImage3DARB },
    { "CompressedTexSubImage1DARB", (SPUGenericFunction)printCompressedTexSubImage1DARB },
    { "CompressedTexSubImage2DARB", (SPUGenericFunction)printCompressedTexSubImage2DARB },
    { "CompressedTexSubImage3DARB", (SPUGenericFunction)printCompressedTexSubImage3DARB },
    { "CopyPixels", (SPUGenericFunction)printCopyPixels },
    { "CopyTexImage1D", (SPUGenericFunction)printCopyTexImage1D },
    { "CopyTexImage2D", (SPUGenericFunction)printCopyTexImage2D },
    { "CopyTexSubImage1D", (SPUGenericFunction)printCopyTexSubImage1D },
    { "CopyTexSubImage2D", (SPUGenericFunction)printCopyTexSubImage2D },
    { "CopyTexSubImage3D", (SPUGenericFunction)printCopyTexSubImage3D },
    { "CreateContext", (SPUGenericFunction)printCreateContext },
    { "CullFace", (SPUGenericFunction)printCullFace },
    { "DeleteBuffersARB", (SPUGenericFunction)printDeleteBuffersARB },
    { "DeleteFencesNV", (SPUGenericFunction)printDeleteFencesNV },
    { "DeleteLists", (SPUGenericFunction)printDeleteLists },
    { "DeleteProgramsARB", (SPUGenericFunction)printDeleteProgramsARB },
    { "DeleteQueriesARB", (SPUGenericFunction)printDeleteQueriesARB },
    { "DeleteTextures", (SPUGenericFunction)printDeleteTextures },
    { "DepthFunc", (SPUGenericFunction)printDepthFunc },
    { "DepthMask", (SPUGenericFunction)printDepthMask },
    { "DepthRange", (SPUGenericFunction)printDepthRange },
    { "DestroyContext", (SPUGenericFunction)printDestroyContext },
    { "Disable", (SPUGenericFunction)printDisable },
    { "DisableClientState", (SPUGenericFunction)printDisableClientState },
    { "DisableVertexAttribArrayARB", (SPUGenericFunction)printDisableVertexAttribArrayARB },
    { "DrawArrays", (SPUGenericFunction)printDrawArrays },
    { "DrawBuffer", (SPUGenericFunction)printDrawBuffer },
    { "DrawElements", (SPUGenericFunction)printDrawElements },
    { "DrawPixels", (SPUGenericFunction)printDrawPixels },
    { "DrawRangeElements", (SPUGenericFunction)printDrawRangeElements },
    { "EdgeFlag", (SPUGenericFunction)printEdgeFlag },
    { "EdgeFlagPointer", (SPUGenericFunction)printEdgeFlagPointer },
    { "EdgeFlagv", (SPUGenericFunction)printEdgeFlagv },
    { "Enable", (SPUGenericFunction)printEnable },
    { "EnableClientState", (SPUGenericFunction)printEnableClientState },
    { "EnableVertexAttribArrayARB", (SPUGenericFunction)printEnableVertexAttribArrayARB },
    { "End", (SPUGenericFunction)printEnd },
    { "EndList", (SPUGenericFunction)printEndList },
    { "EndQueryARB", (SPUGenericFunction)printEndQueryARB },
    { "EvalCoord1d", (SPUGenericFunction)printEvalCoord1d },
    { "EvalCoord1dv", (SPUGenericFunction)printEvalCoord1dv },
    { "EvalCoord1f", (SPUGenericFunction)printEvalCoord1f },
    { "EvalCoord1fv", (SPUGenericFunction)printEvalCoord1fv },
    { "EvalCoord2d", (SPUGenericFunction)printEvalCoord2d },
    { "EvalCoord2dv", (SPUGenericFunction)printEvalCoord2dv },
    { "EvalCoord2f", (SPUGenericFunction)printEvalCoord2f },
    { "EvalCoord2fv", (SPUGenericFunction)printEvalCoord2fv },
    { "EvalMesh1", (SPUGenericFunction)printEvalMesh1 },
    { "EvalMesh2", (SPUGenericFunction)printEvalMesh2 },
    { "EvalPoint1", (SPUGenericFunction)printEvalPoint1 },
    { "EvalPoint2", (SPUGenericFunction)printEvalPoint2 },
    { "ExecuteProgramNV", (SPUGenericFunction)printExecuteProgramNV },
    { "FeedbackBuffer", (SPUGenericFunction)printFeedbackBuffer },
    { "FinalCombinerInputNV", (SPUGenericFunction)printFinalCombinerInputNV },
    { "Finish", (SPUGenericFunction)printFinish },
    { "FinishFenceNV", (SPUGenericFunction)printFinishFenceNV },
    { "Flush", (SPUGenericFunction)printFlush },
    { "Flushg_vertexArrayRangeNV", (SPUGenericFunction)printFlushg_vertexArrayRangeNV },
    { "FogCoordPointerEXT", (SPUGenericFunction)printFogCoordPointerEXT },
    { "FogCoorddEXT", (SPUGenericFunction)printFogCoorddEXT },
    { "FogCoorddvEXT", (SPUGenericFunction)printFogCoorddvEXT },
    { "FogCoordfEXT", (SPUGenericFunction)printFogCoordfEXT },
    { "FogCoordfvEXT", (SPUGenericFunction)printFogCoordfvEXT },
    { "Fogf", (SPUGenericFunction)printFogf },
    { "Fogfv", (SPUGenericFunction)printFogfv },
    { "Fogi", (SPUGenericFunction)printFogi },
    { "Fogiv", (SPUGenericFunction)printFogiv },
    { "FrontFace", (SPUGenericFunction)printFrontFace },
    { "Frustum", (SPUGenericFunction)printFrustum },
    { "GenBuffersARB", (SPUGenericFunction)printGenBuffersARB },
    { "GenFencesNV", (SPUGenericFunction)printGenFencesNV },
    { "GenLists", (SPUGenericFunction)printGenLists },
    { "GenProgramsARB", (SPUGenericFunction)printGenProgramsARB },
    { "GenProgramsNV", (SPUGenericFunction)printGenProgramsNV },
    { "GenQueriesARB", (SPUGenericFunction)printGenQueriesARB },
    { "GenTextures", (SPUGenericFunction)printGenTextures },
    { "GetBooleanv", (SPUGenericFunction)printGetBooleanv },
    { "GetBufferParameterivARB", (SPUGenericFunction)printGetBufferParameterivARB },
    { "GetBufferPointervARB", (SPUGenericFunction)printGetBufferPointervARB },
    { "GetBufferSubDataARB", (SPUGenericFunction)printGetBufferSubDataARB },
    { "GetChromiumParametervCR", (SPUGenericFunction)printGetChromiumParametervCR },
    { "GetClipPlane", (SPUGenericFunction)printGetClipPlane },
    { "GetCombinerInputParameterfvNV", (SPUGenericFunction)printGetCombinerInputParameterfvNV },
    { "GetCombinerInputParameterivNV", (SPUGenericFunction)printGetCombinerInputParameterivNV },
    { "GetCombinerOutputParameterfvNV", (SPUGenericFunction)printGetCombinerOutputParameterfvNV },
    { "GetCombinerOutputParameterivNV", (SPUGenericFunction)printGetCombinerOutputParameterivNV },
    { "GetCombinerStageParameterfvNV", (SPUGenericFunction)printGetCombinerStageParameterfvNV },
    { "GetCompressedTexImageARB", (SPUGenericFunction)printGetCompressedTexImageARB },
    { "GetDoublev", (SPUGenericFunction)printGetDoublev },
    { "GetError", (SPUGenericFunction)printGetError },
    { "GetFenceivNV", (SPUGenericFunction)printGetFenceivNV },
    { "GetFinalCombinerInputParameterfvNV", (SPUGenericFunction)printGetFinalCombinerInputParameterfvNV },
    { "GetFinalCombinerInputParameterivNV", (SPUGenericFunction)printGetFinalCombinerInputParameterivNV },
    { "GetFloatv", (SPUGenericFunction)printGetFloatv },
    { "GetIntegerv", (SPUGenericFunction)printGetIntegerv },
    { "GetLightfv", (SPUGenericFunction)printGetLightfv },
    { "GetLightiv", (SPUGenericFunction)printGetLightiv },
    { "GetMapdv", (SPUGenericFunction)printGetMapdv },
    { "GetMapfv", (SPUGenericFunction)printGetMapfv },
    { "GetMapiv", (SPUGenericFunction)printGetMapiv },
    { "GetMaterialfv", (SPUGenericFunction)printGetMaterialfv },
    { "GetMaterialiv", (SPUGenericFunction)printGetMaterialiv },
    { "GetPixelMapfv", (SPUGenericFunction)printGetPixelMapfv },
    { "GetPixelMapuiv", (SPUGenericFunction)printGetPixelMapuiv },
    { "GetPixelMapusv", (SPUGenericFunction)printGetPixelMapusv },
    { "GetPointerv", (SPUGenericFunction)printGetPointerv },
    { "GetPolygonStipple", (SPUGenericFunction)printGetPolygonStipple },
    { "GetProgramEnvParameterdvARB", (SPUGenericFunction)printGetProgramEnvParameterdvARB },
    { "GetProgramEnvParameterfvARB", (SPUGenericFunction)printGetProgramEnvParameterfvARB },
    { "GetProgramLocalParameterdvARB", (SPUGenericFunction)printGetProgramLocalParameterdvARB },
    { "GetProgramLocalParameterfvARB", (SPUGenericFunction)printGetProgramLocalParameterfvARB },
    { "GetProgramNamedParameterdvNV", (SPUGenericFunction)printGetProgramNamedParameterdvNV },
    { "GetProgramNamedParameterfvNV", (SPUGenericFunction)printGetProgramNamedParameterfvNV },
    { "GetProgramParameterdvNV", (SPUGenericFunction)printGetProgramParameterdvNV },
    { "GetProgramParameterfvNV", (SPUGenericFunction)printGetProgramParameterfvNV },
    { "GetProgramStringARB", (SPUGenericFunction)printGetProgramStringARB },
    { "GetProgramStringNV", (SPUGenericFunction)printGetProgramStringNV },
    { "GetProgramivARB", (SPUGenericFunction)printGetProgramivARB },
    { "GetProgramivNV", (SPUGenericFunction)printGetProgramivNV },
    { "GetQueryObjectivARB", (SPUGenericFunction)printGetQueryObjectivARB },
    { "GetQueryObjectuivARB", (SPUGenericFunction)printGetQueryObjectuivARB },
    { "GetQueryivARB", (SPUGenericFunction)printGetQueryivARB },
    { "GetString", (SPUGenericFunction)printGetString },
    { "GetTexEnvfv", (SPUGenericFunction)printGetTexEnvfv },
    { "GetTexEnviv", (SPUGenericFunction)printGetTexEnviv },
    { "GetTexGendv", (SPUGenericFunction)printGetTexGendv },
    { "GetTexGenfv", (SPUGenericFunction)printGetTexGenfv },
    { "GetTexGeniv", (SPUGenericFunction)printGetTexGeniv },
    { "GetTexImage", (SPUGenericFunction)printGetTexImage },
    { "GetTexLevelParameterfv", (SPUGenericFunction)printGetTexLevelParameterfv },
    { "GetTexLevelParameteriv", (SPUGenericFunction)printGetTexLevelParameteriv },
    { "GetTexParameterfv", (SPUGenericFunction)printGetTexParameterfv },
    { "GetTexParameteriv", (SPUGenericFunction)printGetTexParameteriv },
    { "GetTrackMatrixivNV", (SPUGenericFunction)printGetTrackMatrixivNV },
    { "GetVertexAttribPointervARB", (SPUGenericFunction)printGetVertexAttribPointervARB },
    { "GetVertexAttribPointervNV", (SPUGenericFunction)printGetVertexAttribPointervNV },
    { "GetVertexAttribdvARB", (SPUGenericFunction)printGetVertexAttribdvARB },
    { "GetVertexAttribdvNV", (SPUGenericFunction)printGetVertexAttribdvNV },
    { "GetVertexAttribfvARB", (SPUGenericFunction)printGetVertexAttribfvARB },
    { "GetVertexAttribfvNV", (SPUGenericFunction)printGetVertexAttribfvNV },
    { "GetVertexAttribivARB", (SPUGenericFunction)printGetVertexAttribivARB },
    { "GetVertexAttribivNV", (SPUGenericFunction)printGetVertexAttribivNV },
    { "Hint", (SPUGenericFunction)printHint },
    { "IndexMask", (SPUGenericFunction)printIndexMask },
    { "IndexPointer", (SPUGenericFunction)printIndexPointer },
    { "Indexd", (SPUGenericFunction)printIndexd },
    { "Indexdv", (SPUGenericFunction)printIndexdv },
    { "Indexf", (SPUGenericFunction)printIndexf },
    { "Indexfv", (SPUGenericFunction)printIndexfv },
    { "Indexi", (SPUGenericFunction)printIndexi },
    { "Indexiv", (SPUGenericFunction)printIndexiv },
    { "Indexs", (SPUGenericFunction)printIndexs },
    { "Indexsv", (SPUGenericFunction)printIndexsv },
    { "Indexub", (SPUGenericFunction)printIndexub },
    { "Indexubv", (SPUGenericFunction)printIndexubv },
    { "InitNames", (SPUGenericFunction)printInitNames },
    { "InterleavedArrays", (SPUGenericFunction)printInterleavedArrays },
    { "IsBufferARB", (SPUGenericFunction)printIsBufferARB },
    { "IsEnabled", (SPUGenericFunction)printIsEnabled },
    { "IsFenceNV", (SPUGenericFunction)printIsFenceNV },
    { "IsList", (SPUGenericFunction)printIsList },
    { "IsProgramARB", (SPUGenericFunction)printIsProgramARB },
    { "IsQueryARB", (SPUGenericFunction)printIsQueryARB },
    { "IsTexture", (SPUGenericFunction)printIsTexture },
    { "LightModelf", (SPUGenericFunction)printLightModelf },
    { "LightModelfv", (SPUGenericFunction)printLightModelfv },
    { "LightModeli", (SPUGenericFunction)printLightModeli },
    { "LightModeliv", (SPUGenericFunction)printLightModeliv },
    { "Lightf", (SPUGenericFunction)printLightf },
    { "Lightfv", (SPUGenericFunction)printLightfv },
    { "Lighti", (SPUGenericFunction)printLighti },
    { "Lightiv", (SPUGenericFunction)printLightiv },
    { "LineStipple", (SPUGenericFunction)printLineStipple },
    { "LineWidth", (SPUGenericFunction)printLineWidth },
    { "ListBase", (SPUGenericFunction)printListBase },
    { "LoadIdentity", (SPUGenericFunction)printLoadIdentity },
    { "LoadMatrixd", (SPUGenericFunction)printLoadMatrixd },
    { "LoadMatrixf", (SPUGenericFunction)printLoadMatrixf },
    { "LoadName", (SPUGenericFunction)printLoadName },
    { "LoadProgramNV", (SPUGenericFunction)printLoadProgramNV },
    { "LoadTransposeMatrixdARB", (SPUGenericFunction)printLoadTransposeMatrixdARB },
    { "LoadTransposeMatrixfARB", (SPUGenericFunction)printLoadTransposeMatrixfARB },
    { "LogicOp", (SPUGenericFunction)printLogicOp },
    { "MakeCurrent", (SPUGenericFunction)printMakeCurrent },
    { "Map1d", (SPUGenericFunction)printMap1d },
    { "Map1f", (SPUGenericFunction)printMap1f },
    { "Map2d", (SPUGenericFunction)printMap2d },
    { "Map2f", (SPUGenericFunction)printMap2f },
    { "MapBufferARB", (SPUGenericFunction)printMapBufferARB },
    { "MapGrid1d", (SPUGenericFunction)printMapGrid1d },
    { "MapGrid1f", (SPUGenericFunction)printMapGrid1f },
    { "MapGrid2d", (SPUGenericFunction)printMapGrid2d },
    { "MapGrid2f", (SPUGenericFunction)printMapGrid2f },
    { "Materialf", (SPUGenericFunction)printMaterialf },
    { "Materialfv", (SPUGenericFunction)printMaterialfv },
    { "Materiali", (SPUGenericFunction)printMateriali },
    { "Materialiv", (SPUGenericFunction)printMaterialiv },
    { "MatrixMode", (SPUGenericFunction)printMatrixMode },
    { "MultMatrixd", (SPUGenericFunction)printMultMatrixd },
    { "MultMatrixf", (SPUGenericFunction)printMultMatrixf },
    { "MultTransposeMatrixdARB", (SPUGenericFunction)printMultTransposeMatrixdARB },
    { "MultTransposeMatrixfARB", (SPUGenericFunction)printMultTransposeMatrixfARB },
    { "MultiDrawArraysEXT", (SPUGenericFunction)printMultiDrawArraysEXT },
    { "MultiDrawElementsEXT", (SPUGenericFunction)printMultiDrawElementsEXT },
    { "MultiTexCoord1dARB", (SPUGenericFunction)printMultiTexCoord1dARB },
    { "MultiTexCoord1dvARB", (SPUGenericFunction)printMultiTexCoord1dvARB },
    { "MultiTexCoord1fARB", (SPUGenericFunction)printMultiTexCoord1fARB },
    { "MultiTexCoord1fvARB", (SPUGenericFunction)printMultiTexCoord1fvARB },
    { "MultiTexCoord1iARB", (SPUGenericFunction)printMultiTexCoord1iARB },
    { "MultiTexCoord1ivARB", (SPUGenericFunction)printMultiTexCoord1ivARB },
    { "MultiTexCoord1sARB", (SPUGenericFunction)printMultiTexCoord1sARB },
    { "MultiTexCoord1svARB", (SPUGenericFunction)printMultiTexCoord1svARB },
    { "MultiTexCoord2dARB", (SPUGenericFunction)printMultiTexCoord2dARB },
    { "MultiTexCoord2dvARB", (SPUGenericFunction)printMultiTexCoord2dvARB },
    { "MultiTexCoord2fARB", (SPUGenericFunction)printMultiTexCoord2fARB },
    { "MultiTexCoord2fvARB", (SPUGenericFunction)printMultiTexCoord2fvARB },
    { "MultiTexCoord2iARB", (SPUGenericFunction)printMultiTexCoord2iARB },
    { "MultiTexCoord2ivARB", (SPUGenericFunction)printMultiTexCoord2ivARB },
    { "MultiTexCoord2sARB", (SPUGenericFunction)printMultiTexCoord2sARB },
    { "MultiTexCoord2svARB", (SPUGenericFunction)printMultiTexCoord2svARB },
    { "MultiTexCoord3dARB", (SPUGenericFunction)printMultiTexCoord3dARB },
    { "MultiTexCoord3dvARB", (SPUGenericFunction)printMultiTexCoord3dvARB },
    { "MultiTexCoord3fARB", (SPUGenericFunction)printMultiTexCoord3fARB },
    { "MultiTexCoord3fvARB", (SPUGenericFunction)printMultiTexCoord3fvARB },
    { "MultiTexCoord3iARB", (SPUGenericFunction)printMultiTexCoord3iARB },
    { "MultiTexCoord3ivARB", (SPUGenericFunction)printMultiTexCoord3ivARB },
    { "MultiTexCoord3sARB", (SPUGenericFunction)printMultiTexCoord3sARB },
    { "MultiTexCoord3svARB", (SPUGenericFunction)printMultiTexCoord3svARB },
    { "MultiTexCoord4dARB", (SPUGenericFunction)printMultiTexCoord4dARB },
    { "MultiTexCoord4dvARB", (SPUGenericFunction)printMultiTexCoord4dvARB },
    { "MultiTexCoord4fARB", (SPUGenericFunction)printMultiTexCoord4fARB },
    { "MultiTexCoord4fvARB", (SPUGenericFunction)printMultiTexCoord4fvARB },
    { "MultiTexCoord4iARB", (SPUGenericFunction)printMultiTexCoord4iARB },
    { "MultiTexCoord4ivARB", (SPUGenericFunction)printMultiTexCoord4ivARB },
    { "MultiTexCoord4sARB", (SPUGenericFunction)printMultiTexCoord4sARB },
    { "MultiTexCoord4svARB", (SPUGenericFunction)printMultiTexCoord4svARB },
    { "NewList", (SPUGenericFunction)printNewList },
    { "Normal3b", (SPUGenericFunction)printNormal3b },
    { "Normal3bv", (SPUGenericFunction)printNormal3bv },
    { "Normal3d", (SPUGenericFunction)printNormal3d },
    { "Normal3dv", (SPUGenericFunction)printNormal3dv },
    { "Normal3f", (SPUGenericFunction)printNormal3f },
    { "Normal3fv", (SPUGenericFunction)printNormal3fv },
    { "Normal3i", (SPUGenericFunction)printNormal3i },
    { "Normal3iv", (SPUGenericFunction)printNormal3iv },
    { "Normal3s", (SPUGenericFunction)printNormal3s },
    { "Normal3sv", (SPUGenericFunction)printNormal3sv },
    { "NormalPointer", (SPUGenericFunction)printNormalPointer },
    { "Ortho", (SPUGenericFunction)printOrtho },
    { "PassThrough", (SPUGenericFunction)printPassThrough },
    { "PixelMapfv", (SPUGenericFunction)printPixelMapfv },
    { "PixelMapuiv", (SPUGenericFunction)printPixelMapuiv },
    { "PixelMapusv", (SPUGenericFunction)printPixelMapusv },
    { "PixelStoref", (SPUGenericFunction)printPixelStoref },
    { "PixelStorei", (SPUGenericFunction)printPixelStorei },
    { "PixelTransferf", (SPUGenericFunction)printPixelTransferf },
    { "PixelTransferi", (SPUGenericFunction)printPixelTransferi },
    { "PixelZoom", (SPUGenericFunction)printPixelZoom },
    { "PointParameterfARB", (SPUGenericFunction)printPointParameterfARB },
    { "PointParameterfvARB", (SPUGenericFunction)printPointParameterfvARB },
    { "PointParameteri", (SPUGenericFunction)printPointParameteri },
    { "PointParameteriv", (SPUGenericFunction)printPointParameteriv },
    { "PointSize", (SPUGenericFunction)printPointSize },
    { "PolygonMode", (SPUGenericFunction)printPolygonMode },
    { "PolygonOffset", (SPUGenericFunction)printPolygonOffset },
    { "PolygonStipple", (SPUGenericFunction)printPolygonStipple },
    { "PopAttrib", (SPUGenericFunction)printPopAttrib },
    { "PopClientAttrib", (SPUGenericFunction)printPopClientAttrib },
    { "PopMatrix", (SPUGenericFunction)printPopMatrix },
    { "PopName", (SPUGenericFunction)printPopName },
    { "PrioritizeTextures", (SPUGenericFunction)printPrioritizeTextures },
    { "ProgramEnvParameter4dARB", (SPUGenericFunction)printProgramEnvParameter4dARB },
    { "ProgramEnvParameter4dvARB", (SPUGenericFunction)printProgramEnvParameter4dvARB },
    { "ProgramEnvParameter4fARB", (SPUGenericFunction)printProgramEnvParameter4fARB },
    { "ProgramEnvParameter4fvARB", (SPUGenericFunction)printProgramEnvParameter4fvARB },
    { "ProgramLocalParameter4dARB", (SPUGenericFunction)printProgramLocalParameter4dARB },
    { "ProgramLocalParameter4dvARB", (SPUGenericFunction)printProgramLocalParameter4dvARB },
    { "ProgramLocalParameter4fARB", (SPUGenericFunction)printProgramLocalParameter4fARB },
    { "ProgramLocalParameter4fvARB", (SPUGenericFunction)printProgramLocalParameter4fvARB },
    { "ProgramNamedParameter4dNV", (SPUGenericFunction)printProgramNamedParameter4dNV },
    { "ProgramNamedParameter4dvNV", (SPUGenericFunction)printProgramNamedParameter4dvNV },
    { "ProgramNamedParameter4fNV", (SPUGenericFunction)printProgramNamedParameter4fNV },
    { "ProgramNamedParameter4fvNV", (SPUGenericFunction)printProgramNamedParameter4fvNV },
    { "ProgramParameter4dNV", (SPUGenericFunction)printProgramParameter4dNV },
    { "ProgramParameter4dvNV", (SPUGenericFunction)printProgramParameter4dvNV },
    { "ProgramParameter4fNV", (SPUGenericFunction)printProgramParameter4fNV },
    { "ProgramParameter4fvNV", (SPUGenericFunction)printProgramParameter4fvNV },
    { "ProgramParameters4dvNV", (SPUGenericFunction)printProgramParameters4dvNV },
    { "ProgramParameters4fvNV", (SPUGenericFunction)printProgramParameters4fvNV },
    { "ProgramStringARB", (SPUGenericFunction)printProgramStringARB },
    { "PushAttrib", (SPUGenericFunction)printPushAttrib },
    { "PushClientAttrib", (SPUGenericFunction)printPushClientAttrib },
    { "PushMatrix", (SPUGenericFunction)printPushMatrix },
    { "PushName", (SPUGenericFunction)printPushName },
    { "RasterPos2d", (SPUGenericFunction)printRasterPos2d },
    { "RasterPos2dv", (SPUGenericFunction)printRasterPos2dv },
    { "RasterPos2f", (SPUGenericFunction)printRasterPos2f },
    { "RasterPos2fv", (SPUGenericFunction)printRasterPos2fv },
    { "RasterPos2i", (SPUGenericFunction)printRasterPos2i },
    { "RasterPos2iv", (SPUGenericFunction)printRasterPos2iv },
    { "RasterPos2s", (SPUGenericFunction)printRasterPos2s },
    { "RasterPos2sv", (SPUGenericFunction)printRasterPos2sv },
    { "RasterPos3d", (SPUGenericFunction)printRasterPos3d },
    { "RasterPos3dv", (SPUGenericFunction)printRasterPos3dv },
    { "RasterPos3f", (SPUGenericFunction)printRasterPos3f },
    { "RasterPos3fv", (SPUGenericFunction)printRasterPos3fv },
    { "RasterPos3i", (SPUGenericFunction)printRasterPos3i },
    { "RasterPos3iv", (SPUGenericFunction)printRasterPos3iv },
    { "RasterPos3s", (SPUGenericFunction)printRasterPos3s },
    { "RasterPos3sv", (SPUGenericFunction)printRasterPos3sv },
    { "RasterPos4d", (SPUGenericFunction)printRasterPos4d },
    { "RasterPos4dv", (SPUGenericFunction)printRasterPos4dv },
    { "RasterPos4f", (SPUGenericFunction)printRasterPos4f },
    { "RasterPos4fv", (SPUGenericFunction)printRasterPos4fv },
    { "RasterPos4i", (SPUGenericFunction)printRasterPos4i },
    { "RasterPos4iv", (SPUGenericFunction)printRasterPos4iv },
    { "RasterPos4s", (SPUGenericFunction)printRasterPos4s },
    { "RasterPos4sv", (SPUGenericFunction)printRasterPos4sv },
    { "ReadBuffer", (SPUGenericFunction)printReadBuffer },
    { "ReadPixels", (SPUGenericFunction)printReadPixels },
    { "Rectd", (SPUGenericFunction)printRectd },
    { "Rectdv", (SPUGenericFunction)printRectdv },
    { "Rectf", (SPUGenericFunction)printRectf },
    { "Rectfv", (SPUGenericFunction)printRectfv },
    { "Recti", (SPUGenericFunction)printRecti },
    { "Rectiv", (SPUGenericFunction)printRectiv },
    { "Rects", (SPUGenericFunction)printRects },
    { "Rectsv", (SPUGenericFunction)printRectsv },
    { "RenderMode", (SPUGenericFunction)printRenderMode },
    { "RequestResidentProgramsNV", (SPUGenericFunction)printRequestResidentProgramsNV },
    { "Rotated", (SPUGenericFunction)printRotated },
    { "Rotatef", (SPUGenericFunction)printRotatef },
    { "SampleCoverageARB", (SPUGenericFunction)printSampleCoverageARB },
    { "Scaled", (SPUGenericFunction)printScaled },
    { "Scalef", (SPUGenericFunction)printScalef },
    { "Scissor", (SPUGenericFunction)printScissor },
    { "SecondaryColor3bEXT", (SPUGenericFunction)printSecondaryColor3bEXT },
    { "SecondaryColor3bvEXT", (SPUGenericFunction)printSecondaryColor3bvEXT },
    { "SecondaryColor3dEXT", (SPUGenericFunction)printSecondaryColor3dEXT },
    { "SecondaryColor3dvEXT", (SPUGenericFunction)printSecondaryColor3dvEXT },
    { "SecondaryColor3fEXT", (SPUGenericFunction)printSecondaryColor3fEXT },
    { "SecondaryColor3fvEXT", (SPUGenericFunction)printSecondaryColor3fvEXT },
    { "SecondaryColor3iEXT", (SPUGenericFunction)printSecondaryColor3iEXT },
    { "SecondaryColor3ivEXT", (SPUGenericFunction)printSecondaryColor3ivEXT },
    { "SecondaryColor3sEXT", (SPUGenericFunction)printSecondaryColor3sEXT },
    { "SecondaryColor3svEXT", (SPUGenericFunction)printSecondaryColor3svEXT },
    { "SecondaryColor3ubEXT", (SPUGenericFunction)printSecondaryColor3ubEXT },
    { "SecondaryColor3ubvEXT", (SPUGenericFunction)printSecondaryColor3ubvEXT },
    { "SecondaryColor3uiEXT", (SPUGenericFunction)printSecondaryColor3uiEXT },
    { "SecondaryColor3uivEXT", (SPUGenericFunction)printSecondaryColor3uivEXT },
    { "SecondaryColor3usEXT", (SPUGenericFunction)printSecondaryColor3usEXT },
    { "SecondaryColor3usvEXT", (SPUGenericFunction)printSecondaryColor3usvEXT },
    { "SecondaryColorPointerEXT", (SPUGenericFunction)printSecondaryColorPointerEXT },
    { "SelectBuffer", (SPUGenericFunction)printSelectBuffer },
    { "SemaphoreCreateCR", (SPUGenericFunction)printSemaphoreCreateCR },
    { "SemaphoreDestroyCR", (SPUGenericFunction)printSemaphoreDestroyCR },
    { "SemaphorePCR", (SPUGenericFunction)printSemaphorePCR },
    { "SemaphoreVCR", (SPUGenericFunction)printSemaphoreVCR },
    { "SetFenceNV", (SPUGenericFunction)printSetFenceNV },
    { "ShadeModel", (SPUGenericFunction)printShadeModel },
    { "StencilFunc", (SPUGenericFunction)printStencilFunc },
    { "StencilMask", (SPUGenericFunction)printStencilMask },
    { "StencilOp", (SPUGenericFunction)printStencilOp },
    { "SwapBuffers", (SPUGenericFunction)printSwapBuffers },
    { "TestFenceNV", (SPUGenericFunction)printTestFenceNV },
    { "TexCoord1d", (SPUGenericFunction)printTexCoord1d },
    { "TexCoord1dv", (SPUGenericFunction)printTexCoord1dv },
    { "TexCoord1f", (SPUGenericFunction)printTexCoord1f },
    { "TexCoord1fv", (SPUGenericFunction)printTexCoord1fv },
    { "TexCoord1i", (SPUGenericFunction)printTexCoord1i },
    { "TexCoord1iv", (SPUGenericFunction)printTexCoord1iv },
    { "TexCoord1s", (SPUGenericFunction)printTexCoord1s },
    { "TexCoord1sv", (SPUGenericFunction)printTexCoord1sv },
    { "TexCoord2d", (SPUGenericFunction)printTexCoord2d },
    { "TexCoord2dv", (SPUGenericFunction)printTexCoord2dv },
    { "TexCoord2f", (SPUGenericFunction)printTexCoord2f },
    { "TexCoord2fv", (SPUGenericFunction)printTexCoord2fv },
    { "TexCoord2i", (SPUGenericFunction)printTexCoord2i },
    { "TexCoord2iv", (SPUGenericFunction)printTexCoord2iv },
    { "TexCoord2s", (SPUGenericFunction)printTexCoord2s },
    { "TexCoord2sv", (SPUGenericFunction)printTexCoord2sv },
    { "TexCoord3d", (SPUGenericFunction)printTexCoord3d },
    { "TexCoord3dv", (SPUGenericFunction)printTexCoord3dv },
    { "TexCoord3f", (SPUGenericFunction)printTexCoord3f },
    { "TexCoord3fv", (SPUGenericFunction)printTexCoord3fv },
    { "TexCoord3i", (SPUGenericFunction)printTexCoord3i },
    { "TexCoord3iv", (SPUGenericFunction)printTexCoord3iv },
    { "TexCoord3s", (SPUGenericFunction)printTexCoord3s },
    { "TexCoord3sv", (SPUGenericFunction)printTexCoord3sv },
    { "TexCoord4d", (SPUGenericFunction)printTexCoord4d },
    { "TexCoord4dv", (SPUGenericFunction)printTexCoord4dv },
    { "TexCoord4f", (SPUGenericFunction)printTexCoord4f },
    { "TexCoord4fv", (SPUGenericFunction)printTexCoord4fv },
    { "TexCoord4i", (SPUGenericFunction)printTexCoord4i },
    { "TexCoord4iv", (SPUGenericFunction)printTexCoord4iv },
    { "TexCoord4s", (SPUGenericFunction)printTexCoord4s },
    { "TexCoord4sv", (SPUGenericFunction)printTexCoord4sv },
    { "TexCoordPointer", (SPUGenericFunction)printTexCoordPointer },
    { "TexEnvf", (SPUGenericFunction)printTexEnvf },
    { "TexEnvfv", (SPUGenericFunction)printTexEnvfv },
    { "TexEnvi", (SPUGenericFunction)printTexEnvi },
    { "TexEnviv", (SPUGenericFunction)printTexEnviv },
    { "TexGend", (SPUGenericFunction)printTexGend },
    { "TexGendv", (SPUGenericFunction)printTexGendv },
    { "TexGenf", (SPUGenericFunction)printTexGenf },
    { "TexGenfv", (SPUGenericFunction)printTexGenfv },
    { "TexGeni", (SPUGenericFunction)printTexGeni },
    { "TexGeniv", (SPUGenericFunction)printTexGeniv },
    { "TexImage1D", (SPUGenericFunction)printTexImage1D },
    { "TexImage2D", (SPUGenericFunction)printTexImage2D },
    { "TexImage3D", (SPUGenericFunction)printTexImage3D },
    { "TexImage3DEXT", (SPUGenericFunction)printTexImage3DEXT },
    { "TexParameterf", (SPUGenericFunction)printTexParameterf },
    { "TexParameterfv", (SPUGenericFunction)printTexParameterfv },
    { "TexParameteri", (SPUGenericFunction)printTexParameteri },
    { "TexParameteriv", (SPUGenericFunction)printTexParameteriv },
    { "TexSubImage1D", (SPUGenericFunction)printTexSubImage1D },
    { "TexSubImage2D", (SPUGenericFunction)printTexSubImage2D },
    { "TexSubImage3D", (SPUGenericFunction)printTexSubImage3D },
    { "TrackMatrixNV", (SPUGenericFunction)printTrackMatrixNV },
    { "Translated", (SPUGenericFunction)printTranslated },
    { "Translatef", (SPUGenericFunction)printTranslatef },
    { "UnmapBufferARB", (SPUGenericFunction)printUnmapBufferARB },
    { "Vertex2d", (SPUGenericFunction)printVertex2d },
    { "Vertex2dv", (SPUGenericFunction)printVertex2dv },
    { "Vertex2f", (SPUGenericFunction)printVertex2f },
    { "Vertex2fv", (SPUGenericFunction)printVertex2fv },
    { "Vertex2i", (SPUGenericFunction)printVertex2i },
    { "Vertex2iv", (SPUGenericFunction)printVertex2iv },
    { "Vertex2s", (SPUGenericFunction)printVertex2s },
    { "Vertex2sv", (SPUGenericFunction)printVertex2sv },
    { "Vertex3d", (SPUGenericFunction)printVertex3d },
    { "Vertex3dv", (SPUGenericFunction)printVertex3dv },
    { "Vertex3f", (SPUGenericFunction)printVertex3f },
    { "Vertex3fv", (SPUGenericFunction)printVertex3fv },
    { "Vertex3i", (SPUGenericFunction)printVertex3i },
    { "Vertex3iv", (SPUGenericFunction)printVertex3iv },
    { "Vertex3s", (SPUGenericFunction)printVertex3s },
    { "Vertex3sv", (SPUGenericFunction)printVertex3sv },
    { "Vertex4d", (SPUGenericFunction)printVertex4d },
    { "Vertex4dv", (SPUGenericFunction)printVertex4dv },
    { "Vertex4f", (SPUGenericFunction)printVertex4f },
    { "Vertex4fv", (SPUGenericFunction)printVertex4fv },
    { "Vertex4i", (SPUGenericFunction)printVertex4i },
    { "Vertex4iv", (SPUGenericFunction)printVertex4iv },
    { "Vertex4s", (SPUGenericFunction)printVertex4s },
    { "Vertex4sv", (SPUGenericFunction)printVertex4sv },
    { "g_vertexArrayRangeNV", (SPUGenericFunction)printg_vertexArrayRangeNV },
    { "VertexAttrib1dARB", (SPUGenericFunction)printVertexAttrib1dARB },
    { "VertexAttrib1dvARB", (SPUGenericFunction)printVertexAttrib1dvARB },
    { "VertexAttrib1fARB", (SPUGenericFunction)printVertexAttrib1fARB },
    { "VertexAttrib1fvARB", (SPUGenericFunction)printVertexAttrib1fvARB },
    { "VertexAttrib1sARB", (SPUGenericFunction)printVertexAttrib1sARB },
    { "VertexAttrib1svARB", (SPUGenericFunction)printVertexAttrib1svARB },
    { "VertexAttrib2dARB", (SPUGenericFunction)printVertexAttrib2dARB },
    { "VertexAttrib2dvARB", (SPUGenericFunction)printVertexAttrib2dvARB },
    { "VertexAttrib2fARB", (SPUGenericFunction)printVertexAttrib2fARB },
    { "VertexAttrib2fvARB", (SPUGenericFunction)printVertexAttrib2fvARB },
    { "VertexAttrib2sARB", (SPUGenericFunction)printVertexAttrib2sARB },
    { "VertexAttrib2svARB", (SPUGenericFunction)printVertexAttrib2svARB },
    { "VertexAttrib3dARB", (SPUGenericFunction)printVertexAttrib3dARB },
    { "VertexAttrib3dvARB", (SPUGenericFunction)printVertexAttrib3dvARB },
    { "VertexAttrib3fARB", (SPUGenericFunction)printVertexAttrib3fARB },
    { "VertexAttrib3fvARB", (SPUGenericFunction)printVertexAttrib3fvARB },
    { "VertexAttrib3sARB", (SPUGenericFunction)printVertexAttrib3sARB },
    { "VertexAttrib3svARB", (SPUGenericFunction)printVertexAttrib3svARB },
    { "VertexAttrib4NbvARB", (SPUGenericFunction)printVertexAttrib4NbvARB },
    { "VertexAttrib4NivARB", (SPUGenericFunction)printVertexAttrib4NivARB },
    { "VertexAttrib4NsvARB", (SPUGenericFunction)printVertexAttrib4NsvARB },
    { "VertexAttrib4NubARB", (SPUGenericFunction)printVertexAttrib4NubARB },
    { "VertexAttrib4NubvARB", (SPUGenericFunction)printVertexAttrib4NubvARB },
    { "VertexAttrib4NuivARB", (SPUGenericFunction)printVertexAttrib4NuivARB },
    { "VertexAttrib4NusvARB", (SPUGenericFunction)printVertexAttrib4NusvARB },
    { "VertexAttrib4bvARB", (SPUGenericFunction)printVertexAttrib4bvARB },
    { "VertexAttrib4dARB", (SPUGenericFunction)printVertexAttrib4dARB },
    { "VertexAttrib4dvARB", (SPUGenericFunction)printVertexAttrib4dvARB },
    { "VertexAttrib4fARB", (SPUGenericFunction)printVertexAttrib4fARB },
    { "VertexAttrib4fvARB", (SPUGenericFunction)printVertexAttrib4fvARB },
    { "VertexAttrib4ivARB", (SPUGenericFunction)printVertexAttrib4ivARB },
    { "VertexAttrib4sARB", (SPUGenericFunction)printVertexAttrib4sARB },
    { "VertexAttrib4svARB", (SPUGenericFunction)printVertexAttrib4svARB },
    { "VertexAttrib4ubvARB", (SPUGenericFunction)printVertexAttrib4ubvARB },
    { "VertexAttrib4uivARB", (SPUGenericFunction)printVertexAttrib4uivARB },
    { "VertexAttrib4usvARB", (SPUGenericFunction)printVertexAttrib4usvARB },
    { "VertexAttribPointerARB", (SPUGenericFunction)printVertexAttribPointerARB },
    { "VertexAttribPointerNV", (SPUGenericFunction)printVertexAttribPointerNV },
    { "VertexAttribs1dvNV", (SPUGenericFunction)printVertexAttribs1dvNV },
    { "VertexAttribs1fvNV", (SPUGenericFunction)printVertexAttribs1fvNV },
    { "VertexAttribs1svNV", (SPUGenericFunction)printVertexAttribs1svNV },
    { "VertexAttribs2dvNV", (SPUGenericFunction)printVertexAttribs2dvNV },
    { "VertexAttribs2fvNV", (SPUGenericFunction)printVertexAttribs2fvNV },
    { "VertexAttribs2svNV", (SPUGenericFunction)printVertexAttribs2svNV },
    { "VertexAttribs3dvNV", (SPUGenericFunction)printVertexAttribs3dvNV },
    { "VertexAttribs3fvNV", (SPUGenericFunction)printVertexAttribs3fvNV },
    { "VertexAttribs3svNV", (SPUGenericFunction)printVertexAttribs3svNV },
    { "VertexAttribs4dvNV", (SPUGenericFunction)printVertexAttribs4dvNV },
    { "VertexAttribs4fvNV", (SPUGenericFunction)printVertexAttribs4fvNV },
    { "VertexAttribs4svNV", (SPUGenericFunction)printVertexAttribs4svNV },
    { "VertexAttribs4ubvNV", (SPUGenericFunction)printVertexAttribs4ubvNV },
    { "VertexPointer", (SPUGenericFunction)printVertexPointer },
    { "Viewport", (SPUGenericFunction)printViewport },
    { "WindowCreate", (SPUGenericFunction)printWindowCreate },
    { "WindowDestroy", (SPUGenericFunction)printWindowDestroy },
    { "WindowPos2dARB", (SPUGenericFunction)printWindowPos2dARB },
    { "WindowPos2dvARB", (SPUGenericFunction)printWindowPos2dvARB },
    { "WindowPos2fARB", (SPUGenericFunction)printWindowPos2fARB },
    { "WindowPos2fvARB", (SPUGenericFunction)printWindowPos2fvARB },
    { "WindowPos2iARB", (SPUGenericFunction)printWindowPos2iARB },
    { "WindowPos2ivARB", (SPUGenericFunction)printWindowPos2ivARB },
    { "WindowPos2sARB", (SPUGenericFunction)printWindowPos2sARB },
    { "WindowPos2svARB", (SPUGenericFunction)printWindowPos2svARB },
    { "WindowPos3dARB", (SPUGenericFunction)printWindowPos3dARB },
    { "WindowPos3dvARB", (SPUGenericFunction)printWindowPos3dvARB },
    { "WindowPos3fARB", (SPUGenericFunction)printWindowPos3fARB },
    { "WindowPos3fvARB", (SPUGenericFunction)printWindowPos3fvARB },
    { "WindowPos3iARB", (SPUGenericFunction)printWindowPos3iARB },
    { "WindowPos3ivARB", (SPUGenericFunction)printWindowPos3ivARB },
    { "WindowPos3sARB", (SPUGenericFunction)printWindowPos3sARB },
    { "WindowPos3svARB", (SPUGenericFunction)printWindowPos3svARB },
    { "WindowPosition", (SPUGenericFunction)printWindowPosition },
    { "WindowShow", (SPUGenericFunction)printWindowShow },
    { "WindowSize", (SPUGenericFunction)printWindowSize },
    { "Writeback", (SPUGenericFunction)printWriteback },
    { "ZPixCR", (SPUGenericFunction)printZPixCR },
    { NULL, NULL }
};
