/* Copyright (c) 2001, Stanford University
 * All rights reserved.
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#ifndef CR_STATE_BUFFER_H
#define CR_STATE_BUFFER_H

#include "cr_glwrapper.h"
#include "state/cr_statetypes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	GLbitvalue	dirty[CR_MAX_BITARRAY];
	GLbitvalue	enable[CR_MAX_BITARRAY];
	GLbitvalue	alphaFunc[CR_MAX_BITARRAY];
	GLbitvalue	depthFunc[CR_MAX_BITARRAY];
	GLbitvalue	blendFunc[CR_MAX_BITARRAY];
	GLbitvalue	logicOp[CR_MAX_BITARRAY];
	GLbitvalue	indexLogicOp[CR_MAX_BITARRAY];
	GLbitvalue	drawBuffer[CR_MAX_BITARRAY];
	GLbitvalue	readBuffer[CR_MAX_BITARRAY];
	GLbitvalue	indexMask[CR_MAX_BITARRAY];
	GLbitvalue	colorWriteMask[CR_MAX_BITARRAY];
	GLbitvalue	clearColor[CR_MAX_BITARRAY];
	GLbitvalue	clearIndex[CR_MAX_BITARRAY];
	GLbitvalue	clearDepth[CR_MAX_BITARRAY];
	GLbitvalue	clearAccum[CR_MAX_BITARRAY];
	GLbitvalue	depthMask[CR_MAX_BITARRAY];
#ifdef CR_EXT_blend_color
	GLbitvalue	blendColor[CR_MAX_BITARRAY];
#endif
#if defined(CR_EXT_blend_minmax) || defined(CR_EXT_blend_subtract)
	GLbitvalue	blendEquation[CR_MAX_BITARRAY];
#endif
} CRBufferBits;

typedef struct {
	GLboolean	depthTest;
	GLboolean	blend;
	GLboolean	alphaTest;
	GLboolean	logicOp;
	GLboolean	indexLogicOp;
	GLboolean	dither;
	GLboolean	depthMask;

	GLenum		alphaTestFunc;
	GLfloat		alphaTestRef;
	GLenum		depthFunc;
	GLenum		blendSrc;
	GLenum		blendDst;
	GLenum		logicOpMode;
	GLenum		drawBuffer;
	GLenum		readBuffer;
	GLint		indexWriteMask;
	GLcolorb	colorWriteMask;
	GLcolorf	colorClearValue;
	GLfloat 	indexClearValue;
	GLdefault	depthClearValue;
	GLcolorf	accumClearValue;
#ifdef CR_EXT_blend_color
	GLcolorf	blendColor;
#endif
#if defined(CR_EXT_blend_minmax) || defined(CR_EXT_blend_subtract)
	GLenum		blendEquation;
#endif
} CRBufferState;

void crStateBufferInitBits(CRBufferBits *bb);
void crStateBufferInit(CRBufferState *b);

void crStateBufferDiff(CRBufferBits *bb, GLbitvalue *bitID, 
		CRBufferState *from, CRBufferState *to);
void crStateBufferSwitch(CRBufferBits *bb, GLbitvalue *bitID, 
		CRBufferState *from, CRBufferState *to);

#ifdef __cplusplus
}
#endif

#endif /* CR_STATE_BUFFER_H */
