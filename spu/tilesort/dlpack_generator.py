# Copyright (c) 2001, Stanford University
# All rights reserved.
#
# See the file LICENSE.txt for information on redistributing this software.

import sys, os

sys.path.append( "../../glapi_parser" )
import apiutil

# "mode" is source, header, or table
mode = sys.argv[1]

keys = apiutil.GetDispatchedFunctions("../../glapi_parser/APIspec.txt")
num_funcs = len(keys)

apiutil.CopyrightC()

print '/* DO NOT EDIT.  This code is generated by %s. */' % os.path.basename(sys.argv[0])
print

# Print the appropriate header.
if mode == 'source':
    print """
#include <stdio.h>
#include "cr_server.h"
#include "cr_packfunctions.h"
#include "tilesortspu.h"
#include "cr_dlm.h"
#include "tilesortspu_dlpack_gen.h"
"""
    lastWasComment = 0
elif mode == 'table':
    print """
#include <stdio.h>
#include "cr_string.h"
#include "cr_spu.h"
#include "cr_packfunctions.h"
#include "cr_glstate.h"
#include "tilesortspu.h"
#include "tilesortspu_dlpack_gen.h"

void tilesortspuLoadPackTable(SPUDispatchTable *t)
{"""
elif mode == 'header':
    pass
else:
    raise "unknown generation mode '%s'" % mode

for func_name in keys:
    return_type = apiutil.ReturnType(func_name)
    chromiumProps = apiutil.ChromiumProps(func_name)
    params = apiutil.Parameters(func_name)
    declarationString = apiutil.MakeDeclarationString(params)
    basicCallString = apiutil.MakeCallString(params)

    # These functions will never appear in a display list (they
    # are expanded into other function calls while they're being
    # compiled), so we need not implement them.
    if apiutil.FindSpecial("tilesort_dlpack_ignore", func_name):
	if mode == 'source': 
	    print '/* %s not generated: ignored */' % func_name
	    lastWasComment = 1
	elif mode == 'table':
	    print '\tt->%s = NULL;' % (func_name)
	elif mode == 'header':
	    pass
	continue

    # Two special functions that are not compilable but are still
    # required for display list creation exist; we have to make sure
    # their pack versions end up in the pack dispatch table.
    if func_name in ['NewList', 'EndList']:
	if mode == 'table':
	    print '\tt->%s = tilesort_spu.swap ? crPack%sSWAP : crPack%s;' % (func_name, func_name, func_name)
	continue

    # The packer dispatch table is used only for replaying display lists.
    # It can safely have NULL entries in all the non-compilable functions.
    if not apiutil.CanCompile(func_name):
	if mode == 'source':
	    print '/* %s not generated: not a compilable function */' % func_name
	    lastWasComment = 1
	elif mode == 'table':
	    print '\tt->%s = NULL;' % func_name
	elif mode == 'header':
	    pass
	continue

    # All entries should be packable.  If they're not, we're in trouble.
    if not apiutil.CanPack(func_name):
	print '#error %s is compilable, but not packable?!?' % func_name
	continue

    # The only functions that have to be generated are the ones
    # that need client-side state to unpack their data.  All
    # others will work just fine with the standard crPack
    # routines.
    if not apiutil.UsesClientState(func_name):
	if mode == 'table':
	    print '\tt->%s = tilesort_spu.swap ? crPack%sSWAP : crPack%s;' % (func_name, func_name, func_name)
	continue

    # Here, we need to generate a function.
    if mode == 'source':
	if lastWasComment: 
	    print ''
	    lastWasComment = 0
	print '%s TILESORTSPU_APIENTRY tilesortspu_Pack%s( %s )' % ( return_type, func_name, declarationString )
	print '{'
	print '\tcrPack%s(%s, &crStateNativePixelPacking);' % (func_name, basicCallString)
	print '}'
	print '%s TILESORTSPU_APIENTRY tilesortspu_Pack%sSWAP( %s )' % ( return_type, func_name, declarationString )
	print '{'
	print '\tcrPack%sSWAP(%s, &crStateNativePixelPacking);' % (func_name, basicCallString)
	print '}'
	print
    elif mode == 'header':
	print 'extern %s TILESORTSPU_APIENTRY tilesortspu_Pack%s( %s );' % ( return_type, func_name, declarationString )
	print 'extern %s TILESORTSPU_APIENTRY tilesortspu_Pack%sSWAP( %s );' % ( return_type, func_name, declarationString )
    elif mode == 'table':
	print '\tt->%s = tilesort_spu.swap ? tilesortspu_Pack%sSWAP : tilesortspu_Pack%s;' % (func_name, func_name, func_name)

# Done with the whole file.  Print out any ending necessary.
if mode == 'source':
    pass
elif mode == 'table':
    print '}'
elif mode == 'header':
    pass
