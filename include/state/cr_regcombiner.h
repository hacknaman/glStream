/* Copyright (c) 2001, Stanford University
 * All rights reserved.
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#ifndef CR_STATE_REGCOMBINER_H
#define CR_STATE_REGCOMBINER_H

#include "state/cr_statetypes.h"
#include "state/cr_limits.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	GLenum a,        b,        c,        d;
	GLenum aMapping, bMapping, cMapping, dMapping;
	GLenum aPortion, bPortion, cPortion, dPortion;
	GLenum scale, bias;
	GLenum    abOutput,     cdOutput,     sumOutput;
	GLboolean abDotProduct, cdDotProduct, muxSum;
} CRRegCombinerPortionState;

typedef struct {
	GLboolean enabledRegCombiners;
	GLboolean enabledPerStageConstants;

	GLcolorf constantColor0;
	GLcolorf constantColor1;
	GLcolorf stageConstantColor0[CR_MAX_GENERAL_COMBINERS];
	GLcolorf stageConstantColor1[CR_MAX_GENERAL_COMBINERS];
	GLboolean colorSumClamp;
	GLint numGeneralCombiners;

	CRRegCombinerPortionState rgb[CR_MAX_GENERAL_COMBINERS];
	CRRegCombinerPortionState alpha[CR_MAX_GENERAL_COMBINERS];

	GLenum a,        b,        c,        d,        e,        f,        g;
	GLenum aMapping, bMapping, cMapping, dMapping, eMapping, fMapping, gMapping;
	GLenum aPortion, bPortion, cPortion, dPortion, ePortion, fPortion, gPortion;
} CRRegCombinerState;

typedef struct {
	GLbitvalue dirty;
	GLbitvalue enable;
	GLbitvalue regCombinerVars; // numGeneralCombiners, colorSumClamp
	GLbitvalue regCombinerColor0;
	GLbitvalue regCombinerColor1;
	GLbitvalue regCombinerStageColor0;
	GLbitvalue regCombinerStageColor1;
	GLbitvalue regCombinerInput; // rgb/alpha[].a/b/c/d, .aMapping, .aPortion
	GLbitvalue regCombinerOutput; // rgb/alpha[].abOutput, .cdOutput, .sumOutput, .scale, .bias, .abDotProduct, .cdDotProduct, .muxSum
	GLbitvalue regCombinerFinalInput; // a/b/c/d/e/f/g, aMapping, aPortion
} CRRegCombinerBits;

void crStateRegCombinerInit( CRRegCombinerState *reg );

void crStateRegCombinerDiff( CRRegCombinerBits *b, GLbitvalue bitID, 
		CRRegCombinerState *from, CRRegCombinerState *to );
void crStateRegCombinerSwitch( CRRegCombinerBits *b, GLbitvalue bitID, 
		CRRegCombinerState *from, CRRegCombinerState *to );

#ifdef __cplusplus
}
#endif

#endif /* CR_STATE_REGCOMBINER_H */
