# This module defines
# OSG_FOUND, if false, do not try to link to OSG
# OSG_INCLUDE_DIR , where to find the headers
#
# Created by Naman

FIND_PATH(OSG_INCLUDE_DIR  osg/Node  
		$ENV{OSG_DIR}/include
		NO_DEFAULT_PATH    
	)

if( $ENV{BUILDENV} MATCHES "Release")
	
	find_library(OSG_LIBRARY 
			NAMES osg
			PATHS $ENV{OSG_DIR}/lib/
			NO_DEFAULT_PATH    
    )
else( $ENV{BUILDENV} MATCHES "Release") 
	find_library(OSG_LIBRARY 
			NAMES osgd
			PATHS $ENV{OSG_DIR}/lib/
			NO_DEFAULT_PATH    
    )
endif( $ENV{BUILDENV} MATCHES "Release")

if( $ENV{BUILDENV} MATCHES "Release")
	find_library(OSG_VIEWER_LIBRARY 
			NAMES osgViewer
			PATHS $ENV{OSG_DIR}/lib/
			NO_DEFAULT_PATH    
    )
else( $ENV{BUILDENV} MATCHES "Release")
	find_library(OSG_VIEWER_LIBRARY 
			NAMES osgViewerd
			PATHS $ENV{OSG_DIR}/lib/
			NO_DEFAULT_PATH    
    )
endif( $ENV{BUILDENV} MATCHES "Release")

if( $ENV{BUILDENV} MATCHES "Release")	
	find_library(OSG_GA_LIBRARY 
			NAMES osgGA
			PATHS $ENV{OSG_DIR}/lib/
			NO_DEFAULT_PATH    
	)
else( $ENV{BUILDENV} MATCHES "Release")
	find_library(OSG_GA_LIBRARY 
			NAMES osgGAd
			PATHS $ENV{OSG_DIR}/lib/
			NO_DEFAULT_PATH    
	)
endif( $ENV{BUILDENV} MATCHES "Release")
	
if( $ENV{BUILDENV} MATCHES "Release")
	find_library(OPENTHREADS_LIBRARY 
			NAMES OpenThreads
			PATHS $ENV{OSG_DIR}/lib/
			NO_DEFAULT_PATH    
	)
else( $ENV{BUILDENV} MATCHES "Release")
	find_library(OPENTHREADS_LIBRARY 
			NAMES OpenThreadsd
			PATHS $ENV{OSG_DIR}/lib/
			NO_DEFAULT_PATH    
	)
endif( $ENV{BUILDENV} MATCHES "Release")

if( $ENV{BUILDENV} MATCHES "Release")
	find_library(OSGDB_LIBRARY 
			NAMES osgDB
			PATHS $ENV{OSG_DIR}/lib/
			NO_DEFAULT_PATH    
	)
else( $ENV{BUILDENV} MATCHES "Release")
	find_library(OSGDB_LIBRARY 
			NAMES osgDBd
			PATHS $ENV{OSG_DIR}/lib/
			NO_DEFAULT_PATH    
	)
endif( $ENV{BUILDENV} MATCHES "Release")

if( $ENV{BUILDENV} MATCHES "Release")
	find_library(OSGTEXT_LIBRARY 
			NAMES osgText
			PATHS $ENV{OSG_DIR}/lib/
			NO_DEFAULT_PATH    
	)
else( $ENV{BUILDENV} MATCHES "Release")
	find_library(OSGTEXT_LIBRARY 
			NAMES osgTextd
			PATHS $ENV{OSG_DIR}/lib/
			NO_DEFAULT_PATH    
	)
endif( $ENV{BUILDENV} MATCHES "Release")

if( $ENV{BUILDENV} MATCHES "Release")
	find_library(OSGUTIL_LIBRARY 
			NAMES osgUtil
			PATHS $ENV{OSG_DIR}/lib/
			NO_DEFAULT_PATH    
	)
else( $ENV{BUILDENV} MATCHES "Release")
	find_library(OSGUTIL_LIBRARY 
			NAMES osgUtild
			PATHS $ENV{OSG_DIR}/lib/
			NO_DEFAULT_PATH    
	)
endif( $ENV{BUILDENV} MATCHES "Release")

SET(OSG_FOUND "NO")
IF(OSG_LIBRARY AND OSG_INCLUDE_DIR )
    SET(OSG_FOUND "YES")
ELSE(OSG_LIBRARY AND OSG_INCLUDE_DIR)
    MESSAGE("OSG Not Found")
ENDIF(OSG_LIBRARY AND OSG_INCLUDE_DIR)

if( $ENV{BUILDENV} MATCHES "Release")
	install(FILES $ENV{OSG_DIR}/osg131-osg.dll
			  $ENV{OSG_BIN}/osg131-osgViewer.dll
			  $ENV{OSG_BIN}/osg131-osgDB.dll
			  $ENV{OSG_BIN}/osg131-osgGA.dll
			  $ENV{OSG_BIN}/osg131-osgText.dll
			  $ENV{OSG_BIN}/osg131-osgUtil.dll
			  $ENV{OSG_BIN}/ot20-OpenThreads.dll
		DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
else( $ENV{BUILDENV} MATCHES "Release")
	install(FILES $ENV{OSG_BIN}/osg131-osgd.dll
			  $ENV{OSG_BIN}/osg131-osgViewerd.dll
			  $ENV{OSG_BIN}/osg131-osgDBd.dll
			  $ENV{OSG_BIN}/osg131-osgGAd.dll
			  $ENV{OSG_BIN}/osg131-osgTextd.dll
			  $ENV{OSG_BIN}/osg131-osgUtild.dll
			  $ENV{OSG_BIN}/ot20-OpenThreadsd.dll
		DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
endif( $ENV{BUILDENV} MATCHES "Release")