import sys;
import cPickle;
import types;
import string;
import re;

sys.path.append( "../opengl_stub" )

import stub_common;

parsed_file = open( "../glapi_parser/gl_header.parsed", "rb" )
gl_mapping = cPickle.load( parsed_file )
keys = gl_mapping.keys()
keys.sort()

print """#include "cr_unpack.h"
#include "cr_opcodes.h"
#include "cr_unpackfuncs.h"
#include "cr_glwrapper.h"
#include "cr_error.h"
#include "cr_spu.h"
#include <stdio.h>

unsigned char *cr_unpackData = NULL;
SPUDispatchTable cr_unpackDispatch;
"""

def MakeVector( func_name ):
	return func_name + "v"

def VectorLength( func_name ):
	m = re.search( r"([0-9])", func_name )
	return string.atoi( m.group(1) )

def ReadData( offset, arg_type ):
	if arg_type == "GLdouble" or arg_type == "GLclampd":
		retval = "READ_DOUBLE( %d )" % offset
	else:
		retval = "READ_DATA( %d, %s )" % (offset, arg_type)
	return retval

def FindReturnPointer( return_type, arg_types ):
	arg_len = stub_common.PacketLength( arg_types )
	if (return_type != 'void'):
		print '\tSET_RETURN_PTR( %d );' % (arg_len + 8) # extended opcode plus packet length
	else:
		print '\tSET_RETURN_PTR( %d );' % (arg_len + 8 - stub_common.PacketLength(['void *']))

def FindWritebackPointer( return_type, arg_types ):
	arg_len = stub_common.PacketLength( arg_types )
	if return_type != 'void':
		arg_len += stub_common.PacketLength( ['void *'] )

	print '\tSET_WRITEBACK_PTR( %d );' % (arg_len + 8) # extended opcode plus packet length


def MakeNormalCall( return_type, func_name, arg_types, arg_names, counter_init = 0 ):
	counter = counter_init;
	for i in range( 0, len(arg_names) ):
		if arg_names[i] != '':
			arg_len = stub_common.PacketLength( [arg_types[i]] )
			counter = stub_common.FixAlignment( counter, arg_len )
			if string.find( arg_types[i], '*' ) != -1:
				arg_names[i] = 'NULL'
			else:
				print "\t%s %s = %s;" % ( arg_types[i], arg_names[i], ReadData( counter, arg_types[i] ) )
			counter = counter + arg_len
	if return_type != 'void' or stub_common.FindSpecial( "../packer/packer_get", func_name ):
		FindReturnPointer( return_type, arg_types );
		FindWritebackPointer( return_type, arg_types );
	if return_type != "void":
		print "\t(void) cr_unpackDispatch.%s(" % func_name,
	else:
		print "\tcr_unpackDispatch.%s(" % func_name,
	print string.join( arg_names, ", " ),
	print ");";

def MakeVectorCall( return_type, func_name, arg_type ):
	if arg_type == "GLdouble" or arg_type == "GLclampd":
		print "#ifdef CR_UNALIGNED_ACCESS_OKAY"
		print "\tcr_unpackDispatch.%s((%s *) cr_unpackData);" % (MakeVector( func_name ), arg_type )
		print "#else"
		num_args = VectorLength( func_name )
		for index in range( 0, num_args ):
			print "\tGLdouble v" + `index` + " = READ_DOUBLE(", `index * 8`, ");"
		if return_type != "void":
			print "\t(void) cr_unpackDispatch.%s(" % func_name,
		else:
			print "\tcr_unpackDispatch.%s(" % func_name,
		for index in range( 0, num_args ):
			print "v" + `index`,
			if index != num_args - 1:
				print ",",
		print ");"
		print "#endif"
	else:
		print "\tcr_unpackDispatch.%s((%s *) cr_unpackData);" %( MakeVector( func_name ), arg_type )

for func_name in keys:
	if stub_common.FindSpecial( "../packer/opcode", func_name ): continue
	if stub_common.FindSpecial( "unpacker", func_name ): continue

	( return_type, arg_names, arg_types ) = gl_mapping[func_name]
	print "void crUnpack%s(void)" % func_name
	print "{"

	if not stub_common.FindSpecial( "unpacker_vector", func_name ):
		MakeNormalCall( return_type, func_name, arg_types, arg_names )
	else:
		MakeVectorCall( return_type, func_name, arg_types[0] )
	packet_length = stub_common.PacketLength( arg_types )
	if packet_length == 0:
	    print "\tINCR_DATA_PTR_NO_ARGS( );"
	else:
	    print "\tINCR_DATA_PTR( %d );" % packet_length
	print "}\n"

print """ 
void crUnpack( void *data, void *opcodes, 
		unsigned int num_opcodes, SPUDispatchTable *table )
{
	unsigned int i;
	unsigned char *unpack_opcodes;
	crSPUCopyDispatchTable( &cr_unpackDispatch, table );

	unpack_opcodes = (unsigned char *)opcodes;
	cr_unpackData = (unsigned char *)data;

	for (i = 0 ; i < num_opcodes ; i++)
	{
		switch( *unpack_opcodes )
		{"""

for func_name in keys:
	if stub_common.FindSpecial( "../packer/opcode", func_name ): continue
	(return_type, args, types) = gl_mapping[ func_name ]
	print '\t\t\tcase %s:' % stub_common.OpcodeName( func_name ),
	#print 'crDebug( "Decoding %s" );' % func_name,
	print 'crUnpack%s(); break;' % func_name

print """		
			case CR_EXTEND_OPCODE: crUnpackExtend(); break;
			default:
				crError( "Unknown opcode: %d", *unpack_opcodes );
				break;
		}
		unpack_opcodes--;
	}
}"""

for func_name in keys:
	( return_type, arg_names, arg_types ) = gl_mapping[func_name]
	if return_type != 'void' or stub_common.FindSpecial( "../packer/packer_get", func_name ):
		print 'void crUnpackExtend%s(void)' % func_name
		print '{'
		MakeNormalCall( return_type, func_name, arg_types, arg_names, 8 )
		print '}\n'

print """
void crUnpackExtend(void)
{
	GLenum extend_opcode = %s;
	switch( extend_opcode )
	{""" % ReadData( 4, 'GLenum' );
for func_name in keys:
	( return_type, arg_names, arg_types ) = gl_mapping[func_name]
	if return_type != 'void' or stub_common.FindSpecial( "../packer/packer_get", func_name ):
		print '\t\tcase %s:' % stub_common.ExtendedOpcodeName( func_name )
		#print '\t\t\tcrDebug( "Decoding %s (Extended)" );' % func_name,
		print '\t\t\tcrUnpackExtend%s( );' % func_name
		print '\t\t\tbreak;'
print """		default:
			crError( "Unknown extended opcode: %d", extend_opcode );
			break;
	}
	INCR_VAR_PTR();
}"""
