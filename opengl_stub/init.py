import sys,os;
import cPickle;
import string;
import re;

import stub_common;

parsed_file = open( "../glapi_parser/gl_header.parsed", "rb" )
gl_mapping = cPickle.load( parsed_file )

print """#include "cr_spu.h"
#include "api_templates.h"

#ifdef WINDOWS
// Let me cast function pointers to data pointers, I know what I'm doing.
#pragma warning( disable: 4054 )
#endif

void FakerInit( SPU *spu )
{"""

keys = gl_mapping.keys()
keys.sort();

for func_name in keys:
    print "\t%s = (void *) spu->dispatch_table.%s;" % (stub_common.DoImmediateMapping( func_name ), func_name )

print '}'
