/* Copyright (c) 2001, Stanford University
 * All rights reserved.
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#ifndef CR_STATE_FOG_H 
#define CR_STATE_FOG_H 

#include "cr_glwrapper.h"
#include "state/cr_statetypes.h"
#include "state/cr_extensions.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	GLbitvalue dirty;
	GLbitvalue color;
	GLbitvalue index;
	GLbitvalue density;
	GLbitvalue start;
	GLbitvalue end;
	GLbitvalue mode;
	GLbitvalue enable;
	GLbitvalue extensions;
} CRFogBits;

typedef struct {
	GLcolorf  color;
	GLint     index;
	GLfloat   density;
	GLfloat   start;
	GLfloat   end;
	GLint     mode;
	GLboolean enable;
	CRFogStateExtensions extensions;
} CRFogState;

void crStateFogInitBits (CRFogBits *fb);
void crStateFogInit(CRFogState *f);

void crStateFogDiff(CRFogBits *bb, GLbitvalue bitID, 
		CRFogState *from, CRFogState *to);
void crStateFogSwitch(CRFogBits *bb, GLbitvalue bitID, 
		CRFogState *from, CRFogState *to);

#ifdef __cplusplus
}
#endif

#endif /* CR_STATE_FOG_H */
