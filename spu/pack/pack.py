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

print 'SPUNamedFunctionTable pack_table[%d];' % len(keys)

print """
static void __fillin( int offset, char *name, SPUGenericFunction func )
{
	pack_table[offset].name = crStrdup( name );
	pack_table[offset].fn = func;
}"""

for func_name in keys:
	(return_type, args, types) = gl_mapping[func_name]
	if return_type != 'void' or stub_common.FindSpecial( "packspu", func_name ) or stub_common.FindSpecial( "../../packer/packer_get", func_name ):
		print 'extern %s PACKSPU_APIENTRY packspu_%s%s;' % ( return_type, func_name, stub_common.ArgumentString( args, types ) )

print '\nvoid packspuCreateFunctions( )'
print '{'
for index in range(len(keys)):
	func_name = keys[index]
	(return_type, args, types) = gl_mapping[func_name]
	if return_type != 'void' or stub_common.FindSpecial( "packspu", func_name ) or stub_common.FindSpecial( "../../packer/packer_get", func_name ):
		print '\t__fillin( %3d, "%s", (SPUGenericFunction) packspu_%s );' % (index, func_name, func_name )
	else:
		print '\t__fillin( %3d, "%s", (SPUGenericFunction) crPack%s );' % (index, func_name, func_name )
print '}'
