#include "cr_spu.h"
#include "cr_net.h"
#include "cr_error.h"
#include "cr_mem.h"
#include "cr_string.h"
#include "cr_mothership.h"

SPU *stub_spu = NULL;
extern void FakerInit( SPU *fns );

static void __stubInit(void)
{
	// Here is where we contact the mothership to find out what we're supposed to
	// be doing.  Networking code in a DLL initializer.  I sure hope this
	// works :)
	//
	// HOW can I pass the mothership address to this if I already know it?
	
	CRConnection *conn;
	char response[1024];
	char hostname[1024];
	char **spuchain;
	int num_spus;
	int *spu_ids;
	char **spu_names;
	int i;
	
	crNetInit( NULL, NULL );
	conn = crMothershipConnect( NULL );
	if (crGetHostname( hostname, sizeof( hostname ) ) )
	{
		crError( "Couldn't get my own hostname in the OpenGL DLL!" );
	}
	if (!crMothershipSendString( conn, response, "opengldll %s", hostname ))
	{
		crError( "Mothership didn't like my OpenGL stub: %s", response );
	}
	crMothershipDisconnect( conn );
	crDebug( "response = \"%s\"", response );
	spuchain = crStrSplit( response, " " );
	num_spus = crStrToInt( spuchain[0] );
	spu_ids = (int *) crAlloc( num_spus * sizeof( *spu_ids ) );
	spu_names = (char **) crAlloc( num_spus * sizeof( *spu_names ) );
	for (i = 0 ; i < num_spus ; i++)
	{
		spu_ids[i] = crStrToInt( spuchain[2*i+1] );
		spu_names[i] = crStrdup( spuchain[2*i+2] );
		crDebug( "SPU %d/%d: (%d) \"%s\"", i+1, num_spus, spu_ids[i], spu_names[i] );
	}

	stub_spu = LoadSPUChain( num_spus, spu_ids, spu_names );

	// This is unlikely to change -- We still want to initialize our dispatch
	// table with the functions of the first SPU in the chain.

	FakerInit( stub_spu );
}

#ifdef LINUX
// GCC crap
void (*stub_init_ptr)(void) __attribute__((section(".ctors"))) = __stubInit;
#endif

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Windows crap
BOOL WINAPI DllMain( HINSTANCE instance, DWORD fdwReason, LPVOID lpvReserved )
{
	(void) lpvReserved;
	(void) instance;
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		__stubInit();
	}
	return TRUE;
}
#endif
