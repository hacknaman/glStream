# Copyright (c) 2001, Stanford University
# All rights reserved.
#
# See the file LICENSE.txt for information on redistributing this software.


import sys

sys.path.append("../glapi_parser")
import apiutil


def GenerateEntrypoints():

	apiutil.CopyrightC()

	print '#include "chromium.h"'
	print '#include "stub.h"'

	print ''
	
	print "/* DO NOT EDIT - THIS FILE GENERATED BY THE windows_exports_x64.py SCRIPT */"


	# Get sorted list of dispatched functions.
	# The order is very important - it must match cr_opcodes.h
	# and spu_dispatch_table.h
	keys = apiutil.GetDispatchedFunctions("../glapi_parser/APIspec.txt")

	for index in range(len(keys)):
		func_name = keys[index]
		if apiutil.Category(func_name) == "Chromium":
			continue

		return_type = apiutil.ReturnType(func_name)
		params = apiutil.Parameters(func_name)

		print "extern %s APIENTRY cr_gl%s( %s )" % (return_type, func_name,
									  apiutil.MakeDeclarationString( params ))
		print "{"
		if return_type == "void":
			print "\tglim.%s(%s);" % (func_name,apiutil.MakeCallString(params))
		else:
			print "\t return glim.%s(%s);" % (func_name,apiutil.MakeCallString(params))
		print "}"
		print ""

	print '/*'
	print '* Aliases'
	print '*/'

	# Now loop over all the functions and take care of any aliases
	allkeys = apiutil.GetAllFunctions("../glapi_parser/APIspec.txt")
	for func_name in allkeys:
		if "omit" in apiutil.ChromiumProps(func_name):
			continue

		if func_name in keys:
			# we already processed this function earlier
			continue

		# alias is the function we're aliasing
		alias = apiutil.Alias(func_name)
		if alias:
			return_type = apiutil.ReturnType(func_name)
			params = apiutil.Parameters(func_name)
			print "extern %s APIENTRY cr_gl%s( %s )" % (return_type, func_name,
								apiutil.MakeDeclarationString( params ))
			print "{"
			if return_type == "void":
				print "\t glim.%s(%s);" % (alias,apiutil.MakeCallString(params))
			else:
				print "\t return glim.%s(%s);" % (alias,apiutil.MakeCallString(params))
			print "}"
			print ""


	print '/*'
	print '* No-op stubs'
	print '*/'

	# Now generate no-op stub functions
	for func_name in allkeys:
		if "stub" in apiutil.ChromiumProps(func_name):
			return_type = apiutil.ReturnType(func_name)
			params = apiutil.Parameters(func_name)
			print "extern %s APIENTRY cr_gl%s( %s )" % (return_type, func_name, apiutil.MakeDeclarationString(params))
			print "{"
			if return_type != "void":
				print "return (%s) 0" % return_type
			print "}"
			print ""


		

GenerateEntrypoints()

