#This file tells where include files and .lib files of CatiaCppApi.
#created by Gaurav Sharma
#this command will set CatiaCppApi library inclde files path in variable CatiaCppApi_INCLUDE_DIR.if specified .h file found
FIND_PATH(CatiaCppApi_INCLUDE_DIR CatiaLibCPPAdapter.h 
								  $ENV{CatiaCppApi_DIR}/include 
								  ${CatiaCppApi_DIR}/include
								  NO_DEFAULT_PATH
								  
		  )
		  
#this command will set CatiaCppApi library's lib path for debug  in variable CatiaCppApi_LIBRARY_DEBUG_DIR .if specified lib file found.	  
		  
find_library(CatiaCppApi_LIBRARY_DEBUG_DIR  NAMES CatiaCppApid 
								  PATHS $ENV{CatiaCppApi_DIR}/lib/ 
								  ${CatiaCppApi_DIR}/lib
								  NO_DEFAULT_PATH
								  )
#this command will set CatiaCppApi library's lib path for release  in variable CatiaCppApi_LIBRARY_DIR. if specified lib file found.								  
find_library(CatiaCppApi_LIBRARY_DIR  NAMES CatiaCppApi 
								  PATHS $ENV{CatiaCppApi_DIR}/lib/ 
								  ${CatiaCppApi_DIR}/lib
								  NO_DEFAULT_PATH
								  )
SET(CatiaCppApi_FOUND,"NO")
IF(CatiaCppApi_LIBRARY_DIR AND CatiaCppApi_INCLUDE_DIR)
	SET(CatiaCppApi_FOUND "YES")
ELSE(CatiaCppApi_LIBRARY_DIR AND CatiaCppApi_INCLUDE_DIR)
	MESSAGE("CatiaCppApi Library Not Found")
ENDIF(CatiaCppApi_LIBRARY_DIR AND CatiaCppApi_INCLUDE_DIR)

install(DIRECTORY $ENV{CatiaCppApi_DIR}/bin/
		DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )