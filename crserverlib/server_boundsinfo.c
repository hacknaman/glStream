/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "server_dispatch.h"
#include "server.h"
#include "cr_error.h"
#include "cr_unpack.h"
#include "cr_mem.h"
#include "state/cr_statetypes.h"

/* This code copied from the tilesorter (fooey) */

typedef struct BucketRegion *BucketRegion_ptr;
typedef struct BucketRegion {
	CRbitvalue       id;
	CRrecti          extents;
	BucketRegion_ptr right;
	BucketRegion_ptr up;
} BucketRegion;

#define HASHRANGE 256
static BucketRegion *rhash[HASHRANGE][HASHRANGE];

#define BKT_DOWNHASH(a, range) ((a)*HASHRANGE/(range))
#define BKT_UPHASH(a, range) ((a)*HASHRANGE/(range) + ((a)*HASHRANGE%(range)?1:0))


/*
 * Test if we can really use optimizeBucket with the current set of
 * tiles.
 * Return GL_TRUE if so, GL_FALSE if not.
 */
GLboolean crServerCheckTileLayout(void)
{
	int optTileWidth = 0, optTileHeight = 0;
	int i;

	for (i = 0; i < cr_server.numExtents; i++)
	{
		const int w = cr_server.extents[i].x2 - cr_server.extents[i].x1;
		const int h = cr_server.extents[i].y2 - cr_server.extents[i].y1;

		if (optTileWidth == 0 && optTileHeight == 0) {
			/* first tile */
			optTileWidth = w;
			optTileHeight = h;
		}
		else
		{
			/* subsequent tile - make sure it's the same size as first */
			if (w != optTileWidth || h != optTileHeight)
			{
				return GL_FALSE;
			}
			else if (cr_server.extents[i].x1 % optTileWidth != 0 ||
							 cr_server.extents[i].x2 % optTileWidth != 0 ||
							 cr_server.extents[i].y1 % optTileHeight != 0 ||
							 cr_server.extents[i].y2 % optTileHeight != 0)
			{
				return GL_FALSE;
			}
		}
	}

	return GL_TRUE;
}


void crServerFillBucketingHash(void) 
{
	int i, j, k, m;
	int r_len = 0;
	int xinc, yinc;
	int rlist_alloc = 64 * 128;

	BucketRegion *rlist;
	BucketRegion *rptr;

	/* Allocate rlist (don't free it!!!) */
	rlist = (BucketRegion *) crAlloc( rlist_alloc * sizeof(*rlist) );

	for ( i = 0; i < HASHRANGE; i++ )
	{
		for ( j = 0; j < HASHRANGE; j++ )
		{
			rhash[i][j] = NULL;
		}
	}

	/* Fill the rlist */
	xinc = cr_server.extents[0].x2 - cr_server.extents[0].x1;
	yinc = cr_server.extents[0].y2 - cr_server.extents[0].y1;

	rptr = rlist;
	for (i=0; i < (int) cr_server.muralWidth; i+=xinc) 
	{
		for (j=0; j < (int) cr_server.muralHeight; j+=yinc) 
		{
			for (k=0; k < cr_server.numExtents; k++) 
			{
				if (cr_server.extents[k].x1 == i && cr_server.extents[k].y1 == j) 
				{
					rptr->extents = cr_server.extents[k]; /* x1,y1,x2,y2 */
					rptr->id = k;
					break;
				}
			}
			if (k == cr_server.numExtents) 
			{
				rptr->extents.x1 = i;
				rptr->extents.y1 = j;
				rptr->extents.x2 = i + xinc;
				rptr->extents.y2 = j + yinc;
				rptr->id = -1;
			}
			rptr++;
		}
	}
	r_len = rptr - rlist;

	/* Fill hash table */
	for (i = 0; i < r_len; i++)
	{
		BucketRegion *r = &rlist[i];

		for (k=BKT_DOWNHASH(r->extents.x1, (int)cr_server.muralWidth);
		     k<=BKT_UPHASH(r->extents.x2, (int)cr_server.muralWidth) &&
			     k < HASHRANGE;
		     k++) 
		{
			for (m=BKT_DOWNHASH(r->extents.y1, (int)cr_server.muralHeight);
			     m<=BKT_UPHASH(r->extents.y2, (int)cr_server.muralHeight) &&
				     m < HASHRANGE;
			     m++) 
			{
				if ( rhash[m][k] == NULL ||
				     (rhash[m][k]->extents.x1 > r->extents.x1 &&
				      rhash[m][k]->extents.y1 > r->extents.y1))
				{
					rhash[m][k] = r;
				}
			}
		}
	}

	/* Initialize links */
	for (i=0; i<r_len; i++) 
	{
		BucketRegion *r = &rlist[i];
		r->right = NULL;
		r->up    = NULL;
	}

	/* Build links */
	for (i=0; i<r_len; i++) 
	{
		BucketRegion *r = &rlist[i];
		for (j=0; j<r_len; j++) 
		{
			BucketRegion *q = &rlist[j];
			if (r==q)
				continue;

			/* Right Edge */
			if (r->extents.x2 == q->extents.x1 &&
			    r->extents.y1 == q->extents.y1 &&
			    r->extents.y2 == q->extents.y2) 
			{
				r->right = q;
			}

			/* Upper Edge */
			if (r->extents.y2 == q->extents.y1 &&
			    r->extents.x1 == q->extents.x1 &&
			    r->extents.x2 == q->extents.x2) 
			{
				r->up = q;
			}
		}
	}

}

/*
 * Prepare for rendering a tile.  Setup viewport and base projection.
 * Inputs: ctx - the rendering context (to access the viewport params)
 *         outputWindow - tile bounds in server's rendering window
 *         imagespace - whole mural rectangle
 *         imagewindow - tile bounds within the mural
 */
void crServerSetOutputBounds( CRContext *ctx, 
												const CRrecti *outputwindow, 
												const CRrecti *imagespace, 
												const CRrecti *imagewindow )
{
	CRrecti p, q;

	crServerSetViewportBounds( &(ctx->viewport), outputwindow,
														 imagespace, imagewindow, &p, &q );
	crServerRecomputeBaseProjection( &(cr_server.curClient->baseProjection),
																	 ctx->viewport.viewportX,
																	 ctx->viewport.viewportY,
																	 ctx->viewport.viewportW,
																	 ctx->viewport.viewportH );
	crServerApplyBaseProjection();
}

void SERVER_DISPATCH_APIENTRY
crServerDispatchBoundsInfoCR( const CRrecti *bounds, const GLbyte *payload,
															GLint len, GLint num_opcodes )
{
	char *data_ptr = (char*)(payload + ((num_opcodes + 3 ) & ~0x03));
	int i;
	unsigned int bx, by;

	crUnpackPush();

	bx = BKT_DOWNHASH(bounds->x1, cr_server.muralWidth);
	by = BKT_DOWNHASH(bounds->y1, cr_server.muralHeight);

	/* Check for out of bounds, and optimizebucket to enable */
	if ((bx <= HASHRANGE) &&
	    (by <= HASHRANGE) &&
	    cr_server.optimizeBucket)
	{
		BucketRegion *r;
		BucketRegion *p;

		for (r = rhash[by][bx]; r && bounds->y2 >= r->extents.y1;
		     r = r->up)
		{
			for (p=r; p && bounds->x2 >= p->extents.x1; p = p->right)
			{
				if ( p->id != -1 &&
					bounds->x1 < p->extents.x2  &&
					bounds->y1 < p->extents.y2 &&
					bounds->y2 >= p->extents.y1 )
				{
					cr_server.curExtent = p->id;
					if (cr_server.run_queue->client->currentCtx) {
						crServerSetOutputBounds( cr_server.run_queue->client->currentCtx,
																		 &cr_server.run_queue->extent[p->id].outputwindow,
																		 &cr_server.run_queue->imagespace,
																		 &cr_server.run_queue->extent[p->id].imagewindow );
					}
					crUnpack( data_ptr, data_ptr-1, num_opcodes, &(cr_server.dispatch) );
				}
			}
		}
	} 
	else 
	{
		/* non-optimized bucketing */
		for ( i = 0; i < cr_server.run_queue->numExtents; i++ )
		{
			const CRRunQueueExtent *extent = &cr_server.run_queue->extent[i];

			if ((!cr_server.localTileSpec) &&
			    ( !( extent->imagewindow.x2 > bounds->x1 &&
							extent->imagewindow.x1 < bounds->x2 &&
							extent->imagewindow.y2 > bounds->y1 &&
							extent->imagewindow.y1 < bounds->y2 ) ))
			{
				continue;
			}

			cr_server.curExtent = i;

			if (cr_server.run_queue->client->currentCtx) {

				crServerSetOutputBounds( cr_server.run_queue->client->currentCtx,
																 &extent->outputwindow,
																 &cr_server.run_queue->imagespace,
																 &extent->imagewindow );
			}

			crUnpack( data_ptr, data_ptr-1, num_opcodes, &(cr_server.dispatch) );
		}
	}

	crUnpackPop();
}
