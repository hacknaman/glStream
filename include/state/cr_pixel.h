#ifndef CR_STATE_PIXEL_H
#define CR_STATE_PIXEL_H

#include "cr_glwrapper.h"
#include "state/cr_statetypes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	GLbitvalue dirty;
	GLbitvalue pack;
	GLbitvalue unpack;
	GLbitvalue transfer;
	GLbitvalue zoom;
} CRPixelBits;

typedef struct {
	GLint		   rowLength;
	GLint		   skipRows;
	GLint		   skipPixels;
	GLint		   alignment;
	GLint		   imageHeight;
	GLint		   skipImages;
	GLboolean	 swapBytes;
	GLboolean	 LSBFirst;
} CRPackState;

typedef struct {
	CRPackState pack;
	CRPackState unpack;
	GLboolean   mapColor;
	GLboolean   mapStencil;
	GLint		    indexShift;
	GLint		    indexOffset;
	GLcolorf	  scale;
	GLfloat		  depthScale;
	GLcolorf	  bias;
	GLfloat		  depthBias;
	GLfloat		  xZoom;
	GLfloat		  yZoom;

	GLint		    pixelmapmaxsize;

	GLfloat		 *pixelmapitoi;
	GLfloat		 *pixelmapstos;
	GLfloat		 *pixelmapitor;
	GLfloat		 *pixelmapitog;
	GLfloat		 *pixelmapitob;
	GLfloat		 *pixelmapitoa;
	GLfloat		 *pixelmaprtor;
	GLfloat		 *pixelmapgtog;
	GLfloat		 *pixelmapbtob;
	GLfloat		 *pixelmapatoa;
 
	GLint		    pixelmapitoisize;
	GLint		    pixelmapstossize;
	GLint		    pixelmapitorsize;
	GLint		    pixelmapitogsize;
	GLint		    pixelmapitobsize;
	GLint		    pixelmapitoasize;
	GLint		    pixelmaprtorsize;
	GLint		    pixelmapgtogsize;
	GLint		    pixelmapbtobsize;
	GLint		    pixelmapatoasize;
} CRPixelState;

void crStatePixelInit( CRPixelState *pixel );
void crStatePixelInitBits( CRPixelBits *pixelbits );

#ifdef __cplusplus
}
#endif


#endif /* CR_STATE_PIXEL_H */
