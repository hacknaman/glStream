/* Copyright (c) 2001, Stanford University
 * All rights reserved.
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#ifndef CR_VERSION_H
#define CR_VERSION_H


/* These define the Chromium release number.
 * Alpha Release = 0.1.0, Beta Release = 0.2.0
 */
#define CR_MAJOR_VERSION 1
#define CR_MINOR_VERSION 1
#define CR_PATCH_VERSION 1


/* These define the OpenGL version that Chromium supports.
 * This lets users easily recompile Chromium with/without OpenGL 1.x support.
 * We use OpenGL's GL_VERSION_1_x convention.
 */
#define CR_OPENGL_VERSION_1_0 1
#define CR_OPENGL_VERSION_1_1 1
#define CR_OPENGL_VERSION_1_2 1
#define CR_OPENGL_VERSION_1_3 1
#define CR_OPENGL_VERSION_1_4 1


/* These define the OpenGL extensions that Chromium supports.
 * Users can enable/disable support for particular OpenGL extensions here.
 * Again, use OpenGL's convention.
 * WARNING: if you add new extensions here, also update this file:
 * state_tracker/state_limits.c
 */

/*#define CR_ARB_imaging 1    not yet */
#define CR_ARB_depth_texture 1
#define CR_ARB_multitexture 1
#define CR_ARB_multisample 1
#define CR_ARB_point_parameters 1
#define CR_ARB_shadow 1
#define CR_ARB_shadow_ambient 1
#define CR_ARB_texture_border_clamp 1
#define CR_ARB_texture_compression 1
#define CR_ARB_texture_cube_map 1
#define CR_ARB_texture_env_add 1
#define CR_ARB_texture_env_combine 1
#define CR_ARB_texture_env_crossbar 1
#define CR_ARB_texture_env_dot3 1
#define CR_ARB_texture_mirrored_repeat 1
#define CR_ARB_transpose_matrix 1
#define CR_ARB_window_pos 1

#define CR_EXT_blend_color 1
#define CR_EXT_blend_minmax 1
#define CR_EXT_blend_logic_op 1
#define CR_EXT_blend_subtract 1
#define CR_EXT_blend_func_separate 1
#define CR_EXT_clip_volume_hint 1
#define CR_EXT_fog_coord 1
#define CR_EXT_multi_draw_arrays 1
#define CR_EXT_secondary_color 1
#ifndef CR_OPENGL_VERSION_1_2
#define CR_EXT_separate_specular_color 1
#endif
#define CR_EXT_stencil_wrap 1
#define CR_EXT_texture_cube_map 1
#define CR_EXT_texture_edge_clamp 1
#define CR_EXT_texture_env_add 1
#define CR_EXT_texture_filter_anisotropic 1
#define CR_EXT_texture_lod_bias 1
#define CR_EXT_texture_object 1
#define CR_EXT_texture3D 1

#define CR_NV_fog_distance 1
#define CR_NV_register_combiners 1
#define CR_NV_register_combiners2 1
#define CR_NV_texgen_reflection 1
/*#define CR_NV_vertex_program 1    not yet */

#define CR_SGIS_texture_border_clamp 1
#define CR_SGIS_texture_edge_clamp 1
#define CR_SGIS_generate_mipmap 1

#endif /* CR_VERSION_H */
