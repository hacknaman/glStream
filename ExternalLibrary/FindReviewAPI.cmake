# This module defines
# REVIEW_FOUND, if false, do not try to link to OSG
# REVIEW_INCLUDE_DIR , where to find the headers
#
# Created by Naman

FIND_PATH(REVIEW_INCLUDE_DIR  ReviewCpp.h  
		$ENV{REVIEW_DIR}/include
		NO_DEFAULT_PATH    
)

# only release library
find_library(REVIEW_LIBRARY 
		NAMES ReviewCppApi
		PATHS $ENV{REVIEW_DIR}/lib/
		NO_DEFAULT_PATH    
)


SET(REVIEW_FOUND "NO")
IF(REVIEW_LIBRARY AND REVIEW_INCLUDE_DIR )
    SET(REVIEW_FOUND "YES")
ELSE(REVIEW_LIBRARY AND REVIEW_INCLUDE_DIR)
    MESSAGE("REVIEW Not Found")
ENDIF(REVIEW_LIBRARY AND REVIEW_INCLUDE_DIR)
