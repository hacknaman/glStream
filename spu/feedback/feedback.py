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

print """#include <stdio.h>
#include "cr_spu.h"
#include "feedbackspu.h"
#include "cr_packfunctions.h"
#include "cr_glstate.h"
#include "cr_glwrapper.h"
#include "feedback.h"


extern void STATE_APIENTRY crStateFeedbackBegin( GLenum mode );
extern void STATE_APIENTRY crStateFeedbackEnd( void );
extern void STATE_APIENTRY crStateFeedbackBitmap( GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap );
extern void STATE_APIENTRY crStateFeedbackDrawPixels( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels );
extern void STATE_APIENTRY crStateFeedbackCopyPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type );
"""
for func_name in keys:
	(return_type, args, types) = gl_mapping[func_name]
	if stub_common.FindSpecial( "feedback_state", func_name ):
		print 'extern %s FEEDBACKSPU_APIENTRY feedbackspu_%s%s;' % ( return_type, func_name, stub_common.ArgumentString( args, types ) )

for func_name in keys:
	(return_type, args, types) = gl_mapping[func_name]
	if stub_common.FindSpecial( "feedback", func_name ):
		print 'extern %s FEEDBACKSPU_APIENTRY feedbackspu_%s%s;' % ( return_type, func_name, stub_common.ArgumentString( args, types ) )
#	if stub_common.FindSpecial( "select", func_name ):
#		print 'extern %s FEEDBACKSPU_APIENTRY feedbackspu_%s%s;' % ( return_type, func_name, stub_common.ArgumentString( args, types ) )

print """
#define CHANGE( name, func ) crSPUChangeInterface( &(feedback_spu.self), feedback_spu.self.name, (SPUGenericFunction) func )
#define CHANGESWAP( name, swapfunc, regfunc ) crSPUChangeInterface( &(feedback_spu.self), feedback_spu.self.name, (SPUGenericFunction) (feedback_spu.swap ? swapfunc: regfunc ) )

static void __loadFeedbackAPI( void )
{
"""
for index in range(len(keys)):
	func_name = keys[index]
	(return_type, args, types) = gl_mapping[func_name]
	if stub_common.FindSpecial( "feedback", func_name ):
		print '\tCHANGE( %s, crStateFeedback%s );' % (func_name, func_name )
print """
}

static void __loadSelectAPI( void )
{
"""
for index in range(len(keys)):
	func_name = keys[index]
	(return_type, args, types) = gl_mapping[func_name]
	if stub_common.FindSpecial( "select", func_name ):
		print '\tCHANGE( %s, crStateSelect%s );' % (func_name, func_name )
	elif stub_common.FindSpecial( "feedback", func_name ):
		print '\tCHANGE( %s, feedbackspu_%s );' % (func_name, func_name )
print """
}

static void __loadRenderAPI( void )
{
"""

for index in range(len(keys)):
	func_name = keys[index]
	(return_type, args, types) = gl_mapping[func_name]
	if stub_common.FindSpecial( "feedback", func_name ):
		print '\tCHANGE( %s, feedbackspu_%s );' % (func_name, func_name )
#	if stub_common.FindSpecial( "select", func_name ):
#		print '\tCHANGE( %s, feedbackspu_%s );' % (func_name, func_name )
print """
}
"""

for index in range(len(keys)):
	func_name = keys[index]
	(return_type, args, types) = gl_mapping[func_name]
	if stub_common.FindSpecial( "feedback", func_name ):
		print '%s FEEDBACKSPU_APIENTRY feedbackspu_%s%s' % ( return_type, func_name, stub_common.ArgumentString( args, types ) )
		print '{'
		print '\tfeedback_spu.super.%s%s;' % ( func_name, stub_common.CallString( args ) )
		print '}'
#	if stub_common.FindSpecial( "select", func_name ):
#		print '%s FEEDBACKSPU_APIENTRY feedbackspu_%s%s' % ( return_type, func_name, stub_common.ArgumentString( args, types ) )
#		print '{'
#		print '\tfeedback_spu.super.%s%s;' % ( func_name, stub_common.CallString( args ) )
#		print '}'

print """
GLint FEEDBACKSPU_APIENTRY feedbackspu_RenderMode ( GLenum mode )
{
	feedback_spu.render_mode = mode;

	switch (mode) {
		case GL_FEEDBACK:
			/* printf("Switching to Feedback API\n"); */
			__loadFeedbackAPI( );
			break;
		case GL_SELECT:
			/* printf("Switching to Selection API\n"); */
			__loadSelectAPI( );
			break;
		case GL_RENDER:
			/* printf("Switching to Render API\n"); */
			__loadRenderAPI( );
			break;
	}

	return crStateRenderMode( mode );
}

void FEEDBACKSPU_APIENTRY feedbackspu_Begin ( GLenum mode )
{
	crStateBegin( mode );

	if (feedback_spu.render_mode == GL_FEEDBACK)
		crStateFeedbackBegin( mode );
	else
		feedback_spu.super.Begin( mode );
}

void FEEDBACKSPU_APIENTRY feedbackspu_End ( void )
{
	crStateEnd( );

	if (feedback_spu.render_mode == GL_FEEDBACK)
		crStateFeedbackEnd( );
	else
		feedback_spu.super.End( );
}

void FEEDBACKSPU_APIENTRY feedbackspu_Bitmap ( GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap )
{
	crStateBitmap( width, height, xorig, yorig, xmove, ymove, bitmap );

	if (feedback_spu.render_mode == GL_FEEDBACK)
		crStateFeedbackBitmap( width, height, xorig, yorig, xmove, ymove, bitmap );
	else
		feedback_spu.super.Bitmap( width, height, xorig, yorig, xmove, ymove, bitmap );
}

void FEEDBACKSPU_APIENTRY feedbackspu_CopyPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type )
{
	if (feedback_spu.render_mode == GL_FEEDBACK)
		crStateFeedbackCopyPixels( x, y, width, height, type );
	else
		feedback_spu.super.CopyPixels( x, y, width, height, type );
}

void FEEDBACKSPU_APIENTRY feedbackspu_DrawPixels( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels )
{
	if (feedback_spu.render_mode == GL_FEEDBACK)
		crStateFeedbackDrawPixels( width, height, format, type, pixels );
	else
		feedback_spu.super.DrawPixels( width, height, format, type, pixels );
}

SPUNamedFunctionTable feedback_table[] = {
"""

for index in range(len(keys)):
	func_name = keys[index]
	(return_type, args, types) = gl_mapping[func_name]
	if stub_common.FindSpecial( "feedback_state", func_name ):
		print '\t{ "%s", (SPUGenericFunction) feedbackspu_%s }, ' % ( func_name, func_name )
print """
	{ "FeedbackBuffer", (SPUGenericFunction) crStateFeedbackBuffer },
	{ "SelectBuffer", (SPUGenericFunction) crStateSelectBuffer },
	{ "InitNames", (SPUGenericFunction) crStateInitNames },
	{ "LoadName", (SPUGenericFunction) crStateLoadName },
	{ "PushName", (SPUGenericFunction) crStatePushName },
	{ "PopName", (SPUGenericFunction) crStatePopName },
	{ "Begin", (SPUGenericFunction) feedbackspu_Begin },
	{ "End", (SPUGenericFunction) feedbackspu_End },
	{ "Bitmap", (SPUGenericFunction) feedbackspu_Bitmap },
	{ "CopyPixels", (SPUGenericFunction) feedbackspu_CopyPixels },
	{ "DrawPixels", (SPUGenericFunction) feedbackspu_DrawPixels },
	{ "TexCoord1d", (SPUGenericFunction) feedbackspu_TexCoord1d },
	{ "TexCoord1dv", (SPUGenericFunction) feedbackspu_TexCoord1dv },
	{ "TexCoord1f", (SPUGenericFunction) feedbackspu_TexCoord1f },
	{ "TexCoord1fv", (SPUGenericFunction) feedbackspu_TexCoord1fv },
	{ "TexCoord1s", (SPUGenericFunction) feedbackspu_TexCoord1s },
	{ "TexCoord1sv", (SPUGenericFunction) feedbackspu_TexCoord1sv },
	{ "TexCoord1i", (SPUGenericFunction) feedbackspu_TexCoord1i },
	{ "TexCoord1iv", (SPUGenericFunction) feedbackspu_TexCoord1iv },
	{ "TexCoord2d", (SPUGenericFunction) feedbackspu_TexCoord2d },
	{ "TexCoord2dv", (SPUGenericFunction) feedbackspu_TexCoord2dv },
	{ "TexCoord2f", (SPUGenericFunction) feedbackspu_TexCoord2f },
	{ "TexCoord2fv", (SPUGenericFunction) feedbackspu_TexCoord2fv },
	{ "TexCoord2s", (SPUGenericFunction) feedbackspu_TexCoord2s },
	{ "TexCoord2sv", (SPUGenericFunction) feedbackspu_TexCoord2sv },
	{ "TexCoord2i", (SPUGenericFunction) feedbackspu_TexCoord2i },
	{ "TexCoord2iv", (SPUGenericFunction) feedbackspu_TexCoord2iv },
	{ "TexCoord3d", (SPUGenericFunction) feedbackspu_TexCoord3d },
	{ "TexCoord3dv", (SPUGenericFunction) feedbackspu_TexCoord3dv },
	{ "TexCoord3f", (SPUGenericFunction) feedbackspu_TexCoord3f },
	{ "TexCoord3fv", (SPUGenericFunction) feedbackspu_TexCoord3fv },
	{ "TexCoord3s", (SPUGenericFunction) feedbackspu_TexCoord3s },
	{ "TexCoord3sv", (SPUGenericFunction) feedbackspu_TexCoord3sv },
	{ "TexCoord3i", (SPUGenericFunction) feedbackspu_TexCoord3i },
	{ "TexCoord3iv", (SPUGenericFunction) feedbackspu_TexCoord3iv },
	{ "TexCoord4d", (SPUGenericFunction) feedbackspu_TexCoord4d },
	{ "TexCoord4dv", (SPUGenericFunction) feedbackspu_TexCoord4dv },
	{ "TexCoord4f", (SPUGenericFunction) feedbackspu_TexCoord4f },
	{ "TexCoord4fv", (SPUGenericFunction) feedbackspu_TexCoord4fv },
	{ "TexCoord4s", (SPUGenericFunction) feedbackspu_TexCoord4s },
	{ "TexCoord4sv", (SPUGenericFunction) feedbackspu_TexCoord4sv },
	{ "TexCoord4i", (SPUGenericFunction) feedbackspu_TexCoord4i },
	{ "TexCoord4iv", (SPUGenericFunction) feedbackspu_TexCoord4iv },
	{ "RenderMode", (SPUGenericFunction) feedbackspu_RenderMode },
	{ NULL, NULL }
};
"""
