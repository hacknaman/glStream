# This module defines
# GLUT_FOUND, if false, do not try to link to gdal
# GLUT_INCLUDE_DIR, where to find the headers
#
# Created by Naman

MESSAGE(${EXTERNAL_LIBRARY_PATH}/glut/include)
MESSAGE(${EXTERNAL_LIBRARY_PATH}/glut/lib/x64)

FIND_PATH(GLUT_INCLUDE_DIR GL/glut.h   
		${EXTERNAL_LIBRARY_PATH}/glut/include
		NO_DEFAULT_PATH    
	)	
	
find_library(GLUT_glut_LIBRARY 
			NAMES freeglut
			${EXTERNAL_LIBRARY_PATH}/glut/lib/x64
			NO_DEFAULT_PATH    
    )
	
MESSAGE(${GLUT_glut_LIBRARY})
MESSAGE(${GLUT_INCLUDE_DIR})

SET(GLUT_FOUND "NO")
IF(GLUT_glut_LIBRARY AND GLUT_INCLUDE_DIR)
    SET(GLUT_FOUND "YES")
ELSE(GLUT_glut_LIBRARY AND GLUT_INCLUDE_DIR)
     MESSAGE("GLUT Not Found")
ENDIF(GLUT_glut_LIBRARY AND GLUT_INCLUDE_DIR)
