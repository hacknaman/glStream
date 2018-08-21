# Usage : FIND_LIB (<LIB_VARIABLE> <LIB_NAME> <ROOT_DIR> <BUILD>)

# All 4 Arguments are mandatory 

# ARGV0 : Variable to store the path for the library (IF FOUND!) 
# ARGV1 : Name of the Libraray to FIND
# ARGV2 : Root directory of the external dependency
# ARGV3 : BUILD (debug or release)

# Description : 
#   This function takes in the <LIB_VARIABLE> <LIB_NAME> <ROOT_DIR> <BUILD> 
#   finds <LIB_NAME> in in the OSS according to the 
#     <ROOT_DIR>
#     OS Version 
#     Compiler Version 
#     Machine Architecture 
#     <BUILD>

FUNCTION (FIND_LIB)

if (${WIN32})  
  if (ARGC EQUAL 4)
    FIND_LIBRARY(     ${ARGV0}
          NAMES       ${ARGV1}
          PATHS
          ${ARGV2}/lib/${OS}/$ENV{COMPILER}/$ENV{OSSARCH}/${ARGV3}
      NO_DEFAULT_PATH
      )

    if (${ARGV0})
    else()
      message("${ARGV0} NOT FOUND")
    endif()
  
  else ()
      message("Incorrect Number of arguments passed, Please check <library>_DIR path in TerrainSDK_Common -- ${ARGV0} NOT FOUND")
  endif()
endif()

if (${UNIX})
  if (ARGC EQUAL 4)
    SET(SEARCH_PATHS ${ARGV2}/lib/$ENV{OSSARCH}/${ARGV3}
                     ${ARGV2}/lib/$ENV{OSSARCH}
                     ${ARGV2}/lib
                     ${ARGV2}
    )
    FIND_LIBRARY(     ${ARGV0}
          NAMES       ${ARGV1}
          PATHS       ${SEARCH_PATHS}
          
      NO_DEFAULT_PATH
      )

    if (${ARGV0})
    else()
      message("${ARGV0} NOT FOUND")
      #message(STATUS "Searched in: ${SEARCH_PATHS}")
    endif()
  
  else ()
      message("Incorrect Number of arguments passed, Please check <library>_DIR path in TerrainSDK_Common -- ${ARGV0} NOT FOUND")
  endif()
endif()

ENDFUNCTION()
