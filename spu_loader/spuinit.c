/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "cr_spu.h"
#include "cr_error.h"
#include "cr_mothership.h"
#include <stdio.h>

void crSPUInitDispatchTable( SPUDispatchTable *table )
{
	table->copy_of = NULL;
	table->copyList = NULL;
	table->mark = 0;
}

static int validate_int( const char *response, 
			 const char *min,
			 const char *max )
{
   int i,imin,imax;   
   if (sscanf(response, "%d", &i) != 1)
      return 0;
   if (min && sscanf(min, "%d", &imin) == 1 && imin > i)
      return 0;
   if (max && sscanf(max, "%d", &imax) == 1 && imax < i)
      return 0;
   return 1;
}

static int validate_float( const char *response, 
			 const char *min,
			 const char *max )
{
   float f,fmin,fmax;
   if (sscanf(response, "%f", &f) != 1)
      return 0;
   if (min && sscanf(min, "%f", &fmin) == 1 && fmin > f)
      return 0;
   if (max && sscanf(max, "%f", &fmax) == 1 && fmax < f)
      return 0;
   return 1;
}

static int validate_one_option( SPUOptions *opt, 
				const char *response,
				const char *min,
				const char *max )
{
   switch (opt->type) {
   case BOOL: return validate_int( response, "0", "1" );
   case INT: return validate_int( response, min, max );
   case FLOAT: return validate_float( response, min, max );
   default: return 0;
   }
}

static int validate_option( SPUOptions *opt, const char *response )
{
   const char *min = opt->min;
   const char *max = opt->max;
   int i = 0;

   if (opt->type == STRING)
      return 1;
   
   if (opt->numValues == 0)
      return 1;

   for (;;) {
      if (!validate_one_option( opt, response, min, max )) {
	 return 0;
      }
      if (++i == opt->numValues) {
	 return 1;
      }
      if (min) {
	 while (*min != ' ' && *min) min++;
	 while (*min == ' ') min++;
      }
      if (max) {
	 while (*max != ' ' && *max) max++;
	 while (*max == ' ') max++;
      }
      if (response) {
	 while (*response != ' ' && *response) response++;
	 while (*response == ' ') response++;
      }
   }
   
   return 1;
}

/* Query the mothership, else use the default value for each option:
 */
void crSPUGetMothershipParams( CRConnection *conn,
			       void *spu, 
			       SPUOptions *options )
{
   char response[8096];
   int i;
	
   for (i = 0 ; options[i].option ; i++) {
      SPUOptions *opt = &options[i];

      if (crMothershipGetSPUParam( conn, response, opt->option )) {
	 if (!validate_option( opt, response )) {
	    crError( "Validation failed for option %s, response \"%s\"\n", 
		     opt->option, response );
	 }
	 opt->cb( spu, response );
      }
      else {
	 opt->cb( spu, opt->deflt );
      }
   }
}

/* Use the default values for all the options:
 */
void crSPUSetDefaultParams( void *spu, SPUOptions *options )
{
   int i;
	
   for (i = 0 ; options[i].option ; i++) {
      SPUOptions *opt = &options[i];
      opt->cb( spu, opt->deflt );
   }
}
