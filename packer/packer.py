# Copyright (c) 2001, Stanford University
# All rights reserved.
#
# See the file LICENSE.txt for information on redistributing this software.

# This script generates the packer.c file from the gl_header.parsed file.

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

print """
/* DO NOT EDIT - THIS FILE GENERATED BY THE packer.py SCRIPT */

/* For each of the OpenGL functions we have a packer function which
 * packs the function's opcode and arguments into a buffer.
 */

#include "cr_glwrapper.h"
#include "packer.h"
#include "cr_opcodes.h"

DLLDATA CRPackGlobals cr_packer_globals;
"""

keys = gl_mapping.keys()
keys.sort()

def SmackVector( func_name ):
	match = re.search( r"v$", func_name )
	arb_match = re.search( r"vARB$", func_name )
	if match:
		ret = re.sub( "v$", "", func_name )
	elif arb_match:
		ret = re.sub( "vARB$", "ARB", func_name )
	else:
		ret = func_name
	return ret

def VectorLength( func_name ):
	m = re.search( r"([0-9])", func_name )
	return string.atoi( m.group(1) )

def WriteData( offset, arg_type, arg_name, is_swapped ):
	if string.find( arg_type, '*' ) != -1:
		retval = "\tWRITE_NETWORK_POINTER( %d, (void *) %s );" % (offset, arg_name )
	else:	
		if is_swapped:
			if arg_type == "GLfloat" or arg_type == "GLclampf":
				retval = "\tWRITE_DATA( %d, %s, SWAPFLOAT(%s) );" % (offset, arg_type, arg_name)
			elif arg_type == "GLdouble" or arg_type == "GLclampd":
				retval = "\tWRITE_SWAPPED_DOUBLE( %d, %s );" % (offset, arg_name)
			elif stub_common.lengths[arg_type] == 1:
				retval = "\tWRITE_DATA( %d, %s, %s );" % (offset, arg_type, arg_name)
			elif stub_common.lengths[arg_type] == 2:
				retval = "\tWRITE_DATA( %d, %s, SWAP16(%s) );" % (offset, arg_type, arg_name)
			elif stub_common.lengths[arg_type] == 4:
				retval = "\tWRITE_DATA( %d, %s, SWAP32(%s) );" % (offset, arg_type, arg_name)
		else:
			if arg_type == "GLdouble" or arg_type == "GLclampd":
				retval = "\tWRITE_DOUBLE( %d, %s );" % (offset, arg_name)
			else:
				retval = "\tWRITE_DATA( %d, %s, %s );" % (offset, arg_type, arg_name)
	return retval

def UpdateCurrentPointer( func_name ):
	m = re.search( r"^(Color|Normal)([1234])(ub|b|us|s|ui|i|f|d)$", func_name )
	if m :
		k = m.group(1)
		name = '%s%s' % (k[:1].lower(),k[1:])
		type = m.group(3) + m.group(2)
		print "\tcr_packer_globals.current.%s.%s = data_ptr;" % (name,type)
		return

	m = re.search( r"^(TexCoord)([1234])(ub|b|us|s|ui|i|f|d)$", func_name )
	if m :
		k = m.group(1)
		name = 'texCoord'
		type = m.group(3) + m.group(2)
		print "\tcr_packer_globals.current.%s.%s[0] = data_ptr;" % (name,type)
		return

	m = re.search( r"^(MultiTexCoord)([1234])(ub|b|us|s|ui|i|f|d)ARB$", func_name )
	if m :
		k = m.group(1)
		name = 'texCoord'
		type = m.group(3) + m.group(2)
		print "\tcr_packer_globals.current.%s.%s[texture-GL_TEXTURE0_ARB] = data_ptr;" % (name,type)
		return

	m = re.match( r"^(Index)(ub|b|us|s|ui|i|f|d)$", func_name )
	if m :
		k = m.group(1)
		name = 'index'
		type = m.group(2) + "1"
		print "\tcr_packer_globals.current.%s.%s = data_ptr;" % (name,type)
		return

	m = re.match( r"^(EdgeFlag)$", func_name )
	if m :
		k = m.group(1)
		name = 'edgeFlag'
		type = "l1"
		print "\tcr_packer_globals.current.%s.%s = data_ptr;" % (name,type)
		return

	m = re.match( r"^(Begin)$", func_name )
	if m :
		k = m.group(1)
		name = '%s%s' % (k[:1].lower(),k[1:])
		type = ""
		print "\tcr_packer_globals.current.%s_data = data_ptr;" % name
		print "\tcr_packer_globals.current.%s_op = cr_packer_globals.buffer.opcode_current;" % name
		return

for func_name in keys:
	( return_type, arg_names, arg_types ) = gl_mapping[func_name]
	if stub_common.FindSpecial( "packer", func_name ): continue
	is_extended = 0
	if return_type != 'void':
		# Yet another gross hack for glGetString
		if string.find( return_type, '*' ) == -1:
			arg_types.append( "%s *" % return_type )
		else:
			arg_types.append( "%s" % return_type )
		arg_names.append( "return_value" )
	if return_type != 'void' or stub_common.FindSpecial( 'packer_get', func_name ):
		is_extended = 1
		arg_types.append( "int *" )
		arg_names.append( "writeback" )

	def PrintFunc( func_name, arg_names, arg_types, is_extended, is_swapped ):
		if is_swapped:
			print 'void PACK_APIENTRY crPack%sSWAP%s' % ( func_name, stub_common.ArgumentString( arg_names, arg_types ) )
		else:
			print 'void PACK_APIENTRY crPack%s%s' % ( func_name, stub_common.ArgumentString( arg_names, arg_types ) )
		print '{'
		orig_func_name = func_name[:] #make copy
		vector_arg_type = ""
		vector_nelem = stub_common.IsVector( func_name )
		if vector_nelem :
			func_name = SmackVector( func_name )
			vector_arg_type = re.sub( r"\*", "", arg_types[len(arg_types)-1] )
			vector_arg_type = re.sub( "const ", "", vector_arg_type )
			vector_arg_type = string.strip( vector_arg_type )
			packet_length = stub_common.PacketLength( arg_types[:-1] ) + stub_common.WordAlign( vector_nelem * stub_common.lengths[vector_arg_type] )
		else:
			try:
				packet_length = stub_common.PacketLength( arg_types )
			except:
				print >> sys.stderr, "WHY DID THIS FAIL: %s" % `arg_types`

		# do this on the new name so that MultiTexCoords can work out
		if stub_common.FindSpecial( "opcode_extend", func_name ):
			is_extended = 1

		if packet_length == -1:
			print '\tcrError ( "%s needs to be special cased!");' % orig_func_name
			for arg in arg_names:
				print "\t(void) %s;" % arg
		else:
			print "\tunsigned char *data_ptr;"
			if packet_length == 0:
				print "\tGET_BUFFERED_POINTER_NO_ARGS( );"
			else:
				if is_extended:
					packet_length += 8
				print "\tGET_BUFFERED_POINTER( %d );" % packet_length

			UpdateCurrentPointer( func_name )

			counter = 0
			if is_extended:
				counter = 8
				print WriteData( 0, 'GLint', packet_length, is_swapped )
				print WriteData( 4, 'GLenum', stub_common.ExtendedOpcodeName( func_name ), is_swapped )
			for index in range(0,len(arg_names)):
				if vector_nelem and index == len(arg_names)-1:
					for index in range( 0, vector_nelem ):
						print WriteData( counter + index*stub_common.lengths[vector_arg_type], vector_arg_type, arg_names[-1] + ("[%d]" %index), is_swapped )
				elif arg_names[index] != '':
					print WriteData( counter, arg_types[index], arg_names[index], is_swapped )
					if string.find( arg_types[index], '*' ) != -1:
						counter += stub_common.PointerSize()
					else:
						counter += stub_common.lengths[arg_types[index]]
			if is_extended:
				print "\tWRITE_OPCODE( CR_EXTEND_OPCODE );"
			else:
				print "\tWRITE_OPCODE( %s );" % stub_common.OpcodeName( func_name )
		print '}\n'

	PrintFunc( func_name, arg_names, arg_types, is_extended, 0 )
	PrintFunc( func_name, arg_names, arg_types, is_extended, 1 )
