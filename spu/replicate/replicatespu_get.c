/* Copyright (c) 2001, Stanford University
	All rights reserved.

	See the file LICENSE.txt for information on redistributing this software. */
	

/* 
 * The ReplicateSPU currently doesn't support these nVidia extensions.
 * A little work is needed to support them.
 */

#include "replicatespu_proto.h"
#include "replicatespu.h"
#include "cr_packfunctions.h"
#include "cr_net.h"

GLboolean REPLICATESPU_APIENTRY replicatespu_AreProgramsResidentNV( GLsizei n, const GLuint *ids, GLboolean *residences )
{
	GET_THREAD(thread);
	int writeback = 1;
	GLboolean return_val = (GLboolean) 0;
	if (replicate_spu.swap)
	{
		crPackAreProgramsResidentNVSWAP( n, ids, residences, &return_val, &writeback );
	}
	else
	{
		crPackAreProgramsResidentNV( n, ids, residences, &return_val, &writeback );
	}
	replicatespuFlush( (void *) thread );
	while (writeback)
		crNetRecv();
	if (replicate_spu.swap)
	{
		return_val = (GLboolean) SWAP32(return_val);
	}
	return return_val;
}

void REPLICATESPU_APIENTRY replicatespu_GenFencesNV( GLsizei n, GLuint *fences )
{
	GET_THREAD(thread);
	int writeback = 1;
	if (replicate_spu.swap)
	{
		crPackGenFencesNVSWAP( n, fences, &writeback );
	}
	else
	{
		crPackGenFencesNV( n, fences, &writeback );
	}
	replicatespuFlush( (void *) thread );
	while (writeback)
		crNetRecv();
}

void REPLICATESPU_APIENTRY replicatespu_GenProgramsNV( GLsizei n, GLuint *ids )
{
	GET_THREAD(thread);
	int writeback = 1;
	if (replicate_spu.swap)
	{
		crPackGenProgramsNVSWAP( n, ids, &writeback );
	}
	else
	{
		crPackGenProgramsNV( n, ids, &writeback );
	}
	replicatespuFlush( (void *) thread );
	while (writeback)
		crNetRecv();
}

void REPLICATESPU_APIENTRY replicatespu_GetChromiumParametervCR( GLenum target, GLuint index, GLenum type, GLsizei count, GLvoid *values )
{
	GET_THREAD(thread);
	int writeback = 1;
	if (replicate_spu.swap)
	{
		crPackGetChromiumParametervCRSWAP( target, index, type, count, values, &writeback );
	}
	else
	{
		crPackGetChromiumParametervCR( target, index, type, count, values, &writeback );
	}
	replicatespuFlush( (void *) thread );
	while (writeback)
		crNetRecv();
}

void REPLICATESPU_APIENTRY replicatespu_GetCombinerInputParameterfvNV( GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLfloat *params )
{
	GET_THREAD(thread);
	int writeback = 1;
	if (replicate_spu.swap)
	{
		crPackGetCombinerInputParameterfvNVSWAP( stage, portion, variable, pname, params, &writeback );
	}
	else
	{
		crPackGetCombinerInputParameterfvNV( stage, portion, variable, pname, params, &writeback );
	}
	replicatespuFlush( (void *) thread );
	while (writeback)
		crNetRecv();
}

void REPLICATESPU_APIENTRY replicatespu_GetCombinerInputParameterivNV( GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLint *params )
{
	GET_THREAD(thread);
	int writeback = 1;
	if (replicate_spu.swap)
	{
		crPackGetCombinerInputParameterivNVSWAP( stage, portion, variable, pname, params, &writeback );
	}
	else
	{
		crPackGetCombinerInputParameterivNV( stage, portion, variable, pname, params, &writeback );
	}
	replicatespuFlush( (void *) thread );
	while (writeback)
		crNetRecv();
}

void REPLICATESPU_APIENTRY replicatespu_GetCombinerOutputParameterfvNV( GLenum stage, GLenum portion, GLenum pname, GLfloat *params )
{
	GET_THREAD(thread);
	int writeback = 1;
	if (replicate_spu.swap)
	{
		crPackGetCombinerOutputParameterfvNVSWAP( stage, portion, pname, params, &writeback );
	}
	else
	{
		crPackGetCombinerOutputParameterfvNV( stage, portion, pname, params, &writeback );
	}
	replicatespuFlush( (void *) thread );
	while (writeback)
		crNetRecv();
}

void REPLICATESPU_APIENTRY replicatespu_GetCombinerOutputParameterivNV( GLenum stage, GLenum portion, GLenum pname, GLint *params )
{
	GET_THREAD(thread);
	int writeback = 1;
	if (replicate_spu.swap)
	{
		crPackGetCombinerOutputParameterivNVSWAP( stage, portion, pname, params, &writeback );
	}
	else
	{
		crPackGetCombinerOutputParameterivNV( stage, portion, pname, params, &writeback );
	}
	replicatespuFlush( (void *) thread );
	while (writeback)
		crNetRecv();
}

void REPLICATESPU_APIENTRY replicatespu_GetCombinerStageParameterfvNV( GLenum stage, GLenum pname, GLfloat *params )
{
	GET_THREAD(thread);
	int writeback = 1;
	if (replicate_spu.swap)
	{
		crPackGetCombinerStageParameterfvNVSWAP( stage, pname, params, &writeback );
	}
	else
	{
		crPackGetCombinerStageParameterfvNV( stage, pname, params, &writeback );
	}
	replicatespuFlush( (void *) thread );
	while (writeback)
		crNetRecv();
}

void REPLICATESPU_APIENTRY replicatespu_GetFenceivNV( GLuint fence, GLenum pname, GLint *params )
{
	GET_THREAD(thread);
	int writeback = 1;
	if (replicate_spu.swap)
	{
		crPackGetFenceivNVSWAP( fence, pname, params, &writeback );
	}
	else
	{
		crPackGetFenceivNV( fence, pname, params, &writeback );
	}
	replicatespuFlush( (void *) thread );
	while (writeback)
		crNetRecv();
}

void REPLICATESPU_APIENTRY replicatespu_GetFinalCombinerInputParameterfvNV( GLenum variable, GLenum pname, GLfloat *params )
{
	GET_THREAD(thread);
	int writeback = 1;
	if (replicate_spu.swap)
	{
		crPackGetFinalCombinerInputParameterfvNVSWAP( variable, pname, params, &writeback );
	}
	else
	{
		crPackGetFinalCombinerInputParameterfvNV( variable, pname, params, &writeback );
	}
	replicatespuFlush( (void *) thread );
	while (writeback)
		crNetRecv();
}

void REPLICATESPU_APIENTRY replicatespu_GetFinalCombinerInputParameterivNV( GLenum variable, GLenum pname, GLint *params )
{
	GET_THREAD(thread);
	int writeback = 1;
	if (replicate_spu.swap)
	{
		crPackGetFinalCombinerInputParameterivNVSWAP( variable, pname, params, &writeback );
	}
	else
	{
		crPackGetFinalCombinerInputParameterivNV( variable, pname, params, &writeback );
	}
	replicatespuFlush( (void *) thread );
	while (writeback)
		crNetRecv();
}

void REPLICATESPU_APIENTRY replicatespu_GetProgramParameterdvNV( GLenum target, GLuint index, GLenum pname, GLdouble *params )
{
	GET_THREAD(thread);
	int writeback = 1;
	if (replicate_spu.swap)
	{
		crPackGetProgramParameterdvNVSWAP( target, index, pname, params, &writeback );
	}
	else
	{
		crPackGetProgramParameterdvNV( target, index, pname, params, &writeback );
	}
	replicatespuFlush( (void *) thread );
	while (writeback)
		crNetRecv();
}

void REPLICATESPU_APIENTRY replicatespu_GetProgramParameterfvNV( GLenum target, GLuint index, GLenum pname, GLfloat *params )
{
	GET_THREAD(thread);
	int writeback = 1;
	if (replicate_spu.swap)
	{
		crPackGetProgramParameterfvNVSWAP( target, index, pname, params, &writeback );
	}
	else
	{
		crPackGetProgramParameterfvNV( target, index, pname, params, &writeback );
	}
	replicatespuFlush( (void *) thread );
	while (writeback)
		crNetRecv();
}

void REPLICATESPU_APIENTRY replicatespu_GetProgramStringNV( GLuint id, GLenum pname, GLubyte *program )
{
	GET_THREAD(thread);
	int writeback = 1;
	if (replicate_spu.swap)
	{
		crPackGetProgramStringNVSWAP( id, pname, program, &writeback );
	}
	else
	{
		crPackGetProgramStringNV( id, pname, program, &writeback );
	}
	replicatespuFlush( (void *) thread );
	while (writeback)
		crNetRecv();
}

void REPLICATESPU_APIENTRY replicatespu_GetProgramivNV( GLuint id, GLenum pname, GLint *params )
{
	GET_THREAD(thread);
	int writeback = 1;
	if (replicate_spu.swap)
	{
		crPackGetProgramivNVSWAP( id, pname, params, &writeback );
	}
	else
	{
		crPackGetProgramivNV( id, pname, params, &writeback );
	}
	replicatespuFlush( (void *) thread );
	while (writeback)
		crNetRecv();
}

void REPLICATESPU_APIENTRY replicatespu_GetTrackMatrixivNV( GLenum target, GLuint address, GLenum pname, GLint *params )
{
	GET_THREAD(thread);
	int writeback = 1;
	if (replicate_spu.swap)
	{
		crPackGetTrackMatrixivNVSWAP( target, address, pname, params, &writeback );
	}
	else
	{
		crPackGetTrackMatrixivNV( target, address, pname, params, &writeback );
	}
	replicatespuFlush( (void *) thread );
	while (writeback)
		crNetRecv();
}

void REPLICATESPU_APIENTRY replicatespu_GetVertexAttribPointervNV( GLuint index, GLenum pname, GLvoid **pointer )
{
	GET_THREAD(thread);
	int writeback = 1;
	if (replicate_spu.swap)
	{
		crPackGetVertexAttribPointervNVSWAP( index, pname, pointer, &writeback );
	}
	else
	{
		crPackGetVertexAttribPointervNV( index, pname, pointer, &writeback );
	}
	replicatespuFlush( (void *) thread );
	while (writeback)
		crNetRecv();
}

void REPLICATESPU_APIENTRY replicatespu_GetVertexAttribdvNV( GLuint index, GLenum pname, GLdouble *params )
{
	GET_THREAD(thread);
	int writeback = 1;
	if (replicate_spu.swap)
	{
		crPackGetVertexAttribdvNVSWAP( index, pname, params, &writeback );
	}
	else
	{
		crPackGetVertexAttribdvNV( index, pname, params, &writeback );
	}
	replicatespuFlush( (void *) thread );
	while (writeback)
		crNetRecv();
}

void REPLICATESPU_APIENTRY replicatespu_GetVertexAttribfvNV( GLuint index, GLenum pname, GLfloat *params )
{
	GET_THREAD(thread);
	int writeback = 1;
	if (replicate_spu.swap)
	{
		crPackGetVertexAttribfvNVSWAP( index, pname, params, &writeback );
	}
	else
	{
		crPackGetVertexAttribfvNV( index, pname, params, &writeback );
	}
	replicatespuFlush( (void *) thread );
	while (writeback)
		crNetRecv();
}

void REPLICATESPU_APIENTRY replicatespu_GetVertexAttribivNV( GLuint index, GLenum pname, GLint *params )
{
	GET_THREAD(thread);
	int writeback = 1;
	if (replicate_spu.swap)
	{
		crPackGetVertexAttribivNVSWAP( index, pname, params, &writeback );
	}
	else
	{
		crPackGetVertexAttribivNV( index, pname, params, &writeback );
	}
	replicatespuFlush( (void *) thread );
	while (writeback)
		crNetRecv();
}

GLboolean REPLICATESPU_APIENTRY replicatespu_IsFenceNV( GLuint fence )
{
	GET_THREAD(thread);
	int writeback = 1;
	GLboolean return_val = (GLboolean) 0;
	if (replicate_spu.swap)
	{
		crPackIsFenceNVSWAP( fence, &return_val, &writeback );
	}
	else
	{
		crPackIsFenceNV( fence, &return_val, &writeback );
	}
	replicatespuFlush( (void *) thread );
	while (writeback)
		crNetRecv();
	if (replicate_spu.swap)
	{
		return_val = (GLboolean) SWAP32(return_val);
	}
	return return_val;
}

GLboolean REPLICATESPU_APIENTRY replicatespu_IsProgramARB( GLuint id )
{
	GET_THREAD(thread);
	int writeback = 1;
	GLboolean return_val = (GLboolean) 0;
	if (replicate_spu.swap)
	{
		crPackIsProgramARBSWAP( id, &return_val, &writeback );
	}
	else
	{
		crPackIsProgramARB( id, &return_val, &writeback );
	}
	replicatespuFlush( (void *) thread );
	while (writeback)
		crNetRecv();
	if (replicate_spu.swap)
	{
		return_val = (GLboolean) SWAP32(return_val);
	}
	return return_val;
}

GLboolean REPLICATESPU_APIENTRY replicatespu_TestFenceNV( GLuint fence )
{
	GET_THREAD(thread);
	int writeback = 1;
	GLboolean return_val = (GLboolean) 0;
	if (replicate_spu.swap)
	{
		crPackTestFenceNVSWAP( fence, &return_val, &writeback );
	}
	else
	{
		crPackTestFenceNV( fence, &return_val, &writeback );
	}
	replicatespuFlush( (void *) thread );
	while (writeback)
		crNetRecv();
	if (replicate_spu.swap)
	{
		return_val = (GLboolean) SWAP32(return_val);
	}
	return return_val;
}
