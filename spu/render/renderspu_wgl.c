/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */


#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#include "cr_environment.h"
#include "cr_error.h"
#include "cr_string.h"
#include "renderspu.h"
#include "cr_mem.h"

#define WINDOW_NAME render_spu.window_title

GLboolean renderspu_SystemInitVisual( VisualInfo *visual )
{
	return TRUE;
}

void renderspu_SystemDestroyWindow( WindowInfo *window )
{
	CRASSERT(window);
	DestroyWindow( window->visual->hWnd );
	window->visual->hWnd = NULL;
	window->visual = NULL;
	window->width = window->height = 0;
}

static LONG WINAPI
MainWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	/* int w,h; */

	switch ( uMsg ) {

		case WM_SIZE:
			/* w = LOWORD( lParam ); 
			 * h = HIWORD( lParam ); */

			/* glViewport( 0, 0, w, h ); */
#if 0
			glViewport( -render_spu.mural_x, -render_spu.mural_y, 
					render_spu.mural_width, render_spu.mural_height );
			glScissor( -render_spu.mural_x, -render_spu.mural_y, 
					render_spu.mural_width, render_spu.mural_height );
#endif
			break;

		case WM_CLOSE:
			crWarning( "caught WM_CLOSE -- quitting." );
			exit( 0 );
			break;
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

static BOOL
bSetupPixelFormat( HDC hdc, GLbitfield visAttribs )
{
	PIXELFORMATDESCRIPTOR pfd = { 
		sizeof(PIXELFORMATDESCRIPTOR),  /*  size of this pfd */
		1,                              /* version number */
		PFD_DRAW_TO_WINDOW |            /* support window */
		PFD_SUPPORT_OPENGL,             /* support OpenGL */
		PFD_TYPE_RGBA,                  /* RGBA type */
		24,                             /* 24-bit color depth */
		0, 0, 0, 0, 0, 0,               /* color bits ignored */
		0,                              /* no alpha buffer */
		0,                              /* shift bit ignored */
		0,                              /* no accumulation buffer */
		0, 0, 0, 0,                     /* accum bits ignored */
		0,                              /* set depth buffer	 */
		0,                              /* set stencil buffer */
		0,                              /* no auxiliary buffer */
		PFD_MAIN_PLANE,                 /* main layer */
		0,                              /* reserved */
		0, 0, 0                         /* layer masks ignored */
	}; 
	PIXELFORMATDESCRIPTOR *ppfd = &pfd; 
	char s[1000];
	GLbitfield b = 0;
	int pixelformat;

	renderspuMakeVisString( visAttribs, s );

	crWarning( "Render SPU: WGL wants these visual capabilities: %s", s);

	/* These really come into play with sort-last configs */
	if (visAttribs & CR_DEPTH_BIT)
		ppfd->cDepthBits = 24;
	if (visAttribs & CR_ACCUM_BIT)
		ppfd->cAccumBits = 16;
	if (visAttribs & CR_RGB_BIT)
		ppfd->cColorBits = 24;
	if (visAttribs & CR_STENCIL_BIT)
		ppfd->cStencilBits = 8;
	if (visAttribs & CR_ALPHA_BIT)
		ppfd->cAlphaBits = 8;
	if (visAttribs & CR_DOUBLE_BIT)
		ppfd->dwFlags |= PFD_DOUBLEBUFFER;
	if (visAttribs & CR_STEREO_BIT)
		ppfd->dwFlags |= PFD_STEREO;

	/* 
	 * We call the wgl functions directly if the SPU was loaded
	 * by our faker library, otherwise we have to call the GDI
	 * versions.
	 */
	if (crGetenv( "CR_WGL_DO_NOT_USE_GDI" ) != NULL)
	{
		pixelformat = render_spu.ws.wglChoosePixelFormat( hdc, ppfd );
		/* doing this twice is normal Win32 magic */
		pixelformat = render_spu.ws.wglChoosePixelFormat( hdc, ppfd );
		if ( pixelformat == 0 ) 
		{
			crError( "render_spu.ws.wglChoosePixelFormat failed" );
		}
		if ( !render_spu.ws.wglSetPixelFormat( hdc, pixelformat, ppfd ) ) 
		{
			crError( "render_spu.ws.wglSetPixelFormat failed" );
		}
	
		render_spu.ws.wglDescribePixelFormat( hdc, pixelformat, sizeof(ppfd), ppfd );
	}
	else
	{
		/* Okay, we were loaded manually.  Call the GDI functions. */
		pixelformat = ChoosePixelFormat( hdc, ppfd );
		/* doing this twice is normal Win32 magic */
		pixelformat = ChoosePixelFormat( hdc, ppfd );
		if ( pixelformat == 0 ) 
		{
			crError( "ChoosePixelFormat failed" );
		}
		if ( !SetPixelFormat( hdc, pixelformat, ppfd ) ) 
		{
			crError( "SetPixelFormat failed" );
		}
		
		DescribePixelFormat( hdc, pixelformat, sizeof(ppfd), ppfd );
	}


	if (ppfd->cDepthBits > 0)
		b |= CR_DEPTH_BIT;
	if (ppfd->cAccumBits > 0)
		b |= CR_ACCUM_BIT;
	if (ppfd->cColorBits > 8)
		b |= CR_RGB_BIT;
	if (ppfd->cStencilBits > 0)
		b |= CR_STENCIL_BIT;
	if (ppfd->cAlphaBits > 0)
		b |= CR_ALPHA_BIT;
	if (ppfd->dwFlags & PFD_DOUBLEBUFFER)
		b |= CR_DOUBLE_BIT;
	if (ppfd->dwFlags & PFD_STEREO)
		b |= CR_STEREO_BIT;

	renderspuMakeVisString( b, s );

	crWarning( "Render SPU: WGL chose these visual capabilities: %s", s);
	return TRUE;
}

GLboolean renderspu_SystemCreateWindow( VisualInfo *visual, GLboolean showIt, WindowInfo *window )
{
	HDESK     desktop;
	HINSTANCE hinstance;
	WNDCLASS  wc;
	DWORD     window_style;
	int       window_plus_caption_width;
	int       window_plus_caption_height;

	window->visual = visual;
	window->x = render_spu.defaultX;
	window->y = render_spu.defaultY;
	window->width  = render_spu.defaultWidth;
	window->height = render_spu.defaultHeight;
	window->nativeWindow = 0;

	if ( render_spu.use_L2 )
	{
		crWarning( "Going fullscreen because we think we're using Lightning-2." );
		render_spu.fullscreen = 1;
	}

	/*
	 * Begin Windows / WGL code
	 */

	hinstance = GetModuleHandle( NULL );
	if (!hinstance)
	{
		crError( "Couldn't get a handle to my module." );
		return GL_FALSE;
	}
	crDebug( "Got the module handle: 0x%x", hinstance );

	/* If we were launched from a service, telnet, or rsh, we need to
	 * get the input desktop.  */

	desktop = OpenInputDesktop( 0, FALSE,
			DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
			DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
			DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
			DESKTOP_SWITCHDESKTOP | GENERIC_WRITE );

	if ( !desktop )
	{
		crError( "Couldn't aquire input desktop" );
		return GL_FALSE;
	}
	crDebug( "Got the desktop: 0x%x", desktop );

	if ( !SetThreadDesktop( desktop ) )
	{
		/* If this function fails, it's probably because 
		 * it's already been called (i.e., the render SPU 
		 * is bolted to an application?) */

		/*crError( "Couldn't set thread to input desktop" ); */
	}
	crDebug( "Set the thread desktop -- this might have failed." );

	if ( !GetClassInfo(hinstance, WINDOW_NAME, &wc) ) 
	{
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = (WNDPROC) MainWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hinstance;
		wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
		wc.hCursor = LoadCursor( NULL, IDC_ARROW );
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = WINDOW_NAME;

		if ( !RegisterClass( &wc ) )
		{
			crError( "Couldn't register window class -- you're not trying "
					"to do multi-pipe stuff on windows, are you?\n\nNote --"
					"This error message is from 1997 and probably doesn't make"
					"any sense any more, but it's nostalgic for Humper." );
			return GL_FALSE;
		}
		crDebug( "Registered the class" );
	}
	crDebug( "Got the class information" );

	/* Full screen window should be a popup (undecorated) window */
#if 1
	window_style = ( render_spu.fullscreen ? WS_POPUP : WS_CAPTION );
#else
	window_style = ( WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN );
	window_style |= WS_SYSMENU;
#endif

	crDebug( "Fullscreen: %s\n", render_spu.fullscreen ? "yes" : "no");

	if ( render_spu.fullscreen )
	{
#if 0

		int smCxFixedFrame = GetSystemMetrics( SM_CXFIXEDFRAME );
		int smCyFixedFrame = GetSystemMetrics( SM_CXFIXEDFRAME ) + 1;
		int smCyCaption = GetSystemMetrics( SM_CYCAPTION );

		window->width = GetSystemMetrics( SM_CXSCREEN ) ;
		window->height = GetSystemMetrics( SM_CYSCREEN ) ;

		crDebug( "Window Dims: %d, %d\n", window->width, window->height );

		window->x = render_spu->defaultX - smCxFixedFrame - 1;
		window->y = render_spu->defaultY - smCyFixedFrame - smCyCaption;

		window_plus_caption_width = window->width + 2 * smCxFixedFrame;
		window_plus_caption_height = window->height + 2 * smCyFixedFrame + smCyCaption;

#else
		/* Since it's undecorated, we don't have to do anything fancy
		 * with these parameters. */

		window->width = GetSystemMetrics( SM_CXSCREEN ) ;
		window->height = GetSystemMetrics( SM_CYSCREEN ) ;
		window->x = 0;
		window->y = 0;
		window_plus_caption_width = window->width;
		window_plus_caption_height = window->height;

#endif
	}
	else
	{
		/* CreateWindow takes the size of the entire window, so we add
		 * in the size necessary for the frame and the caption. */

		int smCxFixedFrame, smCyFixedFrame, smCyCaption;
		smCxFixedFrame = GetSystemMetrics( SM_CXFIXEDFRAME );
		crDebug( "Got the X fixed frame" );
		smCyFixedFrame = GetSystemMetrics( SM_CYFIXEDFRAME );
		crDebug( "Got the Y fixed frame" );
		smCyCaption = GetSystemMetrics( SM_CYCAPTION );
		crDebug( "Got the Caption " );

		window_plus_caption_width = window->width +	2 * smCxFixedFrame;
		window_plus_caption_height = window->height + 2 * smCyFixedFrame + smCyCaption;

		window->x = render_spu.defaultX - smCxFixedFrame;
		window->y = render_spu.defaultY - smCyFixedFrame - smCyCaption;
	}

	crDebug( "Creating the window: (%d,%d), (%d,%d)", render_spu.defaultX, render_spu.defaultY, window_plus_caption_width, window_plus_caption_height );
	visual->hWnd = CreateWindow( WINDOW_NAME, WINDOW_NAME,
			window_style,
			window->x, window->y,
			window_plus_caption_width,
			window_plus_caption_height,
			NULL, NULL, hinstance, &render_spu );

	if ( !visual->hWnd )
	{
		crError( "Create Window failed!  That's almost certainly terrible." );
		return GL_FALSE;
	}

	if (showIt) {
		/* NO ERROR CODE FOR SHOWWINDOW */

		crDebug( "Showing the window" );

		ShowWindow( visual->hWnd, SW_SHOWNORMAL );
	}

	SetForegroundWindow( visual->hWnd );

	SetWindowPos( visual->hWnd, HWND_TOP, window->x, window->y,
		      window_plus_caption_width, window_plus_caption_height,
		      ( render_spu.fullscreen ? (SWP_SHOWWINDOW |
			  			 SWP_NOSENDCHANGING | 
			   			 SWP_NOREDRAW | 
						 SWP_NOACTIVATE ) :
  			  			 0 ) );

	if ( render_spu.fullscreen )
		ShowCursor( FALSE );

	visual->device_context = GetDC( visual->hWnd );

	crDebug( " Got the DC: 0x%x", visual->device_context );

	if ( !bSetupPixelFormat( visual->device_context, visual->visAttribs ) )
	{
		crError( "Couldn't set up the device context!  Yikes!" );
		return GL_FALSE;
	}

	return GL_TRUE;
}


/* Either show or hide the render SPU's window. */
void renderspu_SystemShowWindow( WindowInfo *window, GLboolean showIt )
{
	if (showIt)
		ShowWindow( window->visual->hWnd, SW_SHOWNORMAL );
	else
		ShowWindow( window->visual->hWnd, SW_HIDE );
}

GLboolean renderspu_SystemCreateContext( VisualInfo *visual, ContextInfo *context )
{
	crDebug( " Using the DC: 0x%x", visual->device_context );
	context->hRC = render_spu.ws.wglCreateContext( visual->device_context );
	if (!context->hRC)
	{
		crError( "Couldn't create the context for the window 0x%x !", GetLastError() );
		return GL_FALSE;
	}

	return GL_TRUE;
}

void renderspu_SystemDestroyContext( ContextInfo *context )
{
	render_spu.ws.wglDeleteContext( context->hRC );
	context->hRC = NULL;
}

void renderspu_SystemMakeCurrent( WindowInfo *window, GLint nativeWindow, ContextInfo *context )
{
	CRASSERT(render_spu.ws.wglMakeCurrent);

	if (context && window) {
		CRASSERT(context->hRC);
		if (render_spu.render_to_app_window && nativeWindow)
		{
			/* The render_to_app_window option is set and we've got a nativeWindow
			 * handle, save the handle for later calls to swapbuffers().
			 */
			window->nativeWindow = (HDC) nativeWindow;
			render_spu.ws.wglMakeCurrent( window->nativeWindow, context->hRC );
		}
		else
		{
			render_spu.ws.wglMakeCurrent( window->visual->device_context, context->hRC );
		}

	}
	else {
		render_spu.ws.wglMakeCurrent( 0, 0 );
	}
}

void renderspu_SystemWindowSize( WindowInfo *window, int w, int h )
{
	int winprop;
	CRASSERT(window);
	CRASSERT(window->visual);
	if ( render_spu.fullscreen )
		winprop = SWP_SHOWWINDOW | SWP_NOSENDCHANGING |
			  SWP_NOREDRAW | SWP_NOACTIVATE;
	else 
		winprop = SWP_SHOWWINDOW;
	SetWindowPos( window->visual->hWnd, HWND_TOPMOST, 
			window->x, window->y,
			w, h, winprop );
}


void renderspu_SystemGetWindowSize( WindowInfo *window, int *w, int *h )
{
	RECT rect;

	CRASSERT(window);
	CRASSERT(window->visual);

	GetClientRect( window->visual->hWnd, &rect );
	*w = rect.right - rect.left;
	*h = rect.bottom - rect.top;
}


void renderspu_SystemWindowPosition( WindowInfo *window, int x, int y )
{
	int winprop;
	CRASSERT(window);
	CRASSERT(window->visual);
	if ( render_spu.fullscreen )
		winprop = SWP_SHOWWINDOW | SWP_NOSENDCHANGING |
			  SWP_NOREDRAW | SWP_NOACTIVATE;
	else 
		winprop = SWP_SHOWWINDOW;
	SetWindowPos( window->visual->hWnd, HWND_TOPMOST, 
			x, y,
			window->width,
			window->height,
			winprop );
}

static void renderspuHandleWindowMessages( HWND hWnd )
{
	MSG msg;
	while ( PeekMessage( &msg, hWnd, 0, 0xffffffff, PM_REMOVE ) )
	{
		TranslateMessage( &msg );    
		DispatchMessage( &msg );
	}
	
	BringWindowToTop( hWnd );
}

void renderspu_SystemSwapBuffers( WindowInfo *w, GLint flags )
{
	int return_value;

	/* peek at the windows message queue */
	renderspuHandleWindowMessages( w->visual->hWnd );

	/* render_to_app_window:
	 * w->nativeWindow will only be non-zero if the
	 * render_spu.render_to_app_window option is true and
	 * MakeCurrent() recorded the nativeWindow handle in the WindowInfo
	 * structure.
	 */
	if (w->nativeWindow)
		return_value = render_spu.ws.wglSwapBuffers( w->nativeWindow );
	else
		return_value = render_spu.ws.wglSwapBuffers( w->visual->device_context );
	if (!return_value)
	{
		/* GOD DAMN IT.  The latest versions of the NVIDIA drivers
	 	* return failure from wglSwapBuffers, but it works just fine.
	 	* WHAT THE HELL?! */

		crWarning( "wglSwapBuffers failed: return value of %d!", return_value);
	}
}
