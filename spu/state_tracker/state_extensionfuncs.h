#ifndef CR_STATE_EXTENSION_FUNCS_H
#define CR_STATE_EXTENSION_FUNCS_H

#include "cr_glstate.h"

GLboolean crEnableSetExtensions( CRContext *g, CRStateBits *sb, GLbitvalue neg_bitid, GLenum cap, GLboolean val );

void crStateTextureInitExtensions( CRTextureState *t );
void crStateTextureInitTextureObjExtensions( CRTextureState *t, CRTextureObj *tobj );
int crStateTexImage2DTargetExtensions( GLenum target );
int crStateTexSubImage2DTargetExtensions( GLenum target );
int crStateBindTextureExtensions( GLenum target, GLuint texture, CRTextureState *t, CRTextureObj *tobj );
int crStateTexParameterfvExtensions( CRTextureState *t, CRTextureObj *tobj, GLenum pname, const GLfloat *param );
int crStateTexParameterivExtensions( GLenum target, GLenum pname, const GLint *param );
int crStateGetTexParameterfvExtensions( CRTextureObj *tobj, GLenum pname, GLfloat *params );
int crStateGetTexParameterivExtensions( CRTextureObj *tobj, GLenum pname, GLint *params );
int crStateTexGenTextureGenModeExtensions( GLenum coord, GLenum param );
void crStateTextureDiffParameterExtensions( CRTextureObj *tobj );

void crStateFogInitExtensions( CRFogState *f );
int crStateFogfvExtensions( CRFogState *f, GLenum pname, const GLfloat *params );
int crStateFogivExtensions( GLenum pname, const GLint *params );
void crStateFogDiffExtensions( CRFogState *from, CRFogState *to );
void crStateFogSwitchExtensions( CRFogState *from, CRFogState *to );

void crStateBufferInitExtensions( CRBufferState *b );
int crStateBlendFuncExtensionsCheckFactor( GLenum factor );
void crStateBufferDiffExtensions( CRBufferState *from, CRBufferState *to );
void crStateBufferSwitchExtensions( CRBufferState *from, CRBufferState *to );

#endif /* CR_STATE_EXTENSION_FUNCS_H */
