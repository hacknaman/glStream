# Copyright (c) 2001, Stanford University
# All rights reserved.
#
# See the file LICENSE.txt for information on redistributing this software.

import sys

import cPickle;
import string;
import re;

sys.path.append( "../../opengl_stub" )
parsed_file = open( "../../glapi_parser/gl_header.parsed", "rb" )
gl_mapping = cPickle.load( parsed_file )

import stub_common;

keys = gl_mapping.keys()
keys.sort();

#this is simply a list of things that can appear legally between a begin and end
#block.  Note that it *has* to include Begin -- this bug was killing Sean.

things_pinch_cares_about = [ 'Begin', 'End', 'Vertex2d', 'Vertex2f', 'Vertex2i', 'Vertex2s', 'Vertex3d',
	'Vertex3f', 'Vertex3i', 'Vertex3s', 'Vertex4d', 'Vertex4f', 'Vertex4i', 'Vertex4s',
	'Color3b', 'Color3d', 'Color3f', 'Color3i', 'Color3s', 'Color3ub', 'Color3ui',
	'Color3us', 'Color4b', 'Color4d', 'Color4f', 'Color4i', 'Color4s', 'Color4ub',
	'Color4ui', 'Color4us', 'Indexd', 'Indexdv', 'Indexf', 'Indexfv', 'Indexi', 'Indexiv',
	'Indexs', 'Indexsv', 'Indexub', 'Indexubv', 'Normal3b', 'Normal3bv', 'Normal3d',
	'Normal3dv', 'Normal3f', 'Normal3fv', 'Normal3i', 'Normal3iv', 'Normal3s', 'Normal3sv',
	'TexCoord1d', 'TexCoord1f', 'TexCoord1i', 'TexCoord1s', 'TexCoord2d', 'TexCoord2f',
	'TexCoord2i', 'TexCoord2s', 'TexCoord3d', 'TexCoord3f', 'TexCoord3i', 'TexCoord3s',
	'TexCoord4d', 'TexCoord4f', 'TexCoord4i', 'TexCoord4s', 
	'MultiTexCoord1dARB', 'MultiTexCoord1fARB', 'MultiTexCoord1iARB', 'MultiTexCoord1sARB', 'MultiTexCoord2dARB', 'MultiTexCoord2fARB',
	'MultiTexCoord2iARB', 'MultiTexCoord2sARB', 'MultiTexCoord3dARB', 'MultiTexCoord3fARB', 'MultiTexCoord3iARB', 'MultiTexCoord3sARB',
	'MultiTexCoord4dARB', 'MultiTexCoord4fARB', 'MultiTexCoord4iARB', 'MultiTexCoord4sARB', 'EvalCoord1d', 'EvalCoord1f',
	'EvalCoord2d', 'EvalCoord2f', 'EvalPoint1', 'EvalPoint2', 'Materialf', 'Materiali',
	'Materialfv', 'Materialiv',
	'EdgeFlag',
	'CallList',
	'SecondaryColor3bEXT',
	'SecondaryColor3dEXT',
	'SecondaryColor3fEXT',
	'SecondaryColor3iEXT',
	'SecondaryColor3sEXT',
	'SecondaryColor3ubEXT',
	'SecondaryColor3uiEXT',
	'SecondaryColor3usEXT',
	'FogCoordfEXT',
	'FogCoorddEXT',
	'VertexAttrib1dARB',
	'VertexAttrib1fARB',
	'VertexAttrib1sARB',
	'VertexAttrib2dARB',
	'VertexAttrib2fARB',
	'VertexAttrib2sARB',
	'VertexAttrib3dARB',
	'VertexAttrib3fARB',
	'VertexAttrib3sARB',
	'VertexAttrib4dARB',
	'VertexAttrib4fARB',
	'VertexAttrib4sARB',
	'VertexAttrib4bvARB',
	'VertexAttrib4ivARB',
	'VertexAttrib4ubvARB',
	'VertexAttrib4uivARB',
	'VertexAttrib4usvARB',
	'VertexAttrib4NbvARB',
	'VertexAttrib4NivARB',
	'VertexAttrib4NsvARB',
	'VertexAttrib4NubARB',
	'VertexAttrib4NubvARB',
	'VertexAttrib4NuivARB',
	'VertexAttrib4NusvARB'
]

special_sizes = {
	'VertexAttrib4bvARB' : 4 + 4,
	'VertexAttrib4ivARB' : 16 + 4,
	'VertexAttrib4ubvARB' : 4 + 4,
	'VertexAttrib4uivARB' : 16 + 4,
	'VertexAttrib4usvARB' : 8 + 4,
	'VertexAttrib4NbvARB' : 4 + 4,
	'VertexAttrib4NivARB' : 16 + 4,
	'VertexAttrib4NsvARB' : 8 + 4,
	'VertexAttrib4NubARB' : 4 + 4,
	'VertexAttrib4NubvARB' : 4 + 4,
	'VertexAttrib4NuivARB' : 16 + 4,
	'VertexAttrib4NusvARB' : 8 + 4
}


stub_common.CopyrightC()

print """
static const int __cr_packet_length_table[] = {
"""
for func_name in keys:
	(return_type, arg_names, arg_types ) = gl_mapping[func_name]
	if stub_common.FindSpecial( "../../packer/opcode", func_name ) or stub_common.FindSpecial( "../../packer/opcode_extend", func_name ):
		continue
	if func_name in things_pinch_cares_about:
		if func_name in special_sizes.keys():
			size = special_sizes[func_name]
		else:
			size = stub_common.PacketLength( arg_types )
		print "\t%2d, /* %s */" % (size,  func_name)
	else:
		print '\t-1, /* %s */' % func_name
print '\t0 /* crap */'
print "};"
