
import sys,os;
import cPickle;
import string;
import re;

sys.path.append( "../../opengl_stub" )
parsed_file = open( "../../glapi_parser/gl_header.parsed", "rb" )
gl_mapping = cPickle.load( parsed_file )

import stub_common;

print """#include <stdio.h>
#include "cr_opengl_types.h"
#include "cr_error.h"
#include "cr_spu.h"

#define ERROR_UNUSED(x) ((void)x)"""

keys = gl_mapping.keys()
keys.sort();

for func_name in keys:
	(return_type, names, types) = gl_mapping[func_name]
	print '\n%s error%s%s' % (return_type, func_name, stub_common.ArgumentString( names, types ))
	print '{'
	print '\tCRError( "Unsupported function gl%s called!" );' % func_name
	for name in names:
		# Handle the void parameter list
		if name:
			print '\tERROR_UNUSED(%s);' % name
	if return_type != "void":
		print '\treturn (%s)0;' % return_type
	print '}'

print 'SPUNamedFunctionTable error_table[] = {'
for index in range(len(keys)):
	func_name = keys[index]
	if index != len(keys) - 1:
		print '\t{ "%s", (SPUGenericFunction) error%s },' % (func_name, func_name )
	else:
		print '\t{ "%s", (SPUGenericFunction) error%s }' % (func_name, func_name )
print '};'
