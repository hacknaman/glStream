/* Copyright (c) 2004, Tungsten Graphics, Inc.
 * All rights reserved.
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#ifndef VNC_SPU_H
#define VNC_SPU_H

#ifdef WINDOWS
#define VNCSPU_APIENTRY __stdcall
#else
#define VNCSPU_APIENTRY
#endif

#include "cr_hash.h"
#include "cr_spu.h"
#include "cr_server.h"
#include "rfblib.h"
#include "region.h"

typedef struct {
	GLint dirtyX, dirtyY, dirtyW, dirtyH;
} WindowInfo;


/**
 * Vnc SPU descriptor
 */
typedef struct {
	int id; /**< Spu id */
	int has_child; 	/**< Spu has a child  Not used */
	SPUDispatchTable self, child, super;	/**< SPUDispatchTable self for this SPU, child spu and super spu */
	CRServer *server;	/**< crserver descriptor */

	/* config options */
	int server_port;
	int screen_width, screen_height;
	int max_update_rate;

	GLubyte *screen_buffer;
	CRHashTable *windowTable;
	WindowInfo *currentWindow;
} VncSPU;

/** Vnc state descriptor */
extern VncSPU vnc_spu;

/** Named SPU functions */
extern SPUNamedFunctionTable _cr_vnc_table[];

/** Option table for SPU */
extern SPUOptions vncSPUOptions[];

extern void vncspuGatherConfiguration( void );

extern void vncspuStartServerThread(void);

extern void vncspuGetDirtyRegions(RegionPtr dirtyRegion);

#endif /* VNC_SPU_H */
