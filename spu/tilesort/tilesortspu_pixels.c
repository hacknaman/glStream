#include "cr_packfunctions.h"
#include "cr_glstate.h"
#include "cr_error.h"
#include "cr_net.h"
#include "tilesortspu.h"
#include "cr_applications.h"

void TILESORTSPU_APIENTRY tilesortspu_Bitmap( 
					GLsizei width, GLsizei height,
					GLfloat xorig, GLfloat yorig,
					GLfloat xmove, GLfloat ymove,
					const GLubyte * bitmap) 
{
	CRCurrentState *c = &(tilesort_spu.ctx->current);
	CRViewportState *v = &(tilesort_spu.ctx->viewport);
	GLfloat screen_bbox[8];

	if (!c->rasterValid)
	{
		return;
	}

	tilesortspuFlush( tilesort_spu.ctx );

	crStateBitmap( width, height, xorig, yorig, xmove, ymove, bitmap );
	crPackBitmap ( width, height, xorig, yorig, xmove, ymove, bitmap, &(tilesort_spu.ctx->pixel.unpack) );

	screen_bbox[0] = (c->rasterPos.x - xorig)/v->viewportW;
	screen_bbox[1] = (c->rasterPos.y - yorig)/v->viewportH;
	screen_bbox[4] = (c->rasterPos.x - xorig + width)/v->viewportW;
	screen_bbox[5] = (c->rasterPos.y - yorig + height)/v->viewportH;

	screen_bbox[0] *= 2.0f;
	screen_bbox[1] *= 2.0f;
	screen_bbox[4] *= 2.0f;
	screen_bbox[5] *= 2.0f;

	screen_bbox[0] -= 1.0f;
	screen_bbox[1] -= 1.0f;
	screen_bbox[4] -= 1.0f;
	screen_bbox[5] -= 1.0f;

	tilesortspu_Hint( CR_SCREEN_BBOX_HINT, (GLenum) screen_bbox );
	c->rasterPosPre.x -= xmove;
	c->rasterPosPre.y -= ymove;
	tilesortspuFlush( tilesort_spu.ctx );
	c->rasterPosPre.x += xmove;
	c->rasterPosPre.y += ymove;
	tilesortspu_Hint( CR_SCREEN_BBOX_HINT, (GLenum) NULL );
}
