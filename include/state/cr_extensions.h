/* Copyright (c) 2001, Stanford University
 * All rights reserved.
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#ifndef CR_STATE_EXTENSIONS_H
#define CR_STATE_EXTENSIONS_H

#include "cr_glwrapper.h"
#include "state/cr_statetypes.h"

/* Booleans to indicate which OpenGL extensions are supported at runtime */
typedef struct {
	GLboolean ARB_imaging;
	GLboolean ARB_multitexture;
	GLboolean ARB_texture_cube_map; /* or EXT_texture_cube_map */
	GLboolean EXT_blend_color;
	GLboolean EXT_blend_minmax;
	GLboolean EXT_blend_subtract;
	GLboolean EXT_secondary_color;
	GLboolean EXT_separate_specular_color;
	GLboolean EXT_texture_edge_clamp; /* or SGIS_texture_edge_clamp */
	GLboolean EXT_texture_filter_anisotropic;
	GLboolean NV_fog_distance;
	GLboolean NV_register_combiners;
	GLboolean NV_register_combiners2;
	GLboolean NV_texgen_reflection;
} CRExtensionState;

void crStateExtensionsInit( CRContext *g );

#endif /* CR_STATE_EXTENSIONS_H */
