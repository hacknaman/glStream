# Copyright (c) 2001, Stanford University
# All rights reserved.
#
# See the file LICENSE.txt for information on redistributing this software.

import sys,os;
import cPickle;
import string;
import re;

sys.path.append( "../../opengl_stub" )
sys.path.append( "../../crserver" )
parsed_file = open( "../../glapi_parser/gl_header.parsed", "rb" )
gl_mapping = cPickle.load( parsed_file )

import stub_common;

keys = gl_mapping.keys()
keys.sort();

stub_common.CopyrightC()

print """#include <stdio.h>
#include "packspu.h"
#include "cr_packfunctions.h"
#include "cr_glwrapper.h"
#include "cr_net.h"
"""

from get_sizes import *;
from get_components import *;

easy_swaps = { 
	'GenTextures': '(unsigned int) n',
	'GetClipPlane': '4',
	'GetPolygonStipple': '0',
	'GetTexImage': '0' 
}
	
simple_funcs = [ 'GetIntegerv', 'GetFloatv', 'GetDoublev', 'GetBooleanv' ]
simple_swaps = [ 'SWAP32', 'SWAPFLOAT', 'SWAPDOUBLE', '(GLboolean) SWAP32' ]

hard_funcs = {
	'GetLightfv': 'SWAPFLOAT',
	'GetLightiv': 'SWAP32',
	'GetMaterialfv': 'SWAPFLOAT',
	'GetMaterialiv': 'SWAP32',
	'GetTexEnvfv': 'SWAPFLOAT',
	'GetTexEnviv': 'SWAP32',
	'GetTexGendv': 'SWAPDOUBLE',
	'GetTexGenfv': 'SWAPFLOAT',
	'GetTexGeniv': 'SWAP32',
	'GetTexLevelParameterfv': 'SWAPFLOAT',
	'GetTexLevelParameteriv': 'SWAP32',
	'GetTexParameterfv': 'SWAPFLOAT',
	'GetTexParameteriv': 'SWAP32' }

for func_name in keys:
	(return_type, args, types) = gl_mapping[func_name]
	if stub_common.FindSpecial( "packspu", func_name ): continue
	if return_type != 'void' or stub_common.FindSpecial( "../../packer/packer_get", func_name):
		print '%s PACKSPU_APIENTRY packspu_%s%s' % ( return_type, func_name, stub_common.ArgumentString( args, types ) )
		print '{'
		print '\tint writeback = pack_spu.server.conn->type == CR_DROP_PACKETS ? 0 : 1;'
		if return_type != 'void':
			print '\t%s return_val = (%s) 0;' % (return_type, return_type)
			args.append( "&return_val" )
		if (func_name in easy_swaps.keys() and easy_swaps[func_name] != '0') or func_name in simple_funcs or func_name in hard_funcs.keys():
			print '\tunsigned int i;'
		args.append( "&writeback" )
		print '\tif (pack_spu.swap)'
		print '\t{'
		print '\t\tcrPack%sSWAP%s;' % (func_name, stub_common.CallString( args ) )
		print '\t}'
		print '\telse'
		print '\t{'
		print '\t\tcrPack%s%s;' % (func_name, stub_common.CallString( args ) )
		print '\t}'
		print '\tpackspuFlush(NULL);'
		print '\twhile (writeback)'
		print '\t\tcrNetRecv();'
		if return_type != 'void':
			print '\tif (pack_spu.swap)'
			print '\t{'
			print '\t\treturn_val = (%s) SWAP32(return_val);' % return_type
			print '\t}'
			print '\treturn return_val;'
		if func_name in easy_swaps.keys() and easy_swaps[func_name] != '0':
			limit = easy_swaps[func_name]
			print '\tfor (i = 0 ; i < %s ; i++)' % limit
			print '\t{'
			if types[-1].find( "double" ) == -1:
				print '\t\t%s[i] = SWAP32(%s[i]);' % (args[-2], args[-2])
			else:
				print '\t\t%s[i] = SWAPDOUBLE(%s[i]);' % (args[-2], args[-2])
			print '\t}'
		for index in range(len(simple_funcs)):
			if simple_funcs[index] == func_name:
				print '\tif (pack_spu.swap)'
				print '\t{'
				print '\t\tfor (i = 0 ; i < __numValues( pname ) ; i++)'
				print '\t\t{'
				if simple_swaps[index] == 'SWAPDOUBLE':
					print '\t\t\t%s[i] = %s(%s[i]);' % (args[-2], simple_swaps[index], args[-2])
				else:
					print '\t\t\t((GLuint *) %s)[i] = %s(%s[i]);' % (args[-2], simple_swaps[index], args[-2])
				print '\t\t}'
				print '\t}'
		if func_name in hard_funcs.keys():
			print '\tif (pack_spu.swap)'
			print '\t{'
			print '\t\tfor (i = 0 ; i < __lookupComponents(pname) ; i++)'
			print '\t\t{'
			if hard_funcs[func_name] == 'SWAPDOUBLE':
				print '\t\t\t%s[i] = %s(%s[i]);' % (args[-2], hard_funcs[func_name], args[-2])
			else:
				print '\t\t\t((GLuint *) %s)[i] = %s(%s[i]);' % (args[-2], hard_funcs[func_name], args[-2])
			print '\t\t}'
			print '\t}'
		print '}\n'
