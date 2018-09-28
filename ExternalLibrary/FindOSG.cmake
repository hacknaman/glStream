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
			PATHS ${EXTERNAL_LIBRARY_PATH}/osg-3.4/lib/
			NO_DEFAULT_PATH    
    )
	
find_library(OSG_VIEWER_LIBRARY 
			NAMES osgViewer
			PATHS ${EXTERNAL_LIBRARY_PATH}/osg-3.4/lib/
			NO_DEFAULT_PATH    
    )
	
find_library(OSG_GA_LIBRARY 
			NAMES osgGA
			PATHS ${EXTERNAL_LIBRARY_PATH}/osg-3.4/lib/
			NO_DEFAULT_PATH    
)

find_library(OPENTHREADS_LIBRARY 
			NAMES OpenThreads
			PATHS ${EXTERNAL_LIBRARY_PATH}/osg-3.4/lib/
			NO_DEFAULT_PATH    
)

find_library(OSGDB_LIBRARY 
			NAMES osgDB
			PATHS ${EXTERNAL_LIBRARY_PATH}/osg-3.4/lib/
			NO_DEFAULT_PATH    
)

find_library(OSGTEXT_LIBRARY 
			NAMES osgText
			PATHS ${EXTERNAL_LIBRARY_PATH}/osg-3.4/lib/
			NO_DEFAULT_PATH    
)

find_library(OSGUTIL_LIBRARY 
			NAMES osgUtil
			PATHS ${EXTERNAL_LIBRARY_PATH}/osg-3.4/lib/
			NO_DEFAULT_PATH    
)

SET(OSG_FOUND "NO")
IF(OSG_LIBRARY AND OSG_INCLUDE_DIR )
    SET(OSG_FOUND "YES")
ELSE(OSG_LIBRARY AND OSG_INCLUDE_DIR)
    MESSAGE("OSG Not Found")
ENDIF(OSG_LIBRARY AND OSG_INCLUDE_DIR)

install(FILES ${EXTERNAL_LIBRARY_PATH}/osg-3.4/bin/osg131-osg.dll
			  ${EXTERNAL_LIBRARY_PATH}/osg-3.4/bin/osg131-osgViewer.dll
			  ${EXTERNAL_LIBRARY_PATH}/osg-3.4/bin/osg131-osgDB.dll
			  ${EXTERNAL_LIBRARY_PATH}/osg-3.4/bin/osg131-osgGA.dll
			  ${EXTERNAL_LIBRARY_PATH}/osg-3.4/bin/osg131-osgText.dll
			  ${EXTERNAL_LIBRARY_PATH}/osg-3.4/bin/osg131-osgUtil.dll
			  ${EXTERNAL_LIBRARY_PATH}/osg-3.4/bin/ot20-OpenThreads.dll
		DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
