/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "cr_mem.h"
#include "cr_environment.h"
#include "cr_string.h"
#include "cr_dll.h"
#include "cr_error.h"
#include "cr_spu.h"

#include <stdlib.h>
#include <stdio.h>

#ifdef WINDOWS
#define DLL_SUFFIX ".dll"
#define DLL_PREFIX ""
#else
#define DLL_SUFFIX ".so"
#define DLL_PREFIX "lib"
#endif

extern void __buildDispatch( SPU *spu );

static char *__findDLL( char *name, char *dir )
{
	static char path[8092];
	
	if (!dir)
	{
		sprintf ( path, "%s%sspu%s", DLL_PREFIX, name, DLL_SUFFIX );
	}
	else
	{
		sprintf ( path, "%s/%s%sspu%s", dir, DLL_PREFIX, name, DLL_SUFFIX );
	}
	return path;
}

/* Load a single SPU from disk and initialize it.  Is there any reason 
 * to export this from the SPU loader library? */

SPU * crSPULoad( SPU *child, int id, char *name, char *dir, void *server )
{
	SPU *the_spu;
	char *path;

	CRASSERT( name != NULL );

	the_spu = (SPU*)crAlloc( sizeof( *the_spu ) );
	the_spu->id = id;
	path = __findDLL( name, dir );
	the_spu->dll = crDLLOpen( path );
	the_spu->entry_point = 
		(SPULoadFunction) crDLLGetNoError( the_spu->dll, SPU_ENTRY_POINT_NAME );
	if (!the_spu->entry_point)
	{
		crError( "Couldn't load the SPU entry point \"%s\" from SPU \"%s\"!", 
				SPU_ENTRY_POINT_NAME, name );
	}

	/* This basicall calls the SPU's SPULoad() function */
	if (!the_spu->entry_point( &(the_spu->name), &(the_spu->super_name), 
				&(the_spu->init), &(the_spu->self), 
				&(the_spu->cleanup) ) )
	{
		crError( "I found the SPU \"%s\", but loading it failed!", name );
	}
	if (crStrcmp(the_spu->name,"error"))
	{
		/* the default super/base class for an SPU is the error SPU */
		if (the_spu->super_name == NULL)
		{
			the_spu->super_name = "error";
		}
		the_spu->superSPU = crSPULoad( child, id, the_spu->super_name, dir, server );
	}
	else
	{
		the_spu->superSPU = NULL;
	}
	the_spu->function_table = the_spu->init( id, child, the_spu->superSPU, 0, 1 );
	__buildDispatch( the_spu );
	crDebug( "initializing dispatch table %p (for SPU %s)", &(the_spu->dispatch_table), name );
	crSPUInitDispatchTable( &(the_spu->dispatch_table) );
	crDebug( "Done initializing the dispatch table for SPU %s, calling the self function", name );

	the_spu->dispatch_table.server = server;
	the_spu->self( &(the_spu->dispatch_table) );
	crDebug( "Done with the self function" );

	return the_spu;
}

/* Load the entire chain of SPUs and initialize all of them. 
 * This function returns the first one in the chain */

SPU * crSPULoadChain( int count, int *ids, char **names, char *dir, void *server )
{
	int i;
	SPU *child_spu = NULL;
	CRASSERT( count > 0 );

	for (i = count-1 ; i >= 0 ; i--)
	{
		int spu_id = ids[i];
		char *spu_name = names[i];
		SPU *the_spu, *temp;
		
		/* This call passes the previous version of spu, which is the SPU's 
		 * "child" in this chain. */

		the_spu = crSPULoad( child_spu, spu_id, spu_name, dir, server );
		if (child_spu != NULL)
		{
			/* keep track of this so that people can pass functions through but 
			 * still get updated when API's change on the fly. */
			for (temp = the_spu ; temp ; temp = temp->superSPU )
			{
				struct _copy_list_node *node = (struct _copy_list_node *) crAlloc( sizeof( *node ) );
				node->copy = &(temp->dispatch_table);
				node->next = child_spu->dispatch_table.copyList;
				child_spu->dispatch_table.copyList = node;
			}
		}
		child_spu = the_spu;
	}
	return child_spu;
}
