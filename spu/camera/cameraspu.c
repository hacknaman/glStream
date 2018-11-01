/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include <stdio.h>
#include <conio.h>
#include "cr_spu.h"
#include "cameraspu.h"

#define IS_TEST false;


static void CAMERASPU_APIENTRY cameraColor3f(GLfloat red,
    GLfloat green,
    GLfloat blue)
{
#ifdef IS_TEST
    camera_spu.super.Color3f(1 - red, 1 - green, 1 - blue);
#else
	camera_spu.super.Color3f(red, green, blue);
#endif
}

static void CAMERASPU_APIENTRY cameraClearColor(GLfloat red,
    GLfloat green,
    GLfloat blue,
    GLfloat alpha)
{
}

static void CAMERASPU_APIENTRY cameraMaterialfv(GLenum face,
    GLenum mode,
    const GLfloat *param)
{
    if (mode != GL_SHININESS)
    {
        GLfloat local_param[4];
        local_param[0] = 1 - param[0];
        local_param[1] = 1 - param[1];
        local_param[2] = 1 - param[2];
        local_param[3] = 1 - param[3];

        camera_spu.super.Materialfv(face, mode, local_param);
    }
    else
    {
        camera_spu.super.Materialfv(face, mode, param);
    }
}
static void CAMERASPU_APIENTRY cameraMakeCurrent(GLint crWindow, GLint nativeWindow, GLint ctx)
{
    static GLfloat diffuse[4] = { 1 - 0.8f, 1 - 0.8f, 1 - 0.8f, 1.0f };
    camera_spu.super.MakeCurrent(crWindow, nativeWindow, ctx);
    camera_spu.super.Color3f(0, 0, 0); // default color is white
    camera_spu.super.ClearColor(1, 1, 1, 1); // default clear color is black
    camera_spu.super.Materialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
}

static void CAMERASPU_APIENTRY cameraFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{

   // zNear+=5;
    camera_spu.super.Frustum(left, right, bottom, top, zNear, zFar);
}


static void CAMERASPU_APIENTRY cameraSwapBuffers(GLint window, GLint flags)
{
    if (GetKeyState('A') & 0x8000/*check if high-order bit is set (1 << 15)*/)
    {
        camera_spu.super.Translated(-0.1, 0, 0);
    }
    if (GetKeyState('D') & 0x8000)
    {
        camera_spu.super.Translated(0.1, 0, 0);
    }
    if (GetKeyState('W') & 0x8000)
    {
        camera_spu.super.Translated(0, 0.1, 0);
    }
    if (GetKeyState('S') & 0x8000)
    {
        camera_spu.super.Translated(0, -0.1, 0);
    }
    if (GetKeyState('F') & 0x8000)
    {
        camera_spu.super.Translated(0, 0, 0.1);
    }
    if (GetKeyState('G') & 0x8000)
    {
        camera_spu.super.Translated(0, 0, -0.1);
    }
    if (GetKeyState('V') & 0x8000)
    {
        camera_spu.super.Rotated(5, 1, 0, 0);
    }
    if (GetKeyState('B') & 0x8000)
    {
        camera_spu.super.Rotated(5, 0, 1, 0);
    }
    if (GetKeyState('N') & 0x8000)
    {
        camera_spu.super.Rotated(5, 0, 0, 1);
    }
    camera_spu.super.SwapBuffers(window, flags);
}

/**
 * SPU function table
 */
SPUNamedFunctionTable _cr_camera_table[] = {
    { "Color3f", (SPUGenericFunction) cameraColor3f },
    { "ClearColor", (SPUGenericFunction) cameraClearColor },
    { "Materialfv", (SPUGenericFunction) cameraMaterialfv },
    { "MakeCurrent", (SPUGenericFunction)cameraMakeCurrent },
    { "Frustum", (SPUGenericFunction)cameraFrustum },
    { "SwapBuffers", (SPUGenericFunction)cameraSwapBuffers },
	{ NULL, NULL }
};
