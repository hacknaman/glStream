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
#include "tilesortspu.h"
#include "cr_packfunctions.h"
#include "cr_glstate.h"
#include "cr_glwrapper.h"
"""

print 'SPUNamedFunctionTable tilesort_table[%d];' % len(keys)

print """
static void __fillin( int offset, char *name, SPUGenericFunction func )
{
	tilesort_table[offset].name = crStrdup( name );
	tilesort_table[offset].fn = func;
}"""

for func_name in keys:
	(return_type, args, types) = gl_mapping[func_name]
	if stub_common.FindSpecial( "tilesort", func_name ):
		print 'extern %s TILESORTSPU_APIENTRY tilesortspu_%s%s;' % ( return_type, func_name, stub_common.ArgumentString( args, types ) )

print '\nvoid tilesortspuCreateFunctions( )'
print '{'
for index in range(len(keys)):
	func_name = keys[index]
	(return_type, args, types) = gl_mapping[func_name]
	if stub_common.FindSpecial( "tilesort", func_name ):
		print '\t__fillin( %3d, "%s", (SPUGenericFunction) tilesortspu_%s );' % (index, func_name, func_name )
	elif stub_common.FindSpecial( "tilesort_state", func_name ):
		print '\t__fillin( %3d, "%s", (SPUGenericFunction) crState%s );' % (index, func_name, func_name )
	else:
		print '\t__fillin( %3d, "%s", (SPUGenericFunction) crPack%s );' % (index, func_name, func_name )
print '}'
