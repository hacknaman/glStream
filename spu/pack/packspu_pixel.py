# Copyright (c) 2001, Stanford University
# All rights reserved.
#
# See the file LICENSE.txt for information on redistributing this software.

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

stub_common.CopyrightC()

print """/* DO NOT EDIT - AUTOMATICALLY GENERATED BY packspu_pixel.py */
#include <stdio.h>
#include "packspu.h"
#include "cr_packfunctions.h"
#include "cr_glwrapper.h"
#include "cr_glstate.h"
#include "cr_pixeldata.h"

void PACKSPU_APIENTRY packspu_DrawPixels( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels )
{
	GET_THREAD(thread);
	ContextInfo *ctx = thread->currentContext;
	CRClientState *clientState = &(ctx->clientState->client);

	if (crImageSize(format, type, width, height) > thread->server.conn->mtu ) {
		crError("DrawPixels called with insufficient MTU size\n"
			"Needed %d, but currently MTU is set at %d\n", 
			crImageSize(format, type, width, height), 
			thread->server.conn->mtu );
		return;
	}

	if (pack_spu.swap)
	{
		crPackDrawPixelsSWAP( width, height, format, type, pixels, &(clientState->unpack) );
	}
	else
	{
		crPackDrawPixels( width, height, format, type, pixels, &(clientState->unpack) );
	}
}

void PACKSPU_APIENTRY packspu_ReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels )
{
	GET_THREAD(thread);
	ContextInfo *ctx = thread->currentContext;
	CRClientState *clientState = &(ctx->clientState->client);
	pack_spu.ReadPixels++;
	if (pack_spu.swap)
	{
		crPackReadPixelsSWAP( x, y, width, height, format, type, pixels,
							&(clientState->pack) );
	}
	else
	{
		crPackReadPixels( x, y, width, height, format, type, pixels,
							&(clientState->pack) );
	}
	packspuFlush( (void *) thread );
	while (pack_spu.ReadPixels) 
		crNetRecv();
}

void PACKSPU_APIENTRY packspu_CopyPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type )
{
	GET_THREAD(thread);
	if (pack_spu.swap)
	{
		crPackCopyPixelsSWAP( x, y, width, height, type );
	}
	else
	{
		crPackCopyPixels( x, y, width, height, type );
	}
	packspuFlush( (void *) thread );
}

void PACKSPU_APIENTRY packspu_PixelStoref( GLenum pname, GLfloat param )
{
	crStatePixelStoref( pname, param );
	if (pack_spu.swap)
	{
		crPackPixelStorefSWAP( pname, param );
	}
	else
	{
		crPackPixelStoref( pname, param );
	}
}

void PACKSPU_APIENTRY packspu_PixelStorei( GLenum pname, GLint param )
{
	crStatePixelStorei( pname, param );
	if (pack_spu.swap)
	{
		crPackPixelStoreiSWAP( pname, param );
	}
	else
	{
		crPackPixelStorei( pname, param );
	}
}
"""

for func_name in stub_common.AllSpecials( "packspu_pixel" ):
	(return_type, args, types) = gl_mapping[func_name]
	print 'void PACKSPU_APIENTRY packspu_%s%s' % ( func_name, stub_common.ArgumentString( args, types ) )
	print '{'
	args.append( '&(clientState->unpack)' )
	print '\tGET_CONTEXT(ctx);'
	print '\tCRClientState *clientState = &(ctx->clientState->client);'
	print '\tif (pack_spu.swap)'
	print '\t{'
	print '\t\tcrPack%sSWAP%s;' % ( func_name, stub_common.CallString( args ) )
	print '\t}'
	print '\telse'
	print '\t{'
	print '\t\tcrPack%s%s;' % ( func_name, stub_common.CallString( args ) )
	print '\t}'
	print '}'
