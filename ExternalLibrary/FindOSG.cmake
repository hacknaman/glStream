# This module defines
# OSG_FOUND, if false, do not try to link to OSG
# OSG_INCLUDE_DIR , where to find the headers
#
# Created by Naman

FIND_PATH(OSG_INCLUDE_DIR  osg/Node  
		$ENV{OSG_DIR}/include
		NO_DEFAULT_PATH    
	)

find_library(OSG_LIBRARY 
		NAMES osg
		PATHS $ENV{OSG_DIR}/lib/
		NO_DEFAULT_PATH    
)

find_library(OSG_LIBRARY_D 
		NAMES osgd
		PATHS $ENV{OSG_DIR}/lib/
		NO_DEFAULT_PATH    
)

find_library(OSG_VIEWER_LIBRARY 
		NAMES osgViewer
		PATHS $ENV{OSG_DIR}/lib/
		NO_DEFAULT_PATH    
)

find_library(OSG_VIEWER_LIBRARY_D 
		NAMES osgViewerd
		PATHS $ENV{OSG_DIR}/lib/
		NO_DEFAULT_PATH    
)

find_library(OSG_GA_LIBRARY 
		NAMES osgGA
		PATHS $ENV{OSG_DIR}/lib/
		NO_DEFAULT_PATH    
)

find_library(OSG_GA_LIBRARY_D 
		NAMES osgGAd
		PATHS $ENV{OSG_DIR}/lib/
		NO_DEFAULT_PATH    
)

find_library(OPENTHREADS_LIBRARY 
		NAMES OpenThreads
		PATHS $ENV{OSG_DIR}/lib/
		NO_DEFAULT_PATH    
)

find_library(OPENTHREADS_LIBRARY_D 
		NAMES OpenThreadsd
		PATHS $ENV{OSG_DIR}/lib/
		NO_DEFAULT_PATH    
)

find_library(OSGDB_LIBRARY 
		NAMES osgDB
		PATHS $ENV{OSG_DIR}/lib/
		NO_DEFAULT_PATH    
)

find_library(OSGDB_LIBRARY_D 
		NAMES osgDBd
		PATHS $ENV{OSG_DIR}/lib/
		NO_DEFAULT_PATH    
)

find_library(OSGTEXT_LIBRARY 
		NAMES osgText
		PATHS $ENV{OSG_DIR}/lib/
		NO_DEFAULT_PATH    
)

find_library(OSGTEXT_LIBRARY_D 
		NAMES osgTextd
		PATHS $ENV{OSG_DIR}/lib/
		NO_DEFAULT_PATH    
)

find_library(OSGUTIL_LIBRARY 
		NAMES osgUtil
		PATHS $ENV{OSG_DIR}/lib/
		NO_DEFAULT_PATH    
)

find_library(OSGUTIL_LIBRARY_D 
		NAMES osgUtild
		PATHS $ENV{OSG_DIR}/lib/
		NO_DEFAULT_PATH    
)


SET(OSG_FOUND "NO")
IF(OSG_LIBRARY AND OSG_INCLUDE_DIR )
    SET(OSG_FOUND "YES")
ELSE(OSG_LIBRARY AND OSG_INCLUDE_DIR)
    MESSAGE("OSG Not Found")
ENDIF(OSG_LIBRARY AND OSG_INCLUDE_DIR)