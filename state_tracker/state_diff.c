/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "state.h"
#include "cr_error.h"
#include <stdio.h>

void crStateDiffContext( CRContext *from, CRContext *to )
{
	CRContext *g = GetCurrentContext();
	GLbitvalue *bitID = from->bitid;
	CRStateBits *sb = GetCurrentBits();

	/*crDebug( "Diffing two contexts!" ); */

	if (CHECKDIRTY(sb->transform.dirty, bitID))
	{
		crStateTransformDiff (to->limits.maxTextureUnits, &(sb->transform), bitID,
							 &(from->transform), &(to->transform));
	}
	if (CHECKDIRTY(sb->pixel.dirty, bitID))
	{
		crStatePixelDiff	(&(sb->pixel), bitID,
							 &(from->pixel), &(to->pixel));
	}
	if (CHECKDIRTY(sb->viewport.dirty, bitID))
	{
		crStateViewportDiff	(&(sb->viewport), bitID,
								 &(from->viewport), &(to->viewport));
	}
	if (CHECKDIRTY(sb->fog.dirty, bitID))
	{
		crStateFogDiff	(&(sb->fog), bitID,
							 &(from->fog), &(to->fog));
	}
	if (CHECKDIRTY(sb->texture.dirty, bitID))
	{
		crStateTextureDiff	(from, &(sb->texture), bitID,
							 &(from->texture), &(to->texture));
	}
	if (CHECKDIRTY(sb->lists.dirty, bitID))
	{
		crStateListsDiff	(&(sb->lists), bitID,
							 &(from->lists), &(to->lists));
	}
#if 0
	if (CHECKDIRTY(sb->client.dirty, bitID))
	{
		crStateClientDiff	(&(sb->client), bitID,
							 &(from->client), &(to->client));
	}
#endif
	if (CHECKDIRTY(sb->buffer.dirty, bitID))
	{
		crStateBufferDiff	(&(sb->buffer), bitID,
							 &(from->buffer), &(to->buffer));
	}
	if (CHECKDIRTY(sb->hint.dirty, bitID))
	{
		crStateHintDiff	(&(sb->hint), bitID,
							 &(from->hint), &(to->hint));
	}
	if (CHECKDIRTY(sb->lighting.dirty, bitID))
	{
		crStateLightingDiff	(&(sb->lighting), bitID,
								 &(from->lighting), &(to->lighting));
	}
	if (CHECKDIRTY(sb->line.dirty, bitID))
	{
		crStateLineDiff	(&(sb->line), bitID,
							 &(from->line), &(to->line));
	}
	if (CHECKDIRTY(sb->point.dirty, bitID))
	{
		crStatePointDiff (&(sb->point), bitID,
							 &(from->point), &(to->point));
	}
	if (CHECKDIRTY(sb->polygon.dirty, bitID))
	{
		crStatePolygonDiff	(&(sb->polygon), bitID,
							 &(from->polygon), &(to->polygon));
	}
	if (CHECKDIRTY(sb->stencil.dirty, bitID))
	{
		crStateStencilDiff	(&(sb->stencil), bitID,
							 &(from->stencil), &(to->stencil));
	}
	if (CHECKDIRTY(sb->eval.dirty, bitID))
	{
		crStateEvaluatorDiff	(&(sb->eval), bitID,
							 &(from->eval), &(to->eval));
	}
#ifdef CR_ARB_imaging
	if (CHECKDIRTY(sb->imaging.dirty, bitID))
	{
		crStateImagingDiff	(&(sb->imaging), bitID,
							 &(from->imaging), &(to->imaging));
	}
	if (CHECKDIRTY(sb->selection.dirty, bitID))
	{
		crStateSelectionDiff	(&(sb->selection), bitID,
								 &(from->selection), &(to->selection));
	}
#endif
#ifdef CR_NV_register_combiners
	if (CHECKDIRTY(sb->regcombiner.dirty, bitID) && g->extensions.NV_register_combiners)
	{
		crStateRegCombinerDiff	(&(sb->regcombiner), bitID,
								 &(from->regcombiner), &(to->regcombiner));
	}
#endif

	if (CHECKDIRTY(sb->current.dirty, bitID))
	{
		crStateCurrentDiff	(&(sb->current), bitID,
							 &(from->current), &(to->current));
	}
}

void crStateSwitchContext( CRContext *from, CRContext *to )
{
	CRContext *g = GetCurrentContext();
	GLbitvalue *bitID = to->bitid;
	CRStateBits *sb = GetCurrentBits();

	if (CHECKDIRTY(sb->attrib.dirty, bitID))
	{
		crStateAttribSwitch (&(sb->attrib), bitID,
							 &(from->attrib), &(to->attrib));
	}
	if (CHECKDIRTY(sb->transform.dirty, bitID))
	{
		crStateTransformSwitch (to->limits.maxTextureUnits, &(sb->transform), bitID,
							 &(from->transform), &(to->transform));
	}
	if (CHECKDIRTY(sb->pixel.dirty, bitID))
	{
		crStatePixelSwitch	(&(sb->pixel), bitID,
							 &(from->pixel), &(to->pixel));
	}
	if (CHECKDIRTY(sb->viewport.dirty, bitID))
	{
		crStateViewportSwitch	(&(sb->viewport), bitID,
								 &(from->viewport), &(to->viewport));
	}
	if (CHECKDIRTY(sb->fog.dirty, bitID))
	{
		crStateFogSwitch	(&(sb->fog), bitID,
							 &(from->fog), &(to->fog));
	}
	if (CHECKDIRTY(sb->texture.dirty, bitID))
	{
		crStateTextureSwitch	(from, &(sb->texture), bitID,
							 &(from->texture), &(to->texture));
	}
	if (CHECKDIRTY(sb->lists.dirty, bitID))
	{
		crStateListsSwitch	(&(sb->lists), bitID,
							 &(from->lists), &(to->lists));
	}
#if 0
	if (CHECKDIRTY(sb->client.dirty, bitID))
	{
		crStateClientSwitch	(&(sb->client), bitID,
							 &(from->client), &(to->client));
	}
#endif
	if (CHECKDIRTY(sb->buffer.dirty, bitID))
	{
		crStateBufferSwitch	(&(sb->buffer), bitID,
							 &(from->buffer), &(to->buffer));
	}
#if 0
	if (CHECKDIRTY(sb->hint.dirty, bitID))
	{
		crStateHintSwitch	(&(sb->hint), bitID,
							 &(from->hint), &(to->hint));
	}
#endif
	if (CHECKDIRTY(sb->lighting.dirty, bitID))
	{
		crStateLightingSwitch	(&(sb->lighting), bitID,
								 &(from->lighting), &(to->lighting));
	}
	if (CHECKDIRTY(sb->line.dirty, bitID))
	{
		crStateLineSwitch	(&(sb->line), bitID,
							 &(from->line), &(to->line));
	}
	if (CHECKDIRTY(sb->point.dirty, bitID))
	{
		crStatePointSwitch	(&(sb->point), bitID,
							 &(from->point), &(to->point));
	}
	if (CHECKDIRTY(sb->polygon.dirty, bitID))
	{
		crStatePolygonSwitch	(&(sb->polygon), bitID,
							 &(from->polygon), &(to->polygon));
	}
	if (CHECKDIRTY(sb->stencil.dirty, bitID))
	{
		crStateStencilSwitch	(&(sb->stencil), bitID,
							 &(from->stencil), &(to->stencil));
	}
	if (CHECKDIRTY(sb->eval.dirty, bitID))
	{
		crStateEvaluatorSwitch	(&(sb->eval), bitID,
							 &(from->eval), &(to->eval));
	}
#ifdef CR_ARB_imaging
	if (CHECKDIRTY(sb->imaging.dirty, bitID))
	{
		crStateImagingSwitch	(&(sb->imaging), bitID,
							 &(from->imaging), &(to->imaging));
	}
#endif
#if 0
	if (CHECKDIRTY(sb->selection.dirty, bitID))
	{
		crStateSelectionSwitch	(&(sb->selection), bitID,
								 &(from->selection), &(to->selection));
	}
#endif
#ifdef CR_NV_register_combiners
	if (CHECKDIRTY(sb->regcombiner.dirty, bitID) && g->extensions.NV_register_combiners)
	{
		crStateRegCombinerSwitch	(&(sb->regcombiner), bitID,
									 &(from->regcombiner), &(to->regcombiner));
	}
#endif
	if (CHECKDIRTY(sb->current.dirty, bitID))
	{
		crStateCurrentSwitch(to->limits.maxTextureUnits, &(sb->current), bitID,
							 &(from->current), &(to->current));
	}
}
