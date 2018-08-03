/* Copyright (c) 2001, Stanford University
 * All rights reserved.
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#ifndef CAMERA_SPU_H
#define CAMERA_SPU_H

#ifdef WINDOWS
#define CAMERASPU_APIENTRY __stdcall
#else
#define CAMERASPU_APIENTRY
#endif

#include "cr_spu.h"
#include "cr_server.h"

/**
 * Camera SPU descriptor
 */
typedef struct {
	int id; /**< Spu id */
	int has_child; 	/**< Spu has a child  Not used */
	SPUDispatchTable self, child, super;	/**< SPUDispatchTable self for this SPU, child spu and super spu */
	CRServer *server;	/**< crserver descriptor */
} CameraSPU;

/** Camera state descriptor */
extern CameraSPU camera_spu;

/** Named SPU functions */
extern SPUNamedFunctionTable _cr_camera_table[];

/** Option table for SPU */
extern SPUOptions cameraSPUOptions[];

extern void cameraspuGatherConfiguration( void );


#endif /* CAMERA_SPU_H */
