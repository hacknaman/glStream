# This module defines
# JPEG_LIBRARY_FOUND, if false, do not try to link to gdal
# JPEG_INCLUDE_DIR, where to find the headers
#
# Created by Naman

FIND_PATH(JPEG_INCLUDE_DIR jpeglib.h   
		${OSSROOT}/libjpeg-turbo64/include
		NO_DEFAULT_PATH    
	)	
	
FIND_LIBRARY(JPEG_LIBRARY 
		NAMES turbojpeg
		PATHS ${OSSROOT}/libjpeg-turbo64/lib
		NO_DEFAULT_PATH    
    )

SET(JPEG_LIBRARY_FOUND "NO")
IF(JPEG_LIBRARY AND JPEG_INCLUDE_DIR)
    SET(JPEG_LIBRARY_FOUND "YES")
ELSE(JPEG_LIBRARY AND JPEG_INCLUDE_DIR)
     MESSAGE("JPEG_LIBRARY Not Found")
ENDIF(JPEG_LIBRARY AND JPEG_INCLUDE_DIR)

if (JPEG_LIBRARY_FOUND)
  set( JPEG_LIBRARY
    ${JPEG_LIBRARY}
  )

endif()
