# cpg - 5/22/02

import sys;
import cPickle;
import types;
import string;
import re;

import stub_common;

parsed_file = open( "../glapi_parser/gl_header.parsed", "rb" )
gl_mapping = cPickle.load( parsed_file )

keys = gl_mapping.keys()
keys.sort();

stub_common.CopyrightC()

print """#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>

#include "api_templates.h"
"""

for func_name in keys:
    if stub_common.FindSpecial( "exports", func_name ): continue
    if stub_common.FindSpecial( "noexport", func_name ): continue
    ( return_type, arg_names, arg_types ) = gl_mapping[func_name]

    print "%s gl%s%s" % (return_type, func_name, stub_common.ArgumentString( arg_names, arg_types ) )
    print "{"
    print "\t",

    if return_type != "void":
	print "return ",
    print "glim.%s%s;" % (func_name, stub_common.CallString( arg_names ))

    print "}"
    print ""
