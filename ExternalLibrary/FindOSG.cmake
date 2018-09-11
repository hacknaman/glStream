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
			NAMES osgd
			PATHS ${EXTERNAL_LIBRARY_PATH}/osg-3.4/lib/x64
			NO_DEFAULT_PATH    
    )
	
find_library(OSG_VIEWER_LIBRARY 
			NAMES osgViewerd
			PATHS ${EXTERNAL_LIBRARY_PATH}/osg-3.4/lib/x64
			NO_DEFAULT_PATH    
    )
	
find_library(OSG_GA_LIBRARY 
			NAMES osgGAd
			PATHS ${EXTERNAL_LIBRARY_PATH}/osg-3.4/lib/x64
			NO_DEFAULT_PATH    
)

find_library(OPENTHREADS_LIBRARY 
			NAMES OpenThreadsd
			PATHS ${EXTERNAL_LIBRARY_PATH}/osg-3.4/lib/x64
			NO_DEFAULT_PATH    
)

find_library(OSGDB_LIBRARY 
			NAMES osgDBd
			PATHS ${EXTERNAL_LIBRARY_PATH}/osg-3.4/lib/x64
			NO_DEFAULT_PATH    
)

find_library(OSGTEXT_LIBRARY 
			NAMES osgTextd
			PATHS ${EXTERNAL_LIBRARY_PATH}/osg-3.4/lib/x64
			NO_DEFAULT_PATH    
)

find_library(OSGUTIL_LIBRARY 
			NAMES osgUtild
			PATHS ${EXTERNAL_LIBRARY_PATH}/osg-3.4/lib/x64
			NO_DEFAULT_PATH    
)

SET(OSG_FOUND "NO")
IF(OSG_LIBRARY AND OSG_INCLUDE_DIR )
    SET(OSG_FOUND "YES")
ELSE(OSG_LIBRARY AND OSG_INCLUDE_DIR)
    MESSAGE("OSG Not Found")
ENDIF(OSG_LIBRARY AND OSG_INCLUDE_DIR)

install(FILES ${EXTERNAL_LIBRARY_PATH}/osg-3.4/bin/x64/osg131-osgd.dll
			  ${EXTERNAL_LIBRARY_PATH}/osg-3.4/bin/x64/osg131-osgViewerd.dll
			  ${EXTERNAL_LIBRARY_PATH}/osg-3.4/bin/x64/osg131-osgDBd.dll
			  ${EXTERNAL_LIBRARY_PATH}/osg-3.4/bin/x64/osg131-osgGAd.dll
			  ${EXTERNAL_LIBRARY_PATH}/osg-3.4/bin/x64/osg131-osgTextd.dll
			  ${EXTERNAL_LIBRARY_PATH}/osg-3.4/bin/x64/osg131-osgUtild.dll
			  ${EXTERNAL_LIBRARY_PATH}/osg-3.4/bin/x64/ot20-OpenThreadsd.dll
		DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
