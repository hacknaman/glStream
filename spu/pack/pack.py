import sys,os;
import cPickle;
import string;
import re;

sys.path.append( "../../opengl_stub" )
parsed_file = open( "../../glapi_parser/gl_header.parsed", "rb" )
gl_mapping = cPickle.load( parsed_file )

import stub_common;

keys = gl_mapping.keys()
keys.sort();



print """#include <stdio.h>
#include "cr_string.h"
#include "cr_spu.h"
#include "packspu.h"
#include "cr_packfunctions.h"
#include "cr_glwrapper.h"
"""

num_funcs = len(keys) - len(stub_common.AllSpecials('pack_unimplemented'))
print 'SPUNamedFunctionTable pack_table[%d];' % (num_funcs+1)

print """
static void __fillin( int offset, char *name, SPUGenericFunction func )
{
	pack_table[offset].name = crStrdup( name );
	pack_table[offset].fn = func;
}"""

pack_specials = []

for func_name in keys:
	(return_type, args, types) = gl_mapping[func_name]
	if (return_type != 'void' or 
	    stub_common.FindSpecial( "packspu", func_name ) or 
	    stub_common.FindSpecial( "packspu_pixel", func_name ) or 
	    stub_common.FindSpecial( "packspu_client", func_name ) or 
	    stub_common.FindSpecial( "../../packer/packer_get", func_name )):
	  pack_specials.append( func_name )

for func_name in keys:
	(return_type, args, types) = gl_mapping[func_name]
	if stub_common.FindSpecial( "pack_unimplemented", func_name ):
		continue
	if func_name in pack_specials:
		print 'extern %s PACKSPU_APIENTRY packspu_%s%s;' % ( return_type, func_name, stub_common.ArgumentString( args, types ) )

print '\nvoid packspuCreateFunctions( )'
print '{'
for index in range(len(keys)):
	func_name = keys[index]
	(return_type, args, types) = gl_mapping[func_name]
	if stub_common.FindSpecial( "pack_unimplemented", func_name ):
		continue
	if func_name in pack_specials:
		print '\t__fillin( %3d, "%s", (SPUGenericFunction) packspu_%s );' % (index, func_name, func_name )
	else:
		print '\t__fillin( %3d, "%s", (SPUGenericFunction) crPack%s );' % (index, func_name, func_name )
print '\t__fillin( %3d, NULL, NULL );' % num_funcs
print '}'
