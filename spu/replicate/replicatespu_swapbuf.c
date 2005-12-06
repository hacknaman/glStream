/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "cr_packfunctions.h"
#include "cr_error.h"
#include "cr_net.h"
#include "replicatespu.h"
#include "replicatespu_proto.h"


void REPLICATESPU_APIENTRY
replicatespu_SwapBuffers( GLint window, GLint flags )
{
	int i;
	GET_THREAD(thread);
	WindowInfo *winInfo
		= (WindowInfo *) crHashtableSearch( replicate_spu.windowTable, window );

	replicatespuFlush( (void *) thread );

	for (i = 0; i < CR_MAX_REPLICANTS; i++) {
		if (replicate_spu.rserver[i].conn == NULL ||
				replicate_spu.rserver[i].conn->type == CR_NO_CONNECTION)
			continue;

		if (replicate_spu.swap)
			crPackSwapBuffersSWAP( winInfo->id[i], flags );
		else
			crPackSwapBuffers( winInfo->id[i], flags );

		replicatespuFlushOne(thread, i);

		if (replicate_spu.sync_on_swap) {
			/* This won't block unless there has been more than 1 frame
			 * since we received a writeback acknowledgement.  In the
			 * normal case there's no performance penalty for doing this
			 * (beyond the cost of packing the writeback request into the
			 * stream and receiving the reply), but it eliminates the
			 * problem of runaway rendering that can occur, eg when
			 * rendering frames consisting of a single large display list
			 * in a tight loop.
			 *
			 * Note that this is *not* the same as doing a sync after each
			 * swapbuffers, which would force a round-trip 'bubble' into
			 * the network stream under normal conditions.
			 *
			 * This is complicated because writeback in the pack spu is
			 * overriden to always set the value to zero when the
			 * reply is received, rather than decrementing it: 
			 */
			if (winInfo->writeback == 0) {
				/* Request writeback */
				winInfo->writeback = 1;
				if (replicate_spu.swap)
					crPackWritebackSWAP( &winInfo->writeback );
				else
					crPackWriteback( &winInfo->writeback );
			}
			else {
				/* Make sure writeback from previous frame has been received. */
				while (winInfo->writeback) {
					/* detected disconnect during swapbuffers, drop the writeback wait */
					if (replicate_spu.rserver[i].conn->type == CR_NO_CONNECTION)
						winInfo->writeback = 0;
					else
						crNetRecv();
				}
			}

		} /* if sync */
	} /* loop */
}
