# This module defines
# GLUT_FOUND, if false, do not try to link to gdal
# GLUT_INCLUDE_DIR, where to find the headers
#
# Created by Naman

FIND_PATH(GLUT_INCLUDE_DIR GL/glut.h   
		${EXTERNAL_LIBRARY_PATH}/glut/include
		NO_DEFAULT_PATH    
	)	
	
FIND_LIBRARY(GLUT_glut_LIBRARY 
			NAMES freeglut
			PATHS ${EXTERNAL_LIBRARY_PATH}/glut/lib/x64
			NO_DEFAULT_PATH    
    )

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

install(FILES ${EXTERNAL_LIBRARY_PATH}/glut/bin/x64/freeglut.dll
		DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
