/* Copyright (c) 2001, Stanford University
 * All rights reserved.
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#ifndef CR_STATE_ATTRIB_H 
#define CR_STATE_ATTRIB_H 

#include "cr_glwrapper.h"
#include "state/cr_limits.h"
#include "state/cr_statetypes.h"
#include "state/cr_extensions.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	GLbitvalue dirty;
} CRAttribBits;

typedef struct {
	GLcolorf accumClearValue;
} CRAccumBufferStack;

typedef struct {
	GLboolean	blend;
	GLboolean	alphaTest;
	GLboolean	logicOp;
	GLboolean	dither;

	GLenum		alphaTestFunc;
	GLfloat		alphaTestRef;
	GLenum		blendSrc;
	GLenum		blendDst;
	GLcolorf	blendColor;	
	GLenum    blendEquation;
	GLenum		logicOpMode;
	GLenum		drawBuffer;
	GLint     indexWriteMask;
	GLcolorb	colorWriteMask;
	GLcolorf	colorClearValue;
	GLfloat 	indexClearValue;
} CRColorBufferStack;

typedef struct {
	GLcolorf     color;
	GLfloat      index;
	GLtexcoordf  texCoord[CR_MAX_TEXTURE_UNITS];
	GLvectorf    normal;
	GLboolean    edgeFlag;

	GLvectorf    rasterPos;
	GLcolorf     rasterColor;
	GLtexcoordf  rasterTexture;
	GLdouble     rasterIndex;
	GLboolean    rasterValid;
} CRCurrentStack;

typedef struct {
	GLboolean	depthTest;
	GLboolean depthMask;
	GLenum		depthFunc;
	GLdefault	depthClearValue;
} CRDepthBufferStack;

typedef struct {
	GLboolean alphaTest;
	GLboolean autoNormal;
	GLboolean blend;
	GLboolean *clip;
	GLboolean colorMaterial;
	GLboolean cullFace;
	GLboolean depthTest;
	GLboolean dither;
	GLboolean fog;
	GLboolean *light;
	GLboolean lighting;
	GLboolean lineSmooth;
	GLboolean lineStipple;
	GLboolean logicOp;
	GLboolean indexLogicOp;
	GLboolean map1[GLEVAL_TOT];
	GLboolean map2[GLEVAL_TOT];
	GLboolean normalize;
	GLboolean pointSmooth;
	GLboolean polygonOffsetLine;
	GLboolean polygonOffsetFill;
	GLboolean polygonOffsetPoint;
	GLboolean polygonSmooth;
	GLboolean polygonStipple;
	GLboolean scissorTest;
	GLboolean stencilTest;
	GLboolean texture1D[CR_MAX_TEXTURE_UNITS];
	GLboolean texture2D[CR_MAX_TEXTURE_UNITS];
	GLboolean texture3D[CR_MAX_TEXTURE_UNITS];
#ifdef CR_ARB_texture_cube_map
	GLboolean textureCubeMap[CR_MAX_TEXTURE_UNITS];
#endif
	GLboolean textureGenS[CR_MAX_TEXTURE_UNITS];
	GLboolean textureGenT[CR_MAX_TEXTURE_UNITS];
	GLboolean textureGenR[CR_MAX_TEXTURE_UNITS];
	GLboolean textureGenQ[CR_MAX_TEXTURE_UNITS];
} CREnableStack;

typedef struct {
	GLboolean map1[GLEVAL_TOT];
	GLboolean map2[GLEVAL_TOT];
	GLboolean autoNormal;
} CREvalStack;

typedef struct {
	GLboolean	lighting;
	GLenum		shadeModel;
	GLcolorf	ambient[2];
	GLcolorf	diffuse[2];
	GLcolorf	specular[2];
	GLcolorf	emission[2];
	GLfloat		shininess[2];
	GLcolorf	lightModelAmbient;
	GLboolean	lightModelLocalViewer;
	GLboolean	lightModelTwoSide;
	CRLight		*light;
} CRLightingStack;

typedef struct {
	GLcolorf  color;
	GLint     index;
	GLfloat   density;
	GLfloat   start;
	GLfloat   end;
	GLint     mode;
	GLboolean enable;
} CRFogStack;

typedef struct {
	GLboolean	lineSmooth;
	GLboolean	lineStipple;
	GLfloat		width;
	GLushort	pattern;
	GLint		repeat;
} CRLineStack;

typedef struct {
	GLuint base;
} CRListStack;

typedef struct {
	GLboolean   mapColor;
	GLboolean   mapStencil;
	GLint		    indexShift;
	GLint		    indexOffset;
	GLcolorf	  scale;
	GLfloat		  depthScale;
	GLcolorf	  bias;
	GLfloat		  depthBias;
	GLfloat		  xZoom;
	GLfloat		  yZoom;
	GLenum      readBuffer;
} CRPixelModeStack;

typedef struct {
	GLboolean pointSmooth;
	GLfloat pointSize;
} CRPointStack;

typedef struct {
	GLboolean	polygonSmooth;
	GLboolean polygonOffsetFill;
	GLboolean polygonOffsetLine;
	GLboolean polygonOffsetPoint;
	GLboolean	polygonStipple;
	GLboolean cullFace;
	GLfloat		offsetFactor;
	GLfloat		offsetUnits;
	GLenum		cullFaceMode;
	GLenum		frontFace;
	GLenum		frontMode;
	GLenum		backMode;
} CRPolygonStack;

typedef struct {
	GLint pattern[32];
} CRPolygonStippleStack;

typedef struct {
	GLboolean	scissorTest;
	GLint		scissorX;
	GLint		scissorY;
	GLsizei		scissorW;
	GLsizei		scissorH;
} CRScissorStack;

typedef struct {
	GLboolean	stencilTest;
	GLenum		func;
	GLint		mask;
	GLint		ref;
	GLenum		fail;
	GLenum		passDepthFail;
	GLenum		passDepthPass;
	GLint		clearValue;
	GLint		writeMask;
} CRStencilBufferStack;

typedef struct {
	GLboolean	enabled1D[CR_MAX_TEXTURE_UNITS];
	GLboolean	enabled2D[CR_MAX_TEXTURE_UNITS];
	GLboolean	enabled3D[CR_MAX_TEXTURE_UNITS];
#ifdef CR_ARB_texture_cube_map
	GLboolean	enabledCubeMap[CR_MAX_TEXTURE_UNITS];
#endif
	GLcolorf borderColor[4];  /* 4 = 1D, 2D, 3D and cube map textures */
	GLenum minFilter[4];
	GLenum magFilter[4];
	GLenum wrapS[4];
	GLenum wrapT[4];
#ifdef CR_OPENGL_VERSION_1_2
	GLenum wrapR[4];
#endif
	GLtexcoordb	textureGen[CR_MAX_TEXTURE_UNITS];
	GLvectorf	objSCoeff[CR_MAX_TEXTURE_UNITS];
	GLvectorf	objTCoeff[CR_MAX_TEXTURE_UNITS];
	GLvectorf	objRCoeff[CR_MAX_TEXTURE_UNITS];
	GLvectorf	objQCoeff[CR_MAX_TEXTURE_UNITS];
	GLvectorf	eyeSCoeff[CR_MAX_TEXTURE_UNITS];
	GLvectorf	eyeTCoeff[CR_MAX_TEXTURE_UNITS];
	GLvectorf	eyeRCoeff[CR_MAX_TEXTURE_UNITS];
	GLvectorf	eyeQCoeff[CR_MAX_TEXTURE_UNITS];
	GLtexcoorde	gen[CR_MAX_TEXTURE_UNITS];
} CRTextureStack;

typedef struct {
	GLenum    mode;
	GLvectord  *clipPlane;
	GLboolean  *clip;
	GLboolean normalize;
} CRTransformStack;

typedef struct {
	GLint viewportX;
	GLint viewportY;
	GLint viewportW;
	GLint viewportH;
	GLclampd nearClip;
	GLclampd farClip;
} CRViewportStack;

typedef struct {
	GLint attribStackDepth;
	GLbitvalue pushMaskStack[CR_MAX_ATTRIB_STACK_DEPTH];

	GLint accumBufferStackDepth;
	CRAccumBufferStack accumBufferStack[CR_MAX_ATTRIB_STACK_DEPTH];

	GLint colorBufferStackDepth;
	CRColorBufferStack colorBufferStack[CR_MAX_ATTRIB_STACK_DEPTH];

	GLint currentStackDepth;
	CRCurrentStack currentStack[CR_MAX_ATTRIB_STACK_DEPTH];

	GLint depthBufferStackDepth;
	CRDepthBufferStack depthBufferStack[CR_MAX_ATTRIB_STACK_DEPTH];

	GLint enableStackDepth;
	CREnableStack enableStack[CR_MAX_ATTRIB_STACK_DEPTH];

	GLint evalStackDepth;
	CREvalStack evalStack[CR_MAX_ATTRIB_STACK_DEPTH];

	GLint fogStackDepth;
	CRFogStack fogStack[CR_MAX_ATTRIB_STACK_DEPTH];

	GLint lightingStackDepth;
	CRLightingStack lightingStack[CR_MAX_ATTRIB_STACK_DEPTH];

	GLint lineStackDepth;
	CRLineStack lineStack[CR_MAX_ATTRIB_STACK_DEPTH];

	GLint listStackDepth;
	CRListStack listStack[CR_MAX_ATTRIB_STACK_DEPTH];

	GLint pixelModeStackDepth;
	CRPixelModeStack pixelModeStack[CR_MAX_ATTRIB_STACK_DEPTH];

	GLint pointStackDepth;
	CRPointStack pointStack[CR_MAX_ATTRIB_STACK_DEPTH];

	GLint polygonStackDepth;
	CRPolygonStack polygonStack[CR_MAX_ATTRIB_STACK_DEPTH];

	GLint polygonStippleStackDepth;
	CRPolygonStippleStack polygonStippleStack[CR_MAX_ATTRIB_STACK_DEPTH];

	GLint scissorStackDepth;
	CRScissorStack scissorStack[CR_MAX_ATTRIB_STACK_DEPTH];

	GLint stencilBufferStackDepth;
	CRStencilBufferStack stencilBufferStack[CR_MAX_ATTRIB_STACK_DEPTH];

	GLint textureStackDepth;
	CRTextureStack textureStack[CR_MAX_ATTRIB_STACK_DEPTH];

	GLint transformStackDepth;
	CRTransformStack transformStack[CR_MAX_ATTRIB_STACK_DEPTH];

	GLint viewportStackDepth;
	CRViewportStack viewportStack[CR_MAX_ATTRIB_STACK_DEPTH];
} CRAttribState;

void crStateAttribInit(CRAttribState *a);

/* No diff! */
void crStateAttribSwitch(CRAttribBits *bb, GLbitvalue bitID, 
		CRAttribState *from, CRAttribState *to);

#ifdef __cplusplus
}
#endif

#endif /* CR_STATE_ATTRIB_H */
