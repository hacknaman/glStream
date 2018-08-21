# This module defines
# OSG_FOUND, if false, do not try to link to gdal
# OSG_INCLUDE_DIR , where to find the headers
#
# Created by Naman

FIND_PATH(OSG_INCLUDE_DIR  osg/Node  
		${EXTERNAL_LIBRARY_PATH}/osg-3.4/include
		NO_DEFAULT_PATH    
	)	
	
find_library(OSG_LIBRARY 
			NAMES osg
			PATHS ${EXTERNAL_LIBRARY_PATH}/osg-3.4/lib/x64
			NO_DEFAULT_PATH    
    )
	
find_library(OSG_VIEWER_LIBRARY 
			NAMES osgViewer
			PATHS ${EXTERNAL_LIBRARY_PATH}/osg-3.4/lib/x64
			NO_DEFAULT_PATH    
    )
	
find_library(OSG_GA_LIBRARY 
			NAMES osgGA
			PATHS ${EXTERNAL_LIBRARY_PATH}/osg-3.4/lib/x64
			NO_DEFAULT_PATH    
)

find_library(OPENTHREADS_LIBRARY 
			NAMES OpenThreads
			PATHS ${EXTERNAL_LIBRARY_PATH}/osg-3.4/lib/x64
			NO_DEFAULT_PATH    
)

SET(GLUT_FOUND "NO")
IF(OSG_LIBRARY AND OSG_INCLUDE_DIR )
    SET(GLUT_FOUND "YES")
ELSE(OSG_LIBRARY AND OSG_INCLUDE_DIR)
    MESSAGE("GLUT Not Found")
ENDIF(OSG_LIBRARY AND OSG_INCLUDE_DIR)

install(FILES ${EXTERNAL_LIBRARY_PATH}/osg-3.4/bin/x64/osg131-osg.dll
			  ${EXTERNAL_LIBRARY_PATH}/osg-3.4/bin/x64/osg131-osgViewer.dll  
		DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
