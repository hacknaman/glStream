import sys,os;
import cPickle;
import string;
import re;

sys.path.append( "../opengl_stub" )
parsed_file = open( "../glapi_parser/gl_header.parsed", "rb" )
gl_mapping = cPickle.load( parsed_file )

import stub_common;

print """#include "cr_spu.h"
#include "cr_glwrapper.h"
#include "cr_error.h"
#include "cr_mem.h"
#include "cr_net.h"
#include "server_dispatch.h"
#include "server.h"
"""

num_get_values = {
	'GL_ACCUM_ALPHA_BITS' : 1,
	'GL_ACCUM_BLUE_BITS' : 1,
	'GL_ACCUM_CLEAR_VALUE': 4,
	'GL_ACCUM_GREEN_BITS' : 1,
	'GL_ACCUM_RED_BITS' : 1,
	'GL_ALPHA_BIAS' : 1,
	'GL_ALPHA_BITS' : 1,
	'GL_ALPHA_SCALE' : 1,
	'GL_ALPHA_TEST' : 1,
	'GL_ALPHA_TEST_FUNC' : 1,
	'GL_ALPHA_TEST_REF' : 1,
	'GL_ATTRIB_STACK_DEPTH' : 1,
	'GL_AUTO_NORMAL' : 1,
	'GL_AUX_BUFFERS' : 1,
	'GL_BLEND' : 1,
	'GL_BLEND_DST' : 1,
	'GL_BLEND_SRC' : 1,
	'GL_BLUE_BIAS' : 1,
	'GL_BLUE_BITS' : 1,
	'GL_BLUE_SCALE' : 1,
	'GL_CLIENT_ATTRIB_STACK_DEPTH' : 1,
	'GL_CLIP_PLANE0' : 1,
	'GL_CLIP_PLANE1' : 1,
	'GL_CLIP_PLANE2' : 1,
	'GL_CLIP_PLANE3' : 1,
	'GL_CLIP_PLANE4' : 1,
	'GL_CLIP_PLANE5' : 1,
	'GL_COLOR_ARRAY' : 1,
	'GL_COLOR_ARRAY_SIZE' : 1,
	'GL_COLOR_ARRAY_STRIDE' : 1,
	'GL_COLOR_ARRAY_TYPE' : 1,
	'GL_COLOR_CLEAR_VALUE': 4,
	'GL_COLOR_LOGIC_OP' : 1,
	'GL_COLOR_MATERIAL' : 1,
	'GL_COLOR_MATERIAL_FACE' : 1,
	'GL_COLOR_MATERIAL_PARAMETER' : 1,
	'GL_COLOR_WRITEMASK': 4,
	'GL_CULL_FACE' : 1,
	'GL_CULL_FACE_MODE' : 1,
	'GL_CURRENT_COLOR': 4,
	'GL_CURRENT_INDEX' : 1,
	'GL_CURRENT_NORMAL': 3,
	'GL_CURRENT_RASTER_COLOR': 4,
	'GL_CURRENT_RASTER_DISTANCE' : 1,
	'GL_CURRENT_RASTER_INDEX' : 1,
	'GL_CURRENT_RASTER_POSITION': 4,
	'GL_CURRENT_RASTER_POSITION_VALID' : 1,
	'GL_CURRENT_RASTER_TEXTURE_COORDS': 4,
	'GL_CURRENT_TEXTURE_COORDS': 4,
	'GL_DEPTH_BIAS' : 1,
	'GL_DEPTH_BITS' : 1,
	'GL_DEPTH_CLEAR_VALUE' : 1,
	'GL_DEPTH_FUNC' : 1,
	'GL_DEPTH_RANGE': 2,
	'GL_DEPTH_SCALE' : 1,
	'GL_DEPTH_TEST' : 1,
	'GL_DEPTH_WRITEMASK' : 1,
	'GL_DITHER' : 1,
	'GL_DOUBLEBUFFER' : 1,
	'GL_DRAW_BUFFER' : 1,
	'GL_EDGE_FLAG' : 1,
	'GL_EDGE_FLAG_ARRAY' : 1,
	'GL_EDGE_FLAG_ARRAY_STRIDE' : 1,
	'GL_FOG' : 1,
	'GL_FOG_COLOR': 4,
	'GL_FOG_DENSITY' : 1,
	'GL_FOG_END' : 1,
	'GL_FOG_HINT' : 1,
	'GL_FOG_INDEX' : 1,
	'GL_FOG_MODE' : 1,
	'GL_FOG_START' : 1,
	'GL_FRONT_FACE' : 1,
	'GL_GREEN_BIAS' : 1,
	'GL_GREEN_BITS' : 1,
	'GL_GREEN_SCALE' : 1,
	'GL_INDEX_ARRAY' : 1,
	'GL_INDEX_ARRAY_STRIDE' : 1,
	'GL_INDEX_ARRAY_TYPE' : 1,
	'GL_INDEX_BITS' : 1,
	'GL_INDEX_CLEAR_VALUE' : 1,
	'GL_INDEX_LOGIC_OP' : 1,
	'GL_INDEX_MODE' : 1,
	'GL_INDEX_OFFSET' : 1,
	'GL_INDEX_SHIFT' : 1,
	'GL_INDEX_WRITEMASK' : 1,
	'GL_LIGHT0' : 1,
	'GL_LIGHT1' : 1,
	'GL_LIGHT2' : 1,
	'GL_LIGHT3' : 1,
	'GL_LIGHT4' : 1,
	'GL_LIGHT5' : 1,
	'GL_LIGHT6' : 1,
	'GL_LIGHT7' : 1,
	'GL_LIGHTING' : 1,
	'GL_LIGHT_MODEL_AMBIENT': 4,
	'GL_LIGHT_MODEL_LOCAL_VIEWER' : 1,
	'GL_LIGHT_MODEL_TWO_SIDE' : 1,
	'GL_LINE_SMOOTH' : 1,
	'GL_LINE_SMOOTH_HINT' : 1,
	'GL_LINE_STIPPLE' : 1,
	'GL_LINE_STIPPLE_PATTERN' : 1,
	'GL_LINE_STIPPLE_REPEAT' : 1,
	'GL_LINE_WIDTH' : 1,
	'GL_LINE_WIDTH_GRANULARITY' : 1,
	'GL_LINE_WIDTH_RANGE': 2, 
	'GL_LIST_BASE' : 1,
	'GL_LIST_INDEX' : 1,
	'GL_LIST_MODE' : 1,
	'GL_LOGIC_OP' : 1,
	'GL_LOGIC_OP_MODE' : 1,
	'GL_MAP1_COLOR_4' : 1,
	'GL_MAP1_GRID_DOMAIN': 2, 
	'GL_MAP1_GRID_SEGMENTS' : 1,
	'GL_MAP1_INDEX' : 1,
	'GL_MAP1_NORMAL' : 1,
	'GL_MAP1_TEXTURE_COORD_1' : 1,
	'GL_MAP1_TEXTURE_COORD_2' : 1,
	'GL_MAP1_TEXTURE_COORD_3' : 1,
	'GL_MAP1_TEXTURE_COORD_4' : 1,
	'GL_MAP1_VERTEX_3' : 1,
	'GL_MAP1_VERTEX_4' : 1,
	'GL_MAP2_COLOR_4' : 1,
	'GL_MAP2_GRID_DOMAIN': 4,
	'GL_MAP2_GRID_SEGMENTS': 2, 
	'GL_MAP2_INDEX' : 1,
	'GL_MAP2_NORMAL' : 1,
	'GL_MAP2_TEXTURE_COORD_1' : 1,
	'GL_MAP2_TEXTURE_COORD_2' : 1,
	'GL_MAP2_TEXTURE_COORD_3' : 1,
	'GL_MAP2_TEXTURE_COORD_4' : 1,
	'GL_MAP2_VERTEX_3' : 1,
	'GL_MAP2_VERTEX_4' : 1,
	'GL_MAP_COLOR' : 1,
	'GL_MAP_STENCIL' : 1,
	'GL_MATRIX_MODE' : 1,
	'GL_MAX_CLIENT_ATTRIB_STACK_DEPTH' : 1,
	'GL_MAX_ATTRIB_STACK_DEPTH' : 1,
	'GL_MAX_CLIP_PLANES' : 1,
	'GL_MAX_EVAL_ORDER' : 1,
	'GL_MAX_LIGHTS' : 1,
	'GL_MAX_LIST_NESTING' : 1,
	'GL_MAX_MODELVIEW_STACK_DEPTH' : 1,
	'GL_MAX_NAME_STACK_DEPTH' : 1,
	'GL_MAX_PIXEL_MAP_TABLE' : 1,
	'GL_MAX_PROJECTION_STACK_DEPTH' : 1,
	'GL_MAX_TEXTURE_SIZE' : 1,
	'GL_MAX_TEXTURE_STACK_DEPTH' : 1,
	'GL_MAX_VIEWPORT_DIMS': 2, 
	'GL_MODELVIEW_MATRIX': 16,
	'GL_MODELVIEW_STACK_DEPTH' : 1,
	'GL_NAME_STACK_DEPTH' : 1,
	'GL_NORMAL_ARRAY' : 1,
	'GL_NORMAL_ARRAY_STRIDE' : 1,
	'GL_NORMAL_ARRAY_TYPE' : 1,
	'GL_NORMALIZE' : 1,
	'GL_PACK_ALIGNMENT' : 1,
	'GL_PACK_LSB_FIRST' : 1,
	'GL_PACK_ROW_LENGTH' : 1,
	'GL_PACK_SKIP_PIXELS' : 1,
	'GL_PACK_SKIP_ROWS' : 1,
	'GL_PACK_SWAP_BYTES' : 1,
	'GL_PERSPECTIVE_CORRECTION_HINT' : 1,
	'GL_PIXEL_MAP_A_TO_A_SIZE' : 1,
	'GL_PIXEL_MAP_B_TO_B_SIZE' : 1,
	'GL_PIXEL_MAP_G_TO_G_SIZE' : 1,
	'GL_PIXEL_MAP_I_TO_A_SIZE' : 1,
	'GL_PIXEL_MAP_I_TO_B_SIZE' : 1,
	'GL_PIXEL_MAP_I_TO_G_SIZE' : 1,
	'GL_PIXEL_MAP_I_TO_I_SIZE' : 1,
	'GL_PIXEL_MAP_I_TO_R_SIZE' : 1,
	'GL_PIXEL_MAP_R_TO_R_SIZE' : 1,
	'GL_PIXEL_MAP_S_TO_S_SIZE' : 1,
	'GL_POINT_SIZE' : 1,
	'GL_POINT_SIZE_GRANULARITY' : 1,
	'GL_POINT_SIZE_RANGE': 2, 
	'GL_POINT_SMOOTH' : 1,
	'GL_POINT_SMOOTH_HINT' : 1,
	'GL_POLYGON_MODE': 2, 
	'GL_POLYGON_OFFSET_FACTOR' : 1,
	'GL_POLYGON_OFFSET_UNITS' : 1,
	'GL_POLYGON_OFFSET_FILL' : 1,
	'GL_POLYGON_OFFSET_LINE' : 1,
	'GL_POLYGON_OFFSET_POINT' : 1,
	'GL_POLYGON_SMOOTH' : 1,
	'GL_POLYGON_SMOOTH_HINT' : 1,
	'GL_POLYGON_STIPPLE' : 1,
	'GL_PROJECTION_MATRIX': 16,
	'GL_PROJECTION_STACK_DEPTH' : 1,
	'GL_READ_BUFFER' : 1,
	'GL_RED_BIAS' : 1,
	'GL_RED_BITS' : 1,
	'GL_RED_SCALE' : 1,
	'GL_RENDER_MODE' : 1,
	'GL_RGBA_MODE' : 1,
	'GL_SCISSOR_BOX': 4,
	'GL_SCISSOR_TEST' : 1,
	'GL_SHADE_MODEL' : 1,
	'GL_STENCIL_BITS' : 1,
	'GL_STENCIL_CLEAR_VALUE' : 1,
	'GL_STENCIL_FAIL' : 1,
	'GL_STENCIL_FUNC' : 1,
	'GL_STENCIL_PASS_DEPTH_FAIL' : 1,
	'GL_STENCIL_PASS_DEPTH_PASS' : 1,
	'GL_STENCIL_REF' : 1,
	'GL_STENCIL_TEST' : 1,
	'GL_STENCIL_VALUE_MASK' : 1,
	'GL_STENCIL_WRITEMASK' : 1,
	'GL_STEREO' : 1,
	'GL_SUBPIXEL_BITS' : 1,
	'GL_TEXTURE_1D' : 1,
	'GL_TEXTURE_2D' : 1,
	'GL_TEXTURE_COORD_ARRAY' : 1,
	'GL_TEXTURE_COORD_ARRAY_SIZE' : 1,
	'GL_TEXTURE_COORD_ARRAY_STRIDE' : 1,
	'GL_TEXTURE_COORD_ARRAY_TYPE' : 1,
	'GL_TEXTURE_ENV_COLOR': 4,
	'GL_TEXTURE_ENV_MODE' : 1,
	'GL_TEXTURE_GEN_Q' : 1,
	'GL_TEXTURE_GEN_R' : 1,
	'GL_TEXTURE_GEN_S' : 1,
	'GL_TEXTURE_GEN_T' : 1,
	'GL_TEXTURE_MATRIX': 16,
	'GL_TEXTURE_STACK_DEPTH' : 1,
	'GL_UNPACK_ALIGNMENT' : 1,
	'GL_UNPACK_LSB_FIRST' : 1,
	'GL_UNPACK_ROW_LENGTH' : 1,
	'GL_UNPACK_SKIP_PIXELS' : 1,
	'GL_UNPACK_SKIP_ROWS' : 1,
	'GL_UNPACK_SWAP_BYTES' : 1,
	'GL_VERTEX_ARRAY' : 1,
	'GL_VERTEX_ARRAY_SIZE' : 1,
	'GL_VERTEX_ARRAY_STRIDE' : 1,
	'GL_VERTEX_ARRAY_TYPE' : 1,
	'GL_VIEWPORT': 4,
	'GL_ZOOM_X' : 1,
	'GL_ZOOM_Y' : 1,
}

extensions_num_get_values = {
	'GL_BLEND_COLOR_EXT': (4, 'GL_EXT_blend_color'),
	'GL_BLEND_EQUATION_EXT': (1, 'GL_EXT_blend_minmax'),
	'GL_FOG_DISTANCE_MODE_NV': (1, 'GL_NV_fog_distance'),
	'GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB': (1, 'GL_ARB_texture_cube_map'),
	'GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT': (1, 'GL_EXT_texture_filter_anisotropic'),
	'GL_TEXTURE_BINDING_CUBE_MAP_ARB': (1, 'GL_ARB_texture_cube_map'),
	'GL_TEXTURE_CUBE_MAP_ARB': (1, 'GL_ARB_texture_cube_map')
}

keys = num_get_values.keys() + extensions_num_get_values.keys()
keys.sort()

print "struct nv_struct { GLenum pname; int num_values; } num_values_array[] = {"
for key in keys:
	try:
		print '\t{ %s, %d },' % (key, num_get_values[key])
	except KeyError:
		(nv, ifdef) = extensions_num_get_values[key]
		print '#ifdef %s' % ifdef
		print '\t{ %s, %d },' % (key, nv)
		print '#endif /* %s */' % ifdef
print "\t{ 0, 0 }"
print "};"

print """
static int __numValues( GLenum pname )
{
	struct nv_struct *temp;
	
	for (temp = num_values_array; temp->num_values != 0 ; temp++)
	{
		if (temp->pname == pname)
			return temp->num_values;
	}
	crError( "Invalid pname to __numValues: 0x%x\\n", pname );
	return 0;
}
"""

funcs = [ 'GetIntegerv', 'GetFloatv', 'GetDoublev', 'GetBooleanv' ]
types = [ 'GLint', 'GLfloat', 'GLdouble', 'GLboolean' ]

for index in range(len(funcs)):
	func_name = funcs[index]
	(return_type, arg_names, arg_types) = gl_mapping[func_name]
	print 'void SERVER_DISPATCH_APIENTRY crServerDispatch%s%s' % ( func_name, stub_common.ArgumentString( arg_names, arg_types ))
	print '{'
	print '\t%s get_values[16]; // Be safe' % types[index]
	print '\tint num_values;'
	print '\t(void) params;'
	print '\tcr_server.head_spu->dispatch_table.%s( pname, get_values );' % func_name
	print '\tnum_values = __numValues( pname);'
	print '\tcrServerReturnValue( get_values, num_values*sizeof(get_values[0]) );'
	print '}\n'
