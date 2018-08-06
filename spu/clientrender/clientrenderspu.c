/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "cr_environment.h"
#include "cr_string.h"
#include "cr_error.h"
#include "cr_mem.h"
#include "cr_spu.h"
#include "clientrenderspu.h"
#include "cr_extstring.h"


static void
DoSync(void)
{
	CRMessage *in, out;

	out.header.type = CR_MESSAGE_OOB;
	
	if (render_spu.is_swap_master)
	{
		int a;
	
		for (a = 0; a < render_spu.num_swap_clients; a++)
		{
			crNetGetMessage( render_spu.swap_conns[a], &in );			
			crNetFree( render_spu.swap_conns[a], in);
		}

		for (a = 0; a < render_spu.num_swap_clients; a++)
			crNetSend( render_spu.swap_conns[a], NULL, &out, sizeof(CRMessage));
	}
	else
	{
		crNetSend( render_spu.swap_conns[0], NULL, &out, sizeof(CRMessage));

		crNetGetMessage( render_spu.swap_conns[0], &in );			
		crNetFree( render_spu.swap_conns[0], in);
	}
}



/*
 * Visual functions
 */

/**
 * used for debugging and giving info to the user.
 */
void
renderspuMakeVisString( GLbitfield visAttribs, char *s )
{
	s[0] = 0;

	if (visAttribs & CR_RGB_BIT)
		crStrcat(s, "RGB");
	if (visAttribs & CR_ALPHA_BIT)
		crStrcat(s, "A");
	if (visAttribs & CR_DOUBLE_BIT)
		crStrcat(s, ", Doublebuffer");
	if (visAttribs & CR_STEREO_BIT)
		crStrcat(s, ", Stereo");
	if (visAttribs & CR_DEPTH_BIT)
		crStrcat(s, ", Z");
	if (visAttribs & CR_STENCIL_BIT)
		crStrcat(s, ", Stencil");
	if (visAttribs & CR_ACCUM_BIT)
		crStrcat(s, ", Accum");
	if (visAttribs & CR_MULTISAMPLE_BIT)
		crStrcat(s, ", Multisample");
	if (visAttribs & CR_OVERLAY_BIT)
		crStrcat(s, ", Overlay");
	if (visAttribs & CR_PBUFFER_BIT)
		crStrcat(s, ", PBuffer");
}


/*
 * Find a VisualInfo which matches the given display name and attribute
 * bitmask, or return a pointer to a new visual.
 */
VisualInfo *
renderspuFindVisual(const char *displayName, GLbitfield visAttribs)
{
	int i;

	if (!displayName)
		displayName = "";

	/* first, try to find a match */
#if defined(WINDOWS) || defined(DARWIN)
	for (i = 0; i < render_spu.numVisuals; i++) {
		if (visAttribs == render_spu.visuals[i].visAttribs) {
            crDebug("found the visual attributes");
			return &(render_spu.visuals[i]);
		}
	}
#elif defined(GLX)
	for (i = 0; i < render_spu.numVisuals; i++) {
		if (crStrcmp(displayName, render_spu.visuals[i].displayName) == 0
			&& visAttribs == render_spu.visuals[i].visAttribs) {
			return &(render_spu.visuals[i]);
		}
	}
#endif

	if (render_spu.numVisuals >= MAX_VISUALS)
	{
		crWarning("Render SPU: Couldn't create a visual, too many visuals already");
		return NULL;
	}

	/* create a new visual */
	i = render_spu.numVisuals;
	render_spu.visuals[i].displayName = crStrdup(displayName);
	render_spu.visuals[i].visAttribs = visAttribs;
	if (renderspu_SystemInitVisual(&(render_spu.visuals[i]))) {
        crDebug("new Visual is not created");
		render_spu.numVisuals++;
		return &(render_spu.visuals[i]);
	}
	else {
		crWarning("Render SPU: Couldn't get a visual, renderspu_SystemInitVisual failed");
		return NULL;
	}
}

GLint RENDER_APIENTRY
renderspuWindowCreate(const char *dpyName, GLint visBits)
{
	WindowInfo *window;
	VisualInfo *visual;
	GLboolean showIt;

	if (!dpyName || crStrlen(render_spu.display_string) > 0)
		dpyName = render_spu.display_string;

	visual = renderspuFindVisual(dpyName, visBits);
	if (!visual)
	{
		crWarning("Render SPU: Couldn't create a window, renderspuFindVisual returned NULL");
		return -1;
	}

	/* Allocate WindowInfo */
	window = (WindowInfo *)crCalloc(sizeof(WindowInfo));
	if (!window)
	{
		crWarning("Render SPU: Couldn't create a window");
		return -1;
	}

	crHashtableAdd(render_spu.windowTable, render_spu.window_id, window);
	window->id = render_spu.window_id;
	render_spu.window_id++;

	window->x = render_spu.defaultX;
	window->y = render_spu.defaultY;
	window->width = render_spu.defaultWidth;
	window->height = render_spu.defaultHeight;

	if ((render_spu.render_to_app_window || render_spu.render_to_crut_window) && !crGetenv("CRNEWSERVER"))
		showIt = 0;
	else
		showIt = window->id > 0;

	/* Set window->title, replacing %i with the window ID number */
	{
		const char *s = crStrstr(render_spu.window_title, "%i");
		if (s) {
			int i, j, k;
			window->title = crAlloc(crStrlen(render_spu.window_title) + 10);
			for (i = 0; render_spu.window_title[i] != '%'; i++)
				window->title[i] = render_spu.window_title[i];
			k = sprintf(window->title + i, "%d", window->id);
			CRASSERT(k < 10);
			j = i + k;
			i = 2; /* skip the %i in s */
			for (; (window->title[j] = s[i]) != 0; i++, j++)
				;
		}
		else {
			window->title = crStrdup(render_spu.window_title);
		}
	}	

	return window->id;
}

GLint RENDER_APIENTRY
renderspuCreateContext(const char *dpyName, GLint visBits, GLint shareCtx)
{
	ContextInfo *context, *sharedContext = NULL;
	VisualInfo *visual;

	if (shareCtx > 0) {
		sharedContext
			= (ContextInfo *)crHashtableSearch(render_spu.contextTable, shareCtx);
	}

	if (!dpyName || crStrlen(render_spu.display_string)>0)
		dpyName = render_spu.display_string;

	visual = renderspuFindVisual(dpyName, visBits);
	if (!visual){
		crDebug("Visual is not valid");
		return -1;
	}
	context = (ContextInfo *)crCalloc(sizeof(ContextInfo));
	if (!context){
		crDebug("Context is not valid");
		return -1;
	}
	else{
		crDebug("Context is valid");
	}
	context->id = render_spu.context_id;
	context->shared = sharedContext;

	crHashtableAdd(render_spu.contextTable, render_spu.context_id, context);
	render_spu.context_id++;

	/*
	crDebug("Render SPU: CreateContext(%s, 0x%x) returning %d",
	dpyName, visBits, context->id);
	*/

	return context->id;
}


/*
 * Barrier functions
 * Normally, we'll have a crserver somewhere that handles the barrier calls.
 * However, if we're running the render SPU on the client node, then we
 * should handle barriers here.  The threadtest demo illustrates this.
 * If we have N threads calling using this SPU we need these barrier
 * functions to synchronize them.
 */

static void RENDER_APIENTRY renderspuBarrierCreateCR( GLuint name, GLuint count )
{
	Barrier *b;

	if (render_spu.ignore_papi)
		return;

	b = (Barrier *) crHashtableSearch( render_spu.barrierHash, name );
	if (b) {
		/* HACK -- this allows everybody to create a barrier, and all
           but the first creation are ignored, assuming the count
           match. */
		if ( b->count != count ) {
			crError( "Render SPU: Barrier name=%u created with count=%u, but already "
					 "exists with count=%u", name, count, b->count );
		}
	}
	else {
		b = (Barrier *) crAlloc( sizeof(Barrier) );
		b->count = count;
		crInitBarrier( &b->barrier, count );
		crHashtableAdd( render_spu.barrierHash, name, b );
	}
}

static void RENDER_APIENTRY renderspuBarrierDestroyCR( GLuint name )
{
	if (render_spu.ignore_papi)
		return;
	crHashtableDelete( render_spu.barrierHash, name, crFree );
}

static void RENDER_APIENTRY renderspuBarrierExecCR( GLuint name )
{
	Barrier *b;

	if (render_spu.ignore_papi)
		return;

	b = (Barrier *) crHashtableSearch( render_spu.barrierHash, name );
	if (b) {
		crWaitBarrier( &(b->barrier) );
	}
	else {
		crWarning("Render SPU: Bad barrier name %d in BarrierExec()", name);
	}
}


/*
 * Semaphore functions
 * XXX we should probably implement these too, for the same reason as
 * barriers (see above).
 */

static void RENDER_APIENTRY renderspuSemaphoreCreateCR( GLuint name, GLuint count )
{
	(void) name;
	(void) count;
}

static void RENDER_APIENTRY renderspuSemaphoreDestroyCR( GLuint name )
{
	(void) name;
}

static void RENDER_APIENTRY renderspuSemaphorePCR( GLuint name )
{
	(void) name;
}

static void RENDER_APIENTRY renderspuSemaphoreVCR( GLuint name )
{
	(void) name;
}


/*
 * Misc functions
 */



static void RENDER_APIENTRY renderspuChromiumParameteriCR(GLenum target, GLint value)
{
	(void) target;
	(void) value;


#if 0
	switch (target) 
	{
		default:
			crWarning("Unhandled target in renderspuChromiumParameteriCR()");
			break;
	}
#endif
}

static void RENDER_APIENTRY
renderspuChromiumParameterfCR(GLenum target, GLfloat value)
{
	(void) target;
	(void) value;

#if 0
	switch (target) {
	default:
		crWarning("Unhandled target in renderspuChromiumParameterfCR()");
		break;
	}
#endif
}


static void RENDER_APIENTRY
renderspuChromiumParametervCR(GLenum target, GLenum type, GLsizei count,
															const GLvoid *values)
{
	int client_num;
	unsigned short port;
	CRMessage *msg, pingback;
	unsigned char *privbuf = NULL;

	switch (target) {

		case GL_GATHER_CONNECT_CR:
			if (render_spu.gather_userbuf_size)
				privbuf = (unsigned char *)crAlloc(1024*768*4);
		
			port = ((GLint *) values)[0];

			if (render_spu.gather_conns == NULL)
				render_spu.gather_conns = crAlloc(render_spu.server->numClients*sizeof(CRConnection *));
			else
			{
				crError("Oh bother! duplicate GL_GATHER_CONNECT_CR getting through");
			}

			for (client_num=0; client_num< render_spu.server->numClients; client_num++)
			{
				switch (render_spu.server->clients[client_num]->conn->type)
				{
					case CR_TCPIP:
						crDebug("Render SPU: AcceptClient from %s on %d", 
							render_spu.server->clients[client_num]->conn->hostname, render_spu.gather_port);
						render_spu.gather_conns[client_num] = 
								crNetAcceptClient("tcpip", NULL, port, 1024*1024,  1);
						break;
					
					case CR_GM:
						render_spu.gather_conns[client_num] = 
								crNetAcceptClient("gm", NULL, port, 1024*1024,  1);
						break;
						
					default:
						crError("Render SPU: Unknown Network Type to Open Gather Connection");
				}

		
				if (render_spu.gather_userbuf_size)
				{
					render_spu.gather_conns[client_num]->userbuf = privbuf;
					render_spu.gather_conns[client_num]->userbuf_len = render_spu.gather_userbuf_size;
				}
				else
				{
					render_spu.gather_conns[client_num]->userbuf = NULL;
					render_spu.gather_conns[client_num]->userbuf_len = 0;
				}

				if (render_spu.gather_conns[client_num])
				{
					crDebug("Render SPU: success! from %s", render_spu.gather_conns[client_num]->hostname);
				}
			}

			break;

	case GL_GATHER_DRAWPIXELS_CR:
		pingback.header.type = CR_MESSAGE_OOB;

		for (client_num=0; client_num< render_spu.server->numClients; client_num++)
		{
			crNetGetMessage(render_spu.gather_conns[client_num], &msg); 
			if (msg->header.type == CR_MESSAGE_GATHER)
			{
				crNetFree(render_spu.gather_conns[client_num], msg);
			}
			else
			{
				crError("Render SPU: expecting MESSAGE_GATHER. got crap! (%d of %d)",
								client_num, render_spu.server->numClients-1);
			}
		}

		/* 
		 * We're only hitting the case if we're not actually calling 
		 * child.SwapBuffers from readback, so a switch about which
		 * call to DoSync() we really want [this one, or the one
		 * in SwapBuffers above] is not necessary -- karl
		 */
		
		if (render_spu.swap_master_url)
			DoSync();

		for (client_num=0; client_num< render_spu.server->numClients; client_num++)
			crNetSend(render_spu.gather_conns[client_num], NULL, &pingback,
										sizeof(CRMessageHeader));

		render_spu.self.RasterPos2i(((GLint *)values)[0], ((GLint *)values)[1]);
		render_spu.self.DrawPixels(  ((GLint *)values)[2], ((GLint *)values)[3], 
										((GLint *)values)[4], ((GLint *)values)[5], 
									render_spu.gather_conns[0]->userbuf);


		render_spu.self.SwapBuffers(((GLint *)values)[6], 0);
		break;

	case GL_CURSOR_POSITION_CR:
		if (type == GL_INT && count == 2) {
			render_spu.cursorX = ((GLint *) values)[0];
			render_spu.cursorY = ((GLint *) values)[1];
		}
		else {
			crWarning("Render SPU: Bad type or count for ChromiumParametervCR(GL_CURSOR_POSITION_CR)");
		}
		break;

	case GL_WINDOW_SIZE_CR:
		/* XXX this is old code that should be removed.
		 * NOTE: we can only resize the default (id=0) window!!!
		 */
		{
			GLint w, h;
			WindowInfo *window;
			CRASSERT(type == GL_INT);
			CRASSERT(count == 2);
			CRASSERT(values);
			w = ((GLint*)values)[0];
			h = ((GLint*)values)[1];
			window = (WindowInfo *) crHashtableSearch(render_spu.windowTable, 0);
			if (window)
			{
				renderspu_SystemWindowSize(window, w, h);
			}
		}
		break;

	default:
#if 0
		crWarning("Unhandled target in renderspuChromiumParametervCR(0x%x)", (int) target);
#endif
		break;
	}
}


static void RENDER_APIENTRY
renderspuGetChromiumParametervCR(GLenum target, GLuint index, GLenum type,
																 GLsizei count, GLvoid *values)
{
	switch (target) {
	case GL_WINDOW_SIZE_CR:
		{
			GLint x, y, w, h, *size = (GLint *) values;
			WindowInfo *window;
			CRASSERT(type == GL_INT);
			CRASSERT(count == 2);
			CRASSERT(values);
			// Since read back SPU expect a non zero window size
			size[0] = size[1] = 500;  /* default */
			window = (WindowInfo *) crHashtableSearch(render_spu.windowTable, index);
			if (window)
			{
				renderspu_SystemGetWindowGeometry(window, &x, &y, &w, &h);
				size[0] = w;
				size[1] = h;
			}
		}
		break;
	case GL_WINDOW_POSITION_CR:
		/* return window position, as a screen coordinate */
		{
			GLint *pos = (GLint *) values;
			GLint x, y, w, h;
			WindowInfo *window;
			CRASSERT(type == GL_INT);
			CRASSERT(count == 2);
			CRASSERT(values);
			pos[0] = pos[1] = 0;  /* default */
			window = (WindowInfo *) crHashtableSearch(render_spu.windowTable, index);
			if (window)
			{
				renderspu_SystemGetWindowGeometry(window, &x, &y, &w, &h);
				pos[0] = x;/*window->x;*/
				pos[1] = y;/*window->y;*/
			}
		}
		break;
	case GL_MAX_WINDOW_SIZE_CR:
		{
			GLint *maxSize = (GLint *) values;
			WindowInfo *window;
			CRASSERT(type == GL_INT);
			CRASSERT(count == 2);
			CRASSERT(values);
			window = (WindowInfo *) crHashtableSearch(render_spu.windowTable, index);
			if (window)
			{
				renderspu_SystemGetMaxWindowSize(window, maxSize + 0, maxSize + 1);
			}
		}
		break;
	default:
		; /* nothing - silence compiler */
	}
}


static void RENDER_APIENTRY
renderspuBoundsInfoCR( CRrecti *bounds, GLbyte *payload, GLint len,
											 GLint num_opcodes )
{
	(void) bounds;
	(void) payload;
	(void) len;
	(void) num_opcodes;
	/* draw the bounding box */
	if (render_spu.draw_bbox) {
		GET_CONTEXT(context);
		WindowInfo *window = context->currentWindow;
		GLint x, y, w, h;

		renderspu_SystemGetWindowGeometry(window, &x, &y, &w, &h);

		render_spu.self.PushMatrix();
		render_spu.self.LoadIdentity();
		render_spu.self.MatrixMode(GL_PROJECTION);
		render_spu.self.PushMatrix();
		render_spu.self.LoadIdentity();
		render_spu.self.Ortho(0, w, 0, h, -1, 1);
		render_spu.self.Color3f(1, 1, 1);
		render_spu.self.Begin(GL_LINE_LOOP);
		render_spu.self.Vertex2i(bounds->x1, bounds->y1);
		render_spu.self.Vertex2i(bounds->x2, bounds->y1);
		render_spu.self.Vertex2i(bounds->x2, bounds->y2);
		render_spu.self.Vertex2i(bounds->x1, bounds->y2);
		render_spu.self.End();
		render_spu.self.PopMatrix();
		render_spu.self.MatrixMode(GL_MODELVIEW);
		render_spu.self.PopMatrix();
	}
}


static void RENDER_APIENTRY
renderspuWriteback( GLint *writeback )
{
	(void) writeback;
}


static void
remove_trailing_space(char *s)
{
	int k = crStrlen(s);
	while (k > 0 && s[k-1] == ' ')
		k--;
	s[k] = 0;
}

static const GLubyte * RENDER_APIENTRY
renderspuGetString(GLenum pname)
{
	static char tempStr[1000];
	GET_CONTEXT(context);

	if (pname == GL_EXTENSIONS)
	{
		return render_spu.ws.glGetString(GL_EXTENSIONS);
	}
	else if (pname == GL_VENDOR)
		return (const GLubyte *) CR_VENDOR;
	else if (pname == GL_VERSION)
		return render_spu.ws.glGetString(GL_VERSION);
	else if (pname == GL_RENDERER) {
		sprintf(tempStr, "Chromium (%s)", (char *) render_spu.ws.glGetString(GL_RENDERER));
		return (const GLubyte *) tempStr;
	}
	else
		return NULL;
}


#define FILLIN( NAME, FUNC ) \
  table[i].name = crStrdup(NAME); \
  table[i].fn = (SPUGenericFunction) FUNC; \
  i++;


/* These are the functions which the render SPU implements, not OpenGL.
 */
int
renderspuCreateFunctions(SPUNamedFunctionTable table[])
{
	int i = 0;
	FILLIN( "WindowCreate", renderspuWindowCreate);
	FILLIN( "CreateContext", renderspuCreateContext);
	FILLIN( "BarrierCreateCR", renderspuBarrierCreateCR );
	FILLIN( "BarrierDestroyCR", renderspuBarrierDestroyCR );
	FILLIN( "BarrierExecCR", renderspuBarrierExecCR );
	FILLIN( "BoundsInfoCR", renderspuBoundsInfoCR );
	FILLIN( "SemaphoreCreateCR", renderspuSemaphoreCreateCR );
	FILLIN( "SemaphoreDestroyCR", renderspuSemaphoreDestroyCR );
	FILLIN( "SemaphorePCR", renderspuSemaphorePCR );
	FILLIN( "SemaphoreVCR", renderspuSemaphoreVCR );
	FILLIN( "Writeback", renderspuWriteback );
	FILLIN( "ChromiumParameteriCR", renderspuChromiumParameteriCR );
	FILLIN( "ChromiumParameterfCR", renderspuChromiumParameterfCR );
	FILLIN( "ChromiumParametervCR", renderspuChromiumParametervCR );
	FILLIN( "GetChromiumParametervCR", renderspuGetChromiumParametervCR );
	FILLIN( "GetString", renderspuGetString );
	return i;
}
