/* Copyright (c) 2001, Stanford University
 * All rights reserved.
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#ifndef CR_STATE_LINE_H
#define SR_STATE_LINE_H

#include "state/cr_statetypes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	CRbitvalue enable[CR_MAX_BITARRAY];
	CRbitvalue size[CR_MAX_BITARRAY];
#ifdef CR_ARB_point_parameters
	CRbitvalue minSize[CR_MAX_BITARRAY];
	CRbitvalue maxSize[CR_MAX_BITARRAY];
	CRbitvalue fadeThresholdSize[CR_MAX_BITARRAY];
	CRbitvalue distanceAttenuation[CR_MAX_BITARRAY];
#endif
	CRbitvalue dirty[CR_MAX_BITARRAY];
} CRPointBits;

typedef struct {
	GLboolean	pointSmooth;
	GLfloat		pointSize;
#ifdef CR_ARB_point_parameters
	GLfloat		minSize, maxSize;
	GLfloat		fadeThresholdSize;
	GLfloat		distanceAttenuation[3];
#endif
} CRPointState;

void crStatePointInit (CRContext *ctx);

void crStatePointDiff(CRPointBits *bb, CRbitvalue *bitID,
                      CRContext *fromCtx, CRContext *toCtx);
void crStatePointSwitch(CRPointBits *bb, CRbitvalue *bitID, 
                        CRContext *fromCtx, CRContext *toCtx);

#ifdef __cplusplus
}
#endif

#endif /* CR_STATE_LINE_H */
