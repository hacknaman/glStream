# This module defines
# GLUT_FOUND, if false, do not try to link to gdal
# GLUT_INCLUDE_DIR, where to find the headers
#
# Created by Naman
FIND_PATH(GLUT_INCLUDE_DIR GL/glut.h   
		$ENV{GLUT_DIR}/include
		${GLUT_DIR}/include
		NO_DEFAULT_PATH    
	)	
	
if("$ENV{BUILD_ARCH}" STREQUAL x86)
	FIND_LIBRARY(GLUT_glut_LIBRARY 
			NAMES freeglut
			PATHS ${GLUT_DIR}/lib/
			$ENV{GLUT_DIR}/lib/
			NO_DEFAULT_PATH    
		)
else()
	FIND_LIBRARY(GLUT_glut_LIBRARY 
				NAMES freeglut
				PATHS ${GLUT_DIR}/lib/x64
				$ENV{GLUT_DIR}/lib/x64
				NO_DEFAULT_PATH    
		)
endif()
SET(GLUT_FOUND "NO")
IF(GLUT_glut_LIBRARY AND GLUT_INCLUDE_DIR)
    SET(GLUT_FOUND "YES")
ELSE(GLUT_glut_LIBRARY AND GLUT_INCLUDE_DIR)
     MESSAGE("GLUT Not Found")
ENDIF(GLUT_glut_LIBRARY AND GLUT_INCLUDE_DIR)

if (GLUT_FOUND)

  set( GLUT_LIBRARIES
    ${GLUT_glut_LIBRARY}
  )

endif()
if("$ENV{BUILD_ARCH}" STREQUAL x86)
	install(DIRECTORY $ENV{GLUT_DIR}/bin/
		DESTINATION ${CMAKE_INSTALL_PREFIX}/apps 
		PATTERN "x64" EXCLUDE)
else()
	install(DIRECTORY $ENV{GLUT_DIR}/bin/x64/
		DESTINATION ${CMAKE_INSTALL_PREFIX}/apps )
endif()


