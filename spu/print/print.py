
import sys,os;
import cPickle;
import string;
import re;

sys.path.append( "../../opengl_stub" )
parsed_file = open( "../../glapi_parser/gl_header.parsed", "rb" )
gl_mapping = cPickle.load( parsed_file )

import stub_common;

print """#include <stdio.h>
#include "cr_error.h"
#include "cr_spu.h"
#include "cr_glwrapper.h"
#include "printspu.h"

#if defined(WINDOWS)
#define PRINT_APIENTRY __stdcall
#else
#define PRINT_APIENTRY
#endif

#define PRINT_UNUSED(x) ((void)x)"""

keys = gl_mapping.keys()
keys.sort();

printf_mapping = {
	'GLint':      '%d',
	'GLshort':    '%d',
	'GLbyte':     '%d',
	'GLubyte':    '%u',
	'GLuint':     '%u',
	'GLushort':   '%u',
	'GLenum':     '%s',
	'GLfloat':    '%f',
	'GLclampf':   '%f',
	'GLdouble':   '%f',
	'GLclampd':   '%f',
	'GLbitfield': '0x%x',
	'GLboolean':  '%s',
	'GLsizei':    '%u'
}

for func_name in keys:
	if stub_common.FindSpecial( 'printspu_unimplemented', func_name ): continue
	if stub_common.FindSpecial( 'printspu_special', func_name ): continue
	(return_type, names, types) = gl_mapping[func_name]
	print '\n%s PRINT_APIENTRY print%s%s' % (return_type, func_name, stub_common.ArgumentString( names, types ))
	print '{'
	print '\tfprintf( stderr, "%s(' % func_name ,

	printfstr = ""
	argstr = ", "

	vector_nelem = stub_common.IsVector( func_name )
	if vector_nelem != 0:
		printfstr += '['
		vector_arg_type = re.sub( r'\*', '', types[0] )
		vector_arg_type = re.sub( r'const ', '', vector_arg_type )
		vector_arg_type = string.strip( vector_arg_type );
		for index in range( vector_nelem ):
			if printf_mapping.has_key( vector_arg_type ):
				printfstr += printf_mapping[vector_arg_type]
				arg = '%s[%d]' % (names[0], index)
				if vector_arg_type == 'GLboolean':
					argstr += '%s ? "true" : "false"' % arg
				else:
					argstr += arg
			if index != vector_nelem - 1:
				printfstr += ", "
				argstr += ", "
		printfstr += ']'
	else:
		for i in range(len(names)):
			name = names[i]
			type = types[i]

			if printf_mapping.has_key( type ):
				printfstr += printf_mapping[type];
				if type == 'GLenum':
					argstr += 'printspuEnumToStr( %s )' % name
				elif type == 'GLboolean':
					argstr += '%s ? "true" : "false"' % name
				else:
					argstr += name
			else:
				argstr = ""
				printfstr = ""
				break;
			if i != len(names) - 1:
				printfstr += ", "
				argstr += ", "
				
	print '%s )\\n"%s );' % ( printfstr, argstr )

	if return_type != "void":
		print '\treturn',
	else:
		print '\t',
	print 'print_spu.passthrough.%s%s;' % ( func_name, stub_common.CallString( names ) )
	print '}'

print 'SPUNamedFunctionTable print_table[] = {'
for index in range(len(keys)):
	func_name = keys[index]
	if stub_common.FindSpecial( 'printspu_unimplemented', func_name ): continue
	if index != len(keys) - 1:
		print '\t{ "%s", (SPUGenericFunction) print%s },' % (func_name, func_name )
	else:
		print '\t{ "%s", (SPUGenericFunction) print%s }' % (func_name, func_name )
print '};'
