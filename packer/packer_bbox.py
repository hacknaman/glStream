import sys;
import cPickle;
import types;
import string;
import re;

sys.path.append( "../opengl_stub" )

import stub_common;

parsed_file = open( "../glapi_parser/gl_header.parsed", "rb" )
gl_mapping = cPickle.load( parsed_file )

print """
#include "cr_glwrapper.h"
#include "packer.h"
#include "cr_opcodes.h"
#include "pack_bbox.h"

#include <float.h>

void crPackResetBBOX(void)
{
	cr_packer_globals.bounds_min.x =  FLT_MAX;
	cr_packer_globals.bounds_min.y =  FLT_MAX;
	cr_packer_globals.bounds_min.z =  FLT_MAX;
	cr_packer_globals.bounds_max.x = -FLT_MAX;
	cr_packer_globals.bounds_max.y = -FLT_MAX;
	cr_packer_globals.bounds_max.z = -FLT_MAX;
	cr_packer_globals.updateBBOX = 1;
}
"""

def WriteData( offset, arg_type, arg_name ):
	if string.find( arg_type, '*' ) != -1:
		retval = "\tWRITE_NETWORK_POINTER( %d, (void *) %s );" % (offset, arg_name )
	elif arg_type == "GLdouble" or arg_type == "GLclampd":
		retval = "\tWRITE_DOUBLE( %d, %s );" % (offset, arg_name)
	else:	
		retval = "\tWRITE_DATA( %d, %s, %s );" % (offset, arg_type, arg_name)
	return retval

for num_coords in [2,3,4]:
	for argtype in ['d', 'f', 'i', 's']:
		func_name = 'Vertex%d%s' % (num_coords,argtype)
		( return_type, arg_names, arg_types ) = gl_mapping[func_name]
		if argtype == 'f':
			arg_names = map( lambda(s): 'f%s' % s, arg_names )

		print 'void PACK_APIENTRY %sBBOX' % stub_common.PackFunction( func_name ),
		print stub_common.ArgumentString( arg_names, arg_types )
		print '{'
		packet_length = stub_common.PacketLength( arg_types )

		print "\tunsigned char *data_ptr;"
		if arg_types[0] != 'GLfloat':
			print "\tCREATE_%dD_FLOATS();" % num_coords

		print "\tGET_BUFFERED_POINTER( %d );" % packet_length
		print "\tif (cr_packer_globals.updateBBOX)"
		print "\t{"
		if num_coords < 4:
			print "\t\tUPDATE_%dD_BBOX();" % num_coords
		else:
			print "\t\tUPDATE_3D_BBOX();"
		print "\t}"

		counter = 0
		for index in range(0,len(arg_names)):
			print WriteData( counter, arg_types[index], arg_names[index] )
			counter += stub_common.lengths[arg_types[index]]
		print "\tWRITE_OPCODE( %s );" % stub_common.OpcodeName( func_name )
		print '}\n'

		func_name = 'Vertex%d%sv' % (num_coords,argtype)
		( return_type, arg_names, arg_types ) = gl_mapping[func_name]
		vector_type = re.sub( r'\*', '', arg_types[0] )
		vector_type = re.sub( r'const', '', vector_type )
		vector_type = string.strip(vector_type)

		print 'void PACK_APIENTRY %sBBOX' % stub_common.PackFunction( func_name ),
		print stub_common.ArgumentString( arg_names, arg_types )
		print '{'
		packet_length = num_coords * stub_common.lengths[vector_type]
		if packet_length % 4 != 0:
			packet_length += 2

		print "\tunsigned char *data_ptr;"
		if arg_types[0] != 'GLfloat':
			print "\tCREATE_%dD_VFLOATS();" % num_coords

		print "\tGET_BUFFERED_POINTER( %d );" % packet_length
		print "\tif (cr_packer_globals.updateBBOX)"
		print "\t{"
		if num_coords < 4:
			print "\t\tUPDATE_%dD_BBOX();" % num_coords
		else:
			print "\t\tUPDATE_3D_BBOX();"
		print "\t}"

		counter = 0
		for index in range(num_coords):
			print WriteData( counter, vector_type, "%s[%d]" % (arg_names[0], index) )
			counter += stub_common.lengths[vector_type]
		print "\tWRITE_OPCODE( %s );" % stub_common.OpcodeName( func_name[:-1] )
		print '}\n'
