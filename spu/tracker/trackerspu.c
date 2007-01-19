/* 
 * Copyright (c) 2006  Michael Duerig  
 * Bern University of Applied Sciences
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 */

/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include <stdio.h>
#include "cr_spu.h"
#include "trackerspu.h"
#include "chromium.h"
#include "cr_matrix.h"

static void TrackerUpdatePosition() {
  GLfloat view[18];
  CRmatrix *pv = (CRmatrix *) &view[2];

  GLfloat proj[18];
  CRmatrix *pp = (CRmatrix *) &proj[2];

  int c;

  // If there is tracker data available then
  if (tracker_spu.hasNewPos) {
    tracker_spu.hasNewPos = 0;

    // for each screen
    for(c = 0; c < tracker_spu.screenCount; c++) {
      crScreen *scr = &tracker_spu.screens[c];
      GLvectorf v = scr->eye == LEFT? tracker_spu.currentPos->left : tracker_spu.currentPos->right;

	    // Apply sccreen specific offset
      v.x += scr->Offset.x;
      v.y += scr->Offset.y;
      v.z += scr->Offset.z;

      // calculate new view matrix 
      { 
        *pv = scr->orientation;
        crMatrixTranslate(pv, -v.x, -v.y, -v.z);

        // and apply it on the server.
        view[0] = (GLfloat)scr->serverIndex;
        view[1] = LEFT;  // TODO: left eye. atm no support for OpenGL stereo
        tracker_spu.super.ChromiumParametervCR(
           (GLenum )GL_SERVER_VIEW_MATRIX_CR, (GLenum )GL_FLOAT, (GLsizei)18, (const GLvoid *)view);
      }

      // calculate new projection matrix
      {  
        float f = scr->zfar;
        float n = scr->znear;
        float w = scr->width;
        float h = scr->height;
        
        crMatrixTransformPointf(&scr->openGL2Screen, &v);

        // The following code is equivalent to directly calculating the projection matrix. 
        //
        // left   = -scr->znear/(2*v.z)*(scr->width  + 2*v.x);
        // right  =  scr->znear/(2*v.z)*(scr->width  - 2*v.x);
        // bottom = -scr->znear/(2*v.z)*(scr->height + 2*v.y);
        // top    =  scr->znear/(2*v.z)*(scr->height - 2*v.y);
        //
        // crMatrixInit(pp); 
        // crMatrixFrustum(pp, left, right, bottom, top, scr->znear , scr->zfar);

        pp->m00 = 2.0f*v.z/w;   pp->m10 = 0.0f;         pp->m20 = -2.0f*v.x/w;         pp->m30 = 0.0f;
        pp->m01 = 0.0f;         pp->m11 = 2.0f*v.z/h;   pp->m21 = -2.0f*v.y/h;         pp->m31 = 0.0f;
        pp->m02 = 0.0f;         pp->m12 = 0.0f;         pp->m22 = (f + n)/(n - f);     pp->m32 = 2.0f*f*n/(n - f);
        pp->m03 = 0.0f;         pp->m13 = 0.0f;         pp->m23 = -1.0f;               pp->m33 = 0.0f;

        // and apply it on the server.
        proj[0] = (GLfloat)scr->serverIndex;
        proj[1] = LEFT;  // TODO: left eye. atm no support for OpenGL stereo
        tracker_spu.super.ChromiumParametervCR(
           (GLenum )GL_SERVER_PROJECTION_MATRIX_CR, (GLenum )GL_FLOAT, (GLsizei)18, (const GLvoid *)proj);
      }

    }
  }
}

static void TRACKERSPU_APIENTRY TrackerSwapBuffers( GLint window, GLint flags ) {
  TrackerUpdatePosition();
  tracker_spu.super.SwapBuffers(window, flags); 
}

/**
 * SPU function table
 */
SPUNamedFunctionTable _cr_tracker_table[] = {
  { "SwapBuffers", (SPUGenericFunction) TrackerSwapBuffers },
	{ NULL, NULL }
};
