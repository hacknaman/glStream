# Copyright (c) 2001, Stanford University
# All rights reserved.
#
# See the file LICENSE.txt for information on redistributing this software.

# This script generates the cr/include/cr_packfunctions.h file from the
# gl_header.parsed file.

import sys;
import cPickle;
import types;
import string;
import re;

sys.path.append( "../opengl_stub" )

import stub_common;

parsed_file = open( "../glapi_parser/gl_header.parsed", "rb" )
gl_mapping = cPickle.load( parsed_file )

stub_common.CopyrightC()

print """#ifndef CR_PACKFUNCTIONS_H
#define CR_PACKFUNCTIONS_H

/* DO NOT EDIT - THIS FILE GENERATED BY THE pack_header.py SCRIPT */

/* Prototypes for the OpenGL packer functions in packer.c and pack_bbox.c */

#include "cr_glwrapper.h"
#include "state/cr_pixel.h"
#include "state/cr_client.h"
#include "cr_pack.h"

#ifdef WINDOWS
#define PACK_APIENTRY __stdcall
#else
#define PACK_APIENTRY
#endif
"""

keys = gl_mapping.keys()
keys.sort()


for func_name in keys:
	( return_type, arg_names, arg_types ) = gl_mapping[func_name]
	if return_type != 'void':
		if string.find( return_type, '*' ) != -1:
			arg_types.append( "%s" % return_type )
		else:
			arg_types.append( "%s *" % return_type )
		arg_names.append( "return_value" )
	elif stub_common.FindSpecial( "packer_pixel", func_name ):	
		arg_types.append( "CRPackState *" )
		arg_names.append( "packstate" )
	elif stub_common.FindSpecial( "packer_client", func_name ):	
		arg_types.append( "CRClientState *" )
		arg_names.append( "ctx" )
	if return_type != 'void' or stub_common.FindSpecial( 'packer_get', func_name ):
		arg_types.append( "int *" )
		arg_names.append( "writeback" )
	print 'void PACK_APIENTRY crPack%s%s;' %( func_name, stub_common.ArgumentString( arg_names, arg_types ) )
	print 'void PACK_APIENTRY crPack%sSWAP%s;' %( func_name, stub_common.ArgumentString( arg_names, arg_types ) )

for n in [2,3,4]:
	for t in ['d', 'f', 'i', 's']:
		for v in ['', 'v']:
			func_name = 'Vertex%d%s%s' % (n,t,v)
			( return_type, arg_names, arg_types ) = gl_mapping[func_name]
			print 'void PACK_APIENTRY crPack%sBBOX%s;' % (func_name, stub_common.ArgumentString( arg_names, arg_types ) )
			print 'void PACK_APIENTRY crPack%sBBOX_COUNT%s;' % (func_name, stub_common.ArgumentString( arg_names, arg_types ) )
			print 'void PACK_APIENTRY crPack%sBBOXSWAP%s;' % (func_name, stub_common.ArgumentString( arg_names, arg_types ) )
			print 'void PACK_APIENTRY crPack%sBBOX_COUNTSWAP%s;' % (func_name, stub_common.ArgumentString( arg_names, arg_types ) )

print '\n#endif /* CR_PACKFUNCTIONS_H */'
