#ifndef CR_RENDERSPU_H
#define CR_RENDERSPU_H

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define RENDER_APIENTRY __stdcall
#else
#include <GL/glx.h>
#define RENDER_APIENTRY
#endif

#include "cr_spu.h"

void renderspuLoadSystemGL( void );
void renderspuGatherConfiguration( void );
void renderspuCreateWindow( void );

void RENDER_APIENTRY renderspuSwapBuffers( void );

#ifdef WINDOWS
typedef HGLRC (RENDER_APIENTRY *wglCreateContextFunc_t)(HDC);
typedef BOOL (RENDER_APIENTRY *wglMakeCurrentFunc_t)(HDC,HGLRC);
typedef BOOL (RENDER_APIENTRY *wglSwapBuffersFunc_t)(HDC);
typedef int (RENDER_APIENTRY *wglChoosePixelFormatFunc_t)(HDC, CONST PIXELFORMATDESCRIPTOR *);
typedef int (RENDER_APIENTRY *wglSetPixelFormatFunc_t)(HDC, int, CONST PIXELFORMATDESCRIPTOR *);
typedef HGLRC (RENDER_APIENTRY *wglGetCurrentContextFunc_t)();
#else
typedef int (*glXGetConfigFunc_t)( Display *, XVisualInfo *, int, int * );
typedef Bool (*glXQueryExtensionFunc_t) (Display *, int *, int * );
typedef XVisualInfo *(*glXChooseVisualFunc_t)( Display *, int, int * );
typedef GLXContext (*glXCreateContextFunc_t)( Display *, XVisualInfo *, GLXContext, Bool );
typedef Bool (*glXIsDirectFunc_t)( Display *, GLXContext );
typedef Bool (*glXMakeCurrentFunc_t)( Display *, GLXDrawable, GLXContext );
typedef const GLubyte *(*glGetStringFunc_t)( GLenum );
typedef void (*glXSwapBuffersFunc_t)( Display *, GLXDrawable );
#endif

typedef struct {
	SPUDispatchTable *dispatch;
	int id;
	int window_x, window_y;
	unsigned int window_width, window_height;
	unsigned int actual_window_width, actual_window_height;
	int use_L2;
	int fullscreen;
	int depth_bits, stencil_bits;
#ifdef WINDOWS
	HWND         hWnd;
	HGLRC        hRC;
	HDC          device_context;
	wglCreateContextFunc_t wglCreateContext;
	wglMakeCurrentFunc_t wglMakeCurrent;
	wglSwapBuffersFunc_t wglSwapBuffers;
	wglGetCurrentContextFunc_t wglGetCurrentContext;
	wglChoosePixelFormatFunc_t wglChoosePixelFormat;
	wglSetPixelFormatFunc_t wglSetPixelFormat;
#else
	glXGetConfigFunc_t  glXGetConfig;
	glXQueryExtensionFunc_t glXQueryExtension;
	glXChooseVisualFunc_t glXChooseVisual;
	glXCreateContextFunc_t glXCreateContext;
	glXIsDirectFunc_t glXIsDirect;
	glXMakeCurrentFunc_t glXMakeCurrent;
	glGetStringFunc_t glGetString;
	glXSwapBuffersFunc_t glXSwapBuffers;
	Display     *dpy;
	XVisualInfo *visual;
	GLXContext   context;
	Window       window;
	char        *display_string;
	int          try_direct;
	int          force_direct;
	int          sync;
#endif
} RenderSPU;

extern RenderSPU render_spu;

#endif /* CR_RENDERSPU_H */
