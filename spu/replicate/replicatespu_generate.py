# Copyright (c) 2001, Stanford University
# All rights reserved.
#
# See the file LICENSE.txt for information on redistributing this software.

import sys

sys.path.append( "../../glapi_parser" )
import apiutil


keys = apiutil.GetDispatchedFunctions("../../glapi_parser/APIspec.txt")


apiutil.CopyrightC()

print """
#include <stdio.h>
#include "cr_server.h"
#include "cr_packfunctions.h"
#include "replicatespu.h"
#include "cr_dlm.h"
#include "replicatespu_proto.h"

/* DO NOT EDIT.  This code is generated by replicatespu_generate.py. */
"""

lastWasComment = 0
for func_name in keys:
	return_type = apiutil.ReturnType(func_name)
	chromiumProps = apiutil.ChromiumProps(func_name)
	params = apiutil.Parameters(func_name)

	# Skip this function if we have a non-generated implementation
	if apiutil.FindSpecial("replicate", func_name):
	    print '/* %s not generated: special implementation exists */' % func_name
	    lastWasComment = 1
	    continue

	# Also skip this function if there's a trivial implementation
	# (i.e. one that degenerates to solely the use of crPack functions)

	# Otherwise, generate the function body.

	# The outgoing call string is based on the parameters.  State-based
	# functions and pixelstore-based functions will require extended
	# call strings.
	basicCallString = apiutil.MakeCallString(params)
	needDL = 0
	needThread = 0
	needClientState = 0
	needFlush = 0
	needTrackState = 0
	needWriteback = 0
	needPack = apiutil.CanPack(func_name)
	packCallString = basicCallString
	dlmCallString = basicCallString

	# Queries can either be handled by a pack/writeback, or by
	# a crState call, depending on whether the query is
	# server-dependent or not.
	if "serverdependent" in chromiumProps:
	    needPack = 1
	    needWriteback = 1
	    needFlush = 1
	    needThread = 1
	    if return_type == 'void':
		packCallString = packCallString + ', &writeback'
	    else:
		packCallString = packCallString + ', &return_val, &writeback'
	elif apiutil.IsQuery(func_name):
	    needTrackState = 1
	    needPack = 0

	# These functions have to check the display list state to see
	# whether they should be compiled or executed.
	if apiutil.CanCompile(func_name):
	    needDL = 1
	    needThread = 1

	if apiutil.UsesClientState(func_name):
	    if needPack:
		needClientState = 1
		needThread = 1
		packCallString = basicCallString + ", &(clientState->unpack)"
	    if needDL:
		needClientState = 1
		needThread = 1
		dlmCallString = basicCallString + ", clientState"

	# Some functions need to be flushed as soon as they're done.
	if apiutil.FindSpecial("replicatespu_flush", func_name):
	    needFlush = 1
	    needThread = 1

	# Any functions that change state will also need to invoke the
	# state tracker.  Client-side state is managed only on the client
	# side - we don't pack it, even if we can.
	if apiutil.SetsTrackedState(func_name):
	    needTrackState = 1
	    if apiutil.SetsClientState(func_name):
		needPack = 0

	# If we have a trivial implementation (i.e. we need no special processing,
	# so that the function can be handled completely with crPack), bail out.
	# The appropriate crPack functions will be installed in the SPU instead.
	if not needDL and not needThread and not needClientState and not needFlush and not needTrackState and not needWriteback and needPack:
	    print '/* %s not generated: crPack implementation suffices */' % func_name
	    lastWasComment = 1
	    continue

	# Otherwise, start creating the function.
	if lastWasComment: 
	    print ''
	    lastWasComment = 0

	print '%s REPLICATESPU_APIENTRY replicatespu_%s( %s )' % ( return_type, func_name, apiutil.MakeDeclarationString( params ) )
	print '{'

	# We'll need the thread if we need DL or client state
	if needThread:
	    print '\tGET_THREAD(thread);'

	if needClientState:
	    print "\tContextInfo *ctx = thread->currentContext;"
	    print "\tCRClientState *clientState = &(ctx->State->client);";

	if needWriteback:
	    print '\tint writeback = 1;'
	    if return_type != 'void':
		print '\t%s return_val = (%s) 0;' % (return_type, return_type)

	# If this element can be compiled into a display list, we have
	# to check to see if it should be compiled first.  For most
	# functions, the 'needDL' check is sufficient; but some functions
	# have parameter combinations that sometimes are compiled into
	# display lists, and sometimes are executed immediately.  Note
	# that all primitives are still packed, even if the display list
	# mode is GL_COMPILE; that ensures that they get sent to the servers,
	# where they will be compiled as needed.
	if needDL:
	    if "checklist" in chromiumProps:
		print '\tif (thread->currentContext->displayListMode != GL_FALSE &&'
		print '\t\tcrDLMCheckList%s(%s)) {' % (func_name, basicCallString)
	    else:
		print '\tif (thread->currentContext->displayListMode != GL_FALSE) {'
	    print '\t\tcrDLMCompile%s(%s);' % (func_name, dlmCallString)
	    print '\t}'

	# Pack it up to send it away
	if needPack:
	    print '\tif (replicate_spu.swap)'
	    print '\t{'
	    print '\t\tcrPack%sSWAP(%s);' % (func_name, packCallString)
	    print '\t}'
	    print '\telse'
	    print '\t{'
	    print '\t\tcrPack%s(%s);' % (func_name, packCallString)
	    print '\t}'
	
	# Sometimes we'll need to invoke an immediate flush
	if needFlush:
	    print '\treplicatespuFlush( (void *) thread );'

	# If it's a state-affecting function, *and* if we're not compiling
	# a display list, send it off to the state tracker.
	# state tracker
	if needTrackState:
	    if needDL:
		# All of these return void, so we don't need the split below.
		print '\tif (thread->currentContext->displayListMode != GL_COMPILE) {'
		print '\t\tcrState%s( %s );' % (func_name, basicCallString)	
		print '\t}'
	    elif return_type == 'void':
		print '\tcrState%s( %s );' % ( func_name, basicCallString)
	    else:
		print '\treturn crState%s( %s );' % ( func_name, basicCallString)

	# If it's a writeback-requiring Get function, wait for the data to be written.
	if needWriteback:
	    print '\twhile (writeback)'
	    print '\t\tcrNetRecv();'
	    if return_type != 'void':
		print '\tif (replicate_spu.swap)'
		print '\t{'
		print '\t\treturn_val = (%s) SWAP32(return_val);' % return_type
		print '\t}'
		print '\treturn return_val;'

	# All done with this function.
	print '}'
	print ''

# Done with the whole file
