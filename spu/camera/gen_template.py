# Copyright (c) 2001, Stanford University
# All rights reserved.
#
# See the file LICENSE.txt for information on redistributing this software.

import os, os.path
import sys

if os.getcwd().find( "camera" ) != -1:
	print >> sys.stderr, "You're running this script in the camera directory, which I'm SURE you don't want to do!"
	sys.exit(-1)

if len( sys.argv ) != 2:
	print >> sys.stderr, "Usage: %s <SPUNAME>" % sys.argv[0]
	sys.exit(-1)

spuname = sys.argv[1]

def ProcessFile( fn, spuname ):
	print >> sys.stderr, "Processing file %s..." % fn
	inputfile = open( fn, 'r' )
	outputfile = open( fn + "_TEMP", 'w' )
	for line in inputfile.readlines():
		newline = line.replace( "camera", spuname )
		newline = newline.replace( "CAMERA", spuname.upper() )
		newline = newline.replace( "Camera", (spuname[:1].upper() + spuname[1:]) )
		outputfile.write( newline )
	inputfile.close()
	outputfile.close()
	os.unlink( fn )
	os.rename( fn + "_TEMP", fn.replace( "camera", spuname ) )

ProcessFile( "cameraspu.c", spuname )
ProcessFile( "cameraspu.h", spuname )
ProcessFile( "cameraspu_config.c", spuname )
ProcessFile( "cameraspu_init.c", spuname )
ProcessFile( "cameraspu.def", spuname )
ProcessFile( "cameraspu_proto.py", spuname )
ProcessFile( "Makefile", spuname )
