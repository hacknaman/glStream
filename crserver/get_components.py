# Copyright (c) 2001, Stanford University
# All rights reserved.
#
# See the file LICENSE.txt for information on redistributing this software.

num_components = {
	'GL_AMBIENT' : 4, 
	'GL_DIFFUSE' : 4,
	'GL_SPECULAR' : 4,
	'GL_POSITION' : 4,
	'GL_SPOT_DIRECTION' : 3,
	'GL_SPOT_EXPONENT' : 1, 
	'GL_SPOT_CUTOFF' : 1, 
	'GL_CONSTANT_ATTENUATION' : 1, 
	'GL_LINEAR_ATTENUATION' : 1, 
	'GL_QUADRATIC_ATTENUATION' : 1, 
	'GL_EMISSION' : 4, 
	'GL_SHININESS' : 1, 
	'GL_COLOR_INDEXES' : 3, 
	'GL_TEXTURE_ENV_MODE' : 1,
	'GL_TEXTURE_ENV_COLOR' : 4, 
	'GL_TEXTURE_GEN_MODE' : 1, 
	'GL_OBJECT_PLANE' : 4, 
	'GL_EYE_PLANE' : 4, 
	'GL_TEXTURE_MAG_FILTER' : 1,
	'GL_TEXTURE_MIN_FILTER' : 1, 
	'GL_TEXTURE_WRAP_S' : 1, 
	'GL_TEXTURE_WRAP_T' : 1, 
	'GL_TEXTURE_BORDER_COLOR' : 4,
	'GL_TEXTURE_WIDTH': 1,
	'GL_TEXTURE_HEIGHT': 1,
	# 'GL_TEXTURE_INTERNAL_FORMAT': 1,  THIS CONFLICTS WITH SOMETHING?!
	'GL_TEXTURE_BORDER': 1,
	'GL_TEXTURE_RED_SIZE': 1,
	'GL_TEXTURE_GREEN_SIZE': 1,
	'GL_TEXTURE_BLUE_SIZE': 1,
	'GL_TEXTURE_ALPHA_SIZE': 1,
	'GL_TEXTURE_LUMINANCE_SIZE': 1,
	'GL_TEXTURE_INTENSITY_SIZE': 1,
	'GL_TEXTURE_COMPONENTS': 1
}

num_extended_components = {
	'GL_TEXTURE_MAX_ANISOTROPY_EXT': ( 1, 'CR_EXT_texture_filter_anisotropic' ),
	'GL_TEXTURE_WRAP_R': ( 1, 'CR_OPENGL_VERSION_1_2'),
	'GL_TEXTURE_PRIORITY': ( 1, 'CR_OPENGL_VERSION_1_2'),
	'GL_TEXTURE_MIN_LOD': ( 1, 'CR_OPENGL_VERSION_1_2'),
	'GL_TEXTURE_MAX_LOD': ( 1, 'CR_OPENGL_VERSION_1_2'),
	'GL_TEXTURE_BASE_LEVEL': ( 1, 'CR_OPENGL_VERSION_1_2'),
	'GL_TEXTURE_MAX_LEVEL': ( 1, 'CR_OPENGL_VERSION_1_2'),
	'GL_COMBINER_INPUT_NV': ( 1, 'CR_NV_register_combiners'),
	'GL_COMBINER_MAPPING_NV': ( 1, 'CR_NV_register_combiners'),
	'GL_COMBINER_COMPONENT_USAGE_NV': ( 1, 'CR_NV_register_combiners'),
	'GL_COMBINER_AB_DOT_PRODUCT_NV': ( 1, 'CR_NV_register_combiners'),
	'GL_COMBINER_CD_DOT_PRODUCT_NV': ( 1, 'CR_NV_register_combiners'),
	'GL_COMBINER_MUX_SUM_NV': ( 1, 'CR_NV_register_combiners'),
	'GL_COMBINER_SCALE_NV': ( 1, 'CR_NV_register_combiners'),
	'GL_COMBINER_BIAS_NV': ( 1, 'CR_NV_register_combiners'),
	'GL_COMBINER_AB_OUTPUT_NV': ( 1, 'CR_NV_register_combiners'),
	'GL_COMBINER_CD_OUTPUT_NV': ( 1, 'CR_NV_register_combiners'),
	'GL_COMBINER_SUM_OUTPUT_NV': ( 1, 'CR_NV_register_combiners'),
	'GL_COMBINER_INPUT_NV': ( 1, 'CR_NV_register_combiners'),
	'GL_COMBINER_INPUT_NV': ( 1, 'CR_NV_register_combiners'),
	'GL_COMBINER_MAPPING_NV': ( 1, 'CR_NV_register_combiners'),
	'GL_COMBINER_COMPONENT_USAGE_NV': ( 1, 'CR_NV_register_combiners')
}

print """static unsigned int __lookupComponents( GLenum pname )
{
	switch( pname )
	{
"""
comps = num_components.keys();
comps.sort();
for comp in comps:
	print '\t\t\tcase %s: return %d;' % (comp,num_components[comp])

comps = num_extended_components.keys();
comps.sort();
for comp in comps:
	(nc, ifdef) = num_extended_components[comp]
	print '#ifdef %s' % ifdef
	print '\t\t\tcase %s: return %d;' % (comp,nc)
	print '#endif /* %s */' % ifdef

print """
		default:
			crError( "Unknown parameter name in __lookupComponents: %d", pname );
			break;
	}
	/* NOTREACHED */
	return 0;
}
"""


