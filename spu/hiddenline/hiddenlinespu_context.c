/* Copyright (c) 2001, Stanford University
	All rights reserved.

	See the file LICENSE.txt for information on redistributing this software. */
	
#include <stdio.h>
#include "hiddenlinespu.h"
#include "cr_packfunctions.h"
#include "cr_mem.h"

void HIDDENLINESPU_APIENTRY hiddenlinespu_MakeCurrent(GLint crWindow, GLint nativeWindow, GLint ctx)
{
	ContextInfo *context;

	/* look up the context */
	context = crHashtableSearch(hiddenline_spu.contextTable, ctx);

	/* Set current context pointer */
#ifdef CHROMIUM_THREADSAFE
	crSetTSD(&_HiddenlineTSD, context);
#else
	hiddenline_spu.currentContext = context;
#endif

	if (context)
	{
		crPackSetContext( context->packer );
		crStateMakeCurrent( context->ctx );
		hiddenline_spu.super.MakeCurrent(crWindow, nativeWindow, context->super_context);
	}
	else
	{
		crPackSetContext( NULL );
		crStateMakeCurrent( NULL );
		hiddenline_spu.super.MakeCurrent(crWindow, nativeWindow, 0); /* -1? */
	}
}


GLint HIDDENLINESPU_APIENTRY hiddenlinespu_CreateContext( const char *dpyName, GLint visBits )
{
	static int freeContext = 0;  /* unique context id generator */
	ContextInfo *context;

	/* allocate new ContextInfo */
	context = (ContextInfo *) crAlloc(sizeof(ContextInfo));
	if (!context)
		return 0;

	/* init ContextInfo */
	context->packer = crPackNewContext( 0 ); /* no byte swapping */
	/* context->pack_buffer initialized in hiddenlineProvidePackBuffer() */
	context->ctx = crStateCreateContext( &hiddenline_spu.limits );
	context->super_context = hiddenline_spu.super.CreateContext(dpyName, visBits);
	context->clear_color.r = 0.0f;
	context->clear_color.g = 0.0f;
	context->clear_color.b = 0.0f;
	context->clear_color.a = 0.0f;
	context->frame_head = NULL;
	context->frame_tail = NULL;

	/* put context into hash table */
	freeContext++;
	crHashtableAdd(hiddenline_spu.contextTable, freeContext, (void *) context);

	crBufferPoolInit( &(context->bufpool), 16 );
	crPackFlushFunc(context->packer, hiddenlineFlush);
	crPackSendHugeFunc(context->packer, hiddenlineHuge);

	return freeContext;
}

void HIDDENLINESPU_APIENTRY hiddenlinespu_DestroyContext( GLint ctx )
{
	ContextInfo *context;
	context = crHashtableSearch(hiddenline_spu.contextTable, ctx);
	CRASSERT(context);
	hiddenline_spu.super.DestroyContext(context->super_context);
	crHashtableDelete(hiddenline_spu.contextTable, ctx);
}
