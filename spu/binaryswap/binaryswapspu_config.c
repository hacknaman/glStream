/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "binaryswapspu.h"
#include "cr_mothership.h"
#include "cr_string.h"
#include "cr_mem.h"
#include "cr_error.h"

#include <math.h>


static void set_peers( void *foo, const char *response )
{
   char *nodes;
   char *token;
   int i;

   /* figure out peer information! */
   /* grab network configuration */
   /* get count of things first so we can allocate space */
   int count = 0;
   nodes = crStrdup(response);
   if((token = strtok(nodes, ",\n\t\0 ")) != NULL){
      count = 1;
      while((token = strtok(NULL, ",\n\t\0 ")) != NULL){
	 count++;
      }
   }
   else{
      crError("Bad mojo: I can't figure out how many peers you have!");
   }
   crFree(nodes);
    
   /* actually store the network peer list */
   nodes = crStrdup(response);
   binaryswap_spu.peer_names = crAlloc(count*sizeof(char*));
   if((token = strtok(nodes, ",\n\t ")) != NULL){
      i = 0;
      binaryswap_spu.peer_names[i++] = crStrdup(token);
      while((token = strtok(NULL, ",\n\t ")) != NULL){
	 binaryswap_spu.peer_names[i++] = crStrdup(token);
      }
   }
   crFree(nodes);
    
   /* figure out how many stages we have */
   binaryswap_spu.stages = (int)(log(count)/log(2)+0.1);
   crDebug("Swapping Stages: %d", binaryswap_spu.stages);
}


static void set_node( void *foo, const char *response )
{
   if (*response) {
      binaryswap_spu.node_num = crStrToInt( response );
   }
   else {
      crError( "No node number specified for the binaryswap SPU?" );
   }
}

static void set_swapmtu( void *foo, const char *response )
{
   if (*response) {
      binaryswap_spu.mtu = crStrToInt( response );
   }
   else { 
     binaryswap_spu.mtu = -1;
     crWarning("No swap MTU specified, using default from mothership" );
   }
}

static void set_type( void *foo, const char *response )
{
   if(strcmp( response, "alpha") == 0){
      binaryswap_spu.alpha_composite = 1;
      binaryswap_spu.depth_composite = 0;
   }
   else if(strcmp( response, "depth") == 0){
      binaryswap_spu.depth_composite = 1;
      binaryswap_spu.alpha_composite = 0;
   }
   else{
      crError( "Bad composite type specified for the binaryswap SPU?" );
   }
}

static void __setDefaults( BinaryswapSPU *binaryswap_spu )
{
  /* default window */
  binaryswap_spu->windows[0].inUse = GL_TRUE;
  binaryswap_spu->windows[0].renderWindow = 0;
  binaryswap_spu->windows[0].childWindow = 0;
  binaryswap_spu->barrierCount = 0;
  
  /* This will make the viewport be computed later */
  binaryswap_spu->halfViewportWidth = 0;
  binaryswap_spu->cleared_this_frame = 0;
  binaryswap_spu->bbox = NULL;
  
  binaryswap_spu->resizable = 0;
}

void set_extract_depth( BinaryswapSPU *binaryswap_spu, const char *response )
{
   binaryswap_spu->extract_depth = crStrToInt( response );
}

void set_extract_alpha( BinaryswapSPU *binaryswap_spu, const char *response )
{
   binaryswap_spu->extract_alpha = crStrToInt( response );
}

void set_local_visualization( BinaryswapSPU *binaryswap_spu, const char *response )
{
   binaryswap_spu->local_visualization = crStrToInt( response );
}

void set_visualize_depth( BinaryswapSPU *binaryswap_spu, const char *response )
{
   binaryswap_spu->visualize_depth = crStrToInt( response );
}

void set_drawpixels_pos( BinaryswapSPU *binaryswap_spu, const char *response )
{
   sscanf( response, "%d %d", &binaryswap_spu->drawX, &binaryswap_spu->drawY );
}


/* option, type, nr, default, min, max, title, callback
 */
SPUOptions binaryswapSPUOptions[] = {

   /* Really a multiple choice:  Extract depth or alpha.
    */
   { "extract_depth", CR_BOOL, 1, "0", NULL, NULL,
     "Extract Depth Values", (SPUOptionCB)set_extract_depth },

   { "extract_alpha", CR_BOOL, 1, "0", NULL, NULL,
     "Extract Alpha Values", (SPUOptionCB)set_extract_alpha },

   { "local_visualization", CR_BOOL, 1, "1", NULL, NULL,
     "Local Visualization", (SPUOptionCB)set_local_visualization },

   { "visualize_depth", CR_BOOL, 1, "0", NULL, NULL,
     "Visualize Depth as Grayscale", (SPUOptionCB)set_visualize_depth },

   { "drawpixels_pos", CR_INT, 2, "0, 0", "0, 0", NULL,
     "glDrawPixels Position (x,y)", (SPUOptionCB)set_drawpixels_pos },

   { "peers", CR_STRING, 1, "", NULL, NULL, 
     "Peers", (SPUOptionCB)set_peers},

   { "node_number", CR_STRING, 1, "", NULL, NULL, 
     "Node Number", (SPUOptionCB)set_node},

   /* Really an enumerate (alpha/depth), not a string
    */
   { "type", CR_STRING, 1, "depth", NULL, NULL, 
     "Composite type (alpha/depth)", (SPUOptionCB)set_type},

   { "swapmtu", CR_STRING, 1, "", NULL, NULL, 
     "MTU size for swapping", (SPUOptionCB)set_swapmtu},

   { NULL, CR_BOOL, 0, NULL, NULL, NULL, NULL, NULL },

};


void binaryswapspuGatherConfiguration( BinaryswapSPU *binaryswap_spu )
{
  int i;
  CRConnection *conn;
  
  __setDefaults( binaryswap_spu );
  
  /* Connect to the mothership and identify ourselves. */
  
  conn = crMothershipConnect( );
  if (!conn)
    {
      /* The mothership isn't running.  Some SPU's can recover gracefully, some 
       * should issue an error here. */
      crSPUSetDefaultParams( binaryswap_spu, binaryswapSPUOptions );
      return;
    }
  crMothershipIdentifySPU( conn, binaryswap_spu->id );
  
  crSPUGetMothershipParams( conn, (void *)binaryswap_spu, binaryswapSPUOptions );
  
  /* we can either composite with alpha or Z, but not both */
  if (binaryswap_spu->extract_depth && binaryswap_spu->extract_alpha) {
    crWarning("Binaryswap SPU can't extract both depth and alpha, using depth");
    binaryswap_spu->extract_alpha = 0;
  }
  
  /* Get the render SPU's resizable setting */
  {
    char response[1000];
    if (crMothershipGetSPUParam( conn, response, "resizable" )) {
      int resizable = 0;
      sscanf(response, "%d", &resizable);
      binaryswap_spu->resizable = resizable;
    }
    printf(">>>>>>> resizable = %d\n", binaryswap_spu->resizable);
  }

  /* build list of swap partners */
  binaryswap_spu->swap_partners = crAlloc(binaryswap_spu->stages*sizeof(char*));
  for( i=0; i<binaryswap_spu->stages; i++ ){
    /* are we the high in the pair? */
    if((binaryswap_spu->node_num%((int)pow(2, i+1)))/((int)pow(2, i))){
      binaryswap_spu->swap_partners[i] = crStrdup(binaryswap_spu->peer_names[binaryswap_spu->node_num - (int)pow(2, i)]);
    }
    /* or the low? */
    else{
      binaryswap_spu->swap_partners[i] = crStrdup(binaryswap_spu->peer_names[binaryswap_spu->node_num + (int)pow(2, i)]);
    }
  }	

  if(binaryswap_spu->mtu == -1){
    binaryswap_spu->mtu = crMothershipGetMTU( conn );
  }

  crMothershipDisconnect( conn );
}
