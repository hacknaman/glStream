#ifndef CR_STATE_EXTENSION_FUNCS_H
#define CR_STATE_EXTENSION_FUNCS_H

#include "cr_glstate.h"

void crStateTextureInitExtensions( CRTextureState *t );
void crStateTextureInitTextureObjExtensions( CRTextureState *t, CRTextureObj *tobj );
int crStateTexParameterfvExtensions( CRTextureState *t, CRTextureObj *tobj, GLenum pname, const GLfloat *param );
int crStateTexParameterivExtensions( GLenum target, GLenum pname, const GLint *param );
int crStateGetTexParameterfvExtensions( CRTextureObj *tobj, GLenum pname, GLfloat *params );
int crStateGetTexParameterivExtensions( CRTextureObj *tobj, GLenum pname, GLint *params );
void crStateTextureDiffParameterExtensions( CRTextureObj *tobj );

void crStateFogInitExtensions( CRFogState *f );
int crStateFogfvExtensions( CRFogState *f, GLenum pname, const GLfloat *params );
int crStateFogivExtensions( GLenum pname, const GLint *params );

#endif /* CR_STATE_EXTENSION_FUNCS_H */
