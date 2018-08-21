# Where is this and what do we need it for?
#INCLUDE(ListHandle)


  MACRO(FILTER_OUT FILTERS INPUTS OUTPUT)
       # Mimicks Gnu Make's $(filter-out) which removes elements
       # from a list that match the pattern.
       # Arguments:
       #  FILTERS - list of patterns that need to be removed
       #  INPUTS  - list of inputs that will be worked on
       #  OUTPUT  - the filtered list to be returned
       #
       # Example:
       #  SET(MYLIST this that and the other)
       #  SET(FILTS this that)
       #
       #  FILTER_OUT("${FILTS}" "${MYLIST}" OUT)
       #  MESSAGE("OUTPUT = ${OUT}")
       #
       # The output -
       #   OUTPUT = and;the;other
       #
       SET(FOUT "")
       FOREACH(INP ${INPUTS})
           SET(FILTERED 0)
           FOREACH(FILT ${FILTERS})
               IF(${FILTERED} EQUAL 0)
                   IF("${FILT}" STREQUAL "${INP}")
                       SET(FILTERED 1)
                   ENDIF("${FILT}" STREQUAL "${INP}")
               ENDIF(${FILTERED} EQUAL 0)
           ENDFOREACH(FILT ${FILTERS})
           IF(${FILTERED} EQUAL 0)
               SET(FOUT ${FOUT} ${INP})
           ENDIF(${FILTERED} EQUAL 0)
       ENDFOREACH(INP ${INPUTS})
       SET(${OUTPUT} ${FOUT})
   ENDMACRO(FILTER_OUT FILTERS INPUTS OUTPUT)


   MACRO(GET_HEADERS_EXTENSIONLESS DIR GLOB_PATTERN OUTPUT)
            FILE(GLOB TMP "${DIR}/${GLOB_PATTERN}" )
      #FOREACH(F ${TMP})
            #    MESSAGE(STATUS "header-->${F}<--")
      #ENDFOREACH(F ${TMP})
            FILTER_OUT("${DIR}/CVS" "${TMP}" TMP)
            FILTER_OUT("${DIR}/cvs" "${TMP}" ${OUTPUT})
            FILTER_OUT("${DIR}/.svn" "${TMP}" ${OUTPUT})
   ENDMACRO(GET_HEADERS_EXTENSIONLESS DIR GLOB_PATTERN OUTPUT)

MACRO(ADD_DIRS_TO_ENV_VAR _VARNAME )
 FOREACH(_ADD_PATH ${ARGN})
    FILE(TO_NATIVE_PATH ${_ADD_PATH} _ADD_NATIVE)
    #SET(_CURR_ENV_PATH $ENV{PATH})
    #LIST(SET _CURR_ENV_PATH ${_ADD_PATH})
    #SET(ENV{PATH} ${_CURR_ENV_PATH})${_FILE}
    IF(WIN32)
        SET(ENV{${_VARNAME}} "$ENV{${_VARNAME}};${_ADD_NATIVE}")
    ELSE(WIN32)
        SET(ENV{${_VARNAME}} "$ENV{${_VARNAME}}:${_ADD_NATIVE}")
    ENDIF(WIN32)
    #MESSAGE(" env ${_VARNAME} --->$ENV{${_VARNAME}}<---")
 ENDFOREACH(_ADD_PATH)
ENDMACRO(ADD_DIRS_TO_ENV_VAR _VARNAME )

#---------------------------------------------------
# MACRO CORRECT_PATH VAR PATH
# corrects slashes in PATH to be cmake conformous ( / )
# and puts result in VAR
#---------------------------------------------------

MACRO(CORRECT_PATH VAR PATH)
    SET(${VAR} ${PATH})
IF(WIN32)
    STRING(REGEX REPLACE "/" "\\\\" ${VAR} "${PATH}")
ENDIF(WIN32)
ENDMACRO(CORRECT_PATH)

MACRO(TARGET_LOCATIONS_SET_FILE FILE)
 SET(ACCUM_FILE_TARGETS ${FILE})
 FILE(WRITE ${ACCUM_FILE_TARGETS} "")
ENDMACRO(TARGET_LOCATIONS_SET_FILE FILE)

MACRO(TARGET_LOCATIONS_ACCUM TARGET_NAME)
 IF(ACCUM_FILE_TARGETS)
  IF(EXISTS ${ACCUM_FILE_TARGETS})
    GET_TARGET_PROPERTY(_FILE_LOCATION ${TARGET_NAME} LOCATION)
    FILE(APPEND ${ACCUM_FILE_TARGETS} "${_FILE_LOCATION};")
    #SET(_TARGETS_LIST ${_TARGETS_LIST} "${_FILE_LOCATION}" CACHE INTERNAL "lista dll")
    #MESSAGE("adding target -->${TARGET_NAME}<-- file -->${_FILE_LOCATION}<-- to list -->${_TARGETS_LIST}<--")
    #SET(ACCUM_FILE_TARGETS ${ACCUM_FILE_TARGETS} ${_FILE_LOCATION})
  ENDIF(EXISTS ${ACCUM_FILE_TARGETS})
 ENDIF(ACCUM_FILE_TARGETS)
ENDMACRO(TARGET_LOCATIONS_ACCUM TARGET_NAME)

MACRO(TARGET_LOCATIONS_GET_LIST _VAR)
 IF(ACCUM_FILE_TARGETS)
  IF(EXISTS ${ACCUM_FILE_TARGETS})
      FILE(READ ${ACCUM_FILE_TARGETS} ${_VAR})
  ENDIF(EXISTS ${ACCUM_FILE_TARGETS})
 ENDIF(ACCUM_FILE_TARGETS)
ENDMACRO(TARGET_LOCATIONS_GET_LIST _VAR)


MACRO(FIND_DEPENDENCY DEPNAME INCLUDEFILE LIBRARY SEARCHPATHLIST)

MESSAGE(STATUS "searching ${DEPNAME} -->${INCLUDEFILE}<-->${LIBRARY}<-->${SEARCHPATHLIST}<--")

SET(MY_PATH_INCLUDE )
SET(MY_PATH_LIB )
SET(MY_PATH_BIN )

FOREACH( MYPATH ${SEARCHPATHLIST} )
    SET(MY_PATH_INCLUDE ${MY_PATH_INCLUDE} ${MYPATH}/include)
    SET(MY_PATH_LIB ${MY_PATH_LIB} ${MYPATH}/lib)
    SET(MY_PATH_BIN ${MY_PATH_BIN} ${MYPATH}/bin)
ENDFOREACH( MYPATH ${SEARCHPATHLIST} )

SET(MYLIBRARY "${LIBRARY}")
SEPARATE_ARGUMENTS(MYLIBRARY)

#MESSAGE( " include paths: -->${MY_PATH_INCLUDE}<--")

#MESSAGE( " ${DEPNAME}_INCLUDE_DIR --> ${${DEPNAME}_INCLUDE_DIR}<--")

FIND_PATH("${DEPNAME}_INCLUDE_DIR" ${INCLUDEFILE}
  ${MY_PATH_INCLUDE}
)
MARK_AS_ADVANCED("${DEPNAME}_INCLUDE_DIR")
#MESSAGE( " ${DEPNAME}_INCLUDE_DIR --> ${${DEPNAME}_INCLUDE_DIR}<--")

FIND_LIBRARY("${DEPNAME}_LIBRARY"
    NAMES ${MYLIBRARY}
  PATHS ${MY_PATH_LIB}
)
IF(${DEPNAME}_LIBRARY)
    GET_FILENAME_COMPONENT(MYLIBNAME ${${DEPNAME}_LIBRARY} NAME_WE)
    GET_FILENAME_COMPONENT(MYBINPATH ${${DEPNAME}_LIBRARY} PATH)
    GET_FILENAME_COMPONENT(MYBINPATH ${MYBINPATH} PATH)
    SET(MYBINPATH "${MYBINPATH}/bin")
    IF(EXISTS ${MYBINPATH})
        SET(MYFOUND 0)
        FOREACH(MYPATH ${MY_ACCUM_BINARY_DEP})
            IF(MYPATH MATCHES ${MYBINPATH})
                SET(MYFOUND 1)
                #MESSAGE("found -->${MYPATH}<-->${MYBINPATH}<--")
            ENDIF(MYPATH MATCHES ${MYBINPATH})
        ENDFOREACH(MYPATH )
        IF(MYFOUND EQUAL 0)
            SET(MY_ACCUM_BINARY_DEP ${MY_ACCUM_BINARY_DEP} ${MYBINPATH})
        ENDIF(MYFOUND EQUAL 0)
    ENDIF(EXISTS ${MYBINPATH})
    #MESSAGE("${DEPNAME}_BINDEP searching -->${MYLIBNAME}${CMAKE_SHARED_MODULE_SUFFIX}<--in-->${MY_PATH_BIN}<--")
#    FIND_FILE("${DEPNAME}_BINDEP"
#        ${MYLIBNAME}${CMAKE_SHARED_MODULE_SUFFIX}
#      PATHS ${MY_PATH_BIN}
#    )
#    FIND_LIBRARY("${DEPNAME}_BINDEP"
#        NAMES ${MYLIBRARY}
#      PATHS ${MY_PATH_BIN}
#    )
ENDIF(${DEPNAME}_LIBRARY)
MARK_AS_ADVANCED("${DEPNAME}_LIBRARY")
#MESSAGE( " ${DEPNAME}_LIBRARY --> ${${DEPNAME}_LIBRARY}<--")
IF(${DEPNAME}_INCLUDE_DIR)
  IF(${DEPNAME}_LIBRARY)
    SET( ${DEPNAME}_FOUND "YES" )
    SET( ${DEPNAME}_LIBRARIES ${${DEPNAME}_LIBRARY} )
  ENDIF(${DEPNAME}_LIBRARY)
ENDIF(${DEPNAME}_INCLUDE_DIR)
ENDMACRO(FIND_DEPENDENCY DEPNAME INCLUDEFILE LIBRARY SEARCHPATHLIST)

#SET(MACRO_MESSAGE_DEBUG TRUE)
MACRO(MACRO_MESSAGE MYTEXT)
    IF(MACRO_MESSAGE_DEBUG)
        MESSAGE("in file -->${CMAKE_CURRENT_LIST_FILE}<-- line -->${CMAKE_CURRENT_LIST_LINE}<-- message  ${MYTEXT}")
    ELSE(MACRO_MESSAGE_DEBUG)
        MESSAGE(STATUS "in file -->${CMAKE_CURRENT_LIST_FILE}<-- line -->${CMAKE_CURRENT_LIST_LINE}<-- message  ${MYTEXT}")
    ENDIF(MACRO_MESSAGE_DEBUG)
ENDMACRO(MACRO_MESSAGE MYTEXT)

MACRO(LINK_WITH_VARIABLES TRGTNAME)
    FOREACH(varname ${ARGN})
      IF (WIN32)
        IF(${varname}_RELEASE)
           IF(${varname}_DEBUG)
               TARGET_LINK_LIBRARIES(${TRGTNAME} optimized "${${varname}_RELEASE}" debug "${${varname}_DEBUG}")
           ELSE(${varname}_DEBUG)
               TARGET_LINK_LIBRARIES(${TRGTNAME} optimized "${${varname}_RELEASE}" debug "${${varname}_RELEASE}" )
           ENDIF(${varname}_DEBUG)
        ELSE(${varname}_RELEASE)
           IF(${varname}_DEBUG)
               TARGET_LINK_LIBRARIES(${TRGTNAME} optimized "${${varname}}" debug "${${varname}_DEBUG}")
           ELSE(${varname}_DEBUG)
               TARGET_LINK_LIBRARIES(${TRGTNAME} optimized "${${varname}}" debug "${${varname}}" )
           ENDIF(${varname}_DEBUG)
        ENDIF(${varname}_RELEASE)
      ELSE (WIN32)
        IF(${varname}_RELEASE)
           IF(${varname}_DEBUG)
               TARGET_LINK_LIBRARIES(${TRGTNAME} optimized "${${varname}_RELEASE}" debug "${${varname}_DEBUG}")
           ELSE(${varname}_DEBUG)
               TARGET_LINK_LIBRARIES(${TRGTNAME} optimized "${${varname}_RELEASE}" debug "${${varname}_RELEASE}" )
           ENDIF(${varname}_DEBUG)
        ELSE(${varname}_RELEASE)
           IF(${varname}_DEBUG)
               TARGET_LINK_LIBRARIES(${TRGTNAME} optimized "${${varname}}" debug "${${varname}_DEBUG}")
           ELSE(${varname}_DEBUG)
               TARGET_LINK_LIBRARIES(${TRGTNAME} optimized "${${varname}}" debug "${${varname}}" )
           ENDIF(${varname}_DEBUG)
        ENDIF(${varname}_RELEASE)
      ENDIF (WIN32)
    ENDFOREACH(varname)
ENDMACRO(LINK_WITH_VARIABLES TRGTNAME)
# --------------------------------------------------------------------------------------------------------------
# Convenience macro that generates a class that has been "programmed to interfaces"
# That means that for every item in interface_names_list, this macro will generate
# a header with boiler plate code for the interface, another header with boilerplate code
# for the implementation, and finally, a class definition cpp file. Note that if these
# files already exist, this macro WILL NOT overwrite them. 
# Consider the following example, where I add a new interface, GridNewEntity to the
# INTERFACE_LIST as shown below. The other already existing files (GridMesh, GridQuad, GridVertex, GridController)
# will not be touched/overwritten.
#
#
# Implementation details: This macro simply does some string manipulation on three
# files: InterfaceBoilerPlate.txt, InterfaceImplBoilerPlate.txt and InterfaceDefBoilerPlate.txt that is
# defined in the same folder as this CMake module. You can change the boiler plate code to suit the
# code template you need. 
# NOTE WHILE MODIFYING BOILERPLATE: CMake uses semicolons ";" as string delimiters internally and (annoyingly)
# removes them from any string it reads from a file. Hence, when changing boilerplate code, make sure to
# replace all semicolons with a placeholder @@SEMI@@ which gets replaced with semicolons during write.
#
#
# Example Usage:
#
#	---------------------------(example start)---------------------------------
#
#	#FILE: CMakeLists.txt in Core Source directory
# 	SET(LIB_NAME    ${CORE_LIBRARY})
#	SET(HEADER_PATH ${TERRAINSDK_DIR}/include/Core)
#	SET(SOURCE_PATH ${TERRAINSDK_DIR}/src/Core)
#
#	#Define all interface names here
#	SET(INTERFACE_LIST 	Sample1
#						Sample2 )
#
#	GENERATE_INTERFACES(${LIB_NAME} ${HEADER_PATH} ${SOURCE_PATH} "${INTERFACE_LIST}")
#   ADD_SOURCE_TO_LIBRARY(${LIB_NAME} ${HEADER_PATH} ${SOURCE_PATH} LIB_PUBLIC_HEADERS LIB_SOURCES "${INTERFACE_LIST}")
#
#	---------------------------(example end)----------------------------------------
#
#	The preceeding example will generate 3 new files: ISample1.h, Sample1.h and Sample1.cpp and ISample2.h, Sample2.h and Sample2.cpp
#   in the specified header and source paths
# ------------------------------------------------------------------------------------------------------------------------
MACRO(GENERATE_INTERFACES library_name header_path source_path interface_names_list)
		#MESSAGE( STATUS "Generating interfaces for " ${library_name} " library...")
		#MESSAGE( STATUS "... at the header path: " ${header_path} )
		#MESSAGE( STATUS "... at the source path: " ${source_path} )
		
		FILE(READ "${TERRAINSDK_DIR}/CMakeModules/InterfaceBoilerPlate.txt" interfaceBoilerPlateText)
		FILE(READ "${TERRAINSDK_DIR}/CMakeModules/InterfaceImplBoilerPlate.txt" implementationBoilerPlateText)
		FILE(READ "${TERRAINSDK_DIR}/CMakeModules/InterfaceDefBoilerPlate.txt" definitionBoilerPlateText)
		FILE(READ "${TERRAINSDK_DIR}/CMakeModules/ExportHeaderBoilerPlate.txt" exportBoilerPlateText)
		
		SET(export_header_path "${header_path}/export.h")
			IF(NOT EXISTS ${export_header_path})
				MESSAGE( STATUS "Generating export header file for module: " ${library_name} )
				SET(export_code "" )	
				SET(library_name_in_caps "" )
				STRING(TOUPPER ${library_name} library_name_in_caps)
				STRING(REPLACE "@@LIBNAMEINCAPS@@" ${library_name_in_caps} export_code ${exportBoilerPlateText})
				STRING(REPLACE "@@LIBNAME@@" ${library_name} export_code ${export_code})
				STRING(REPLACE "@@SEMI@@" "\;" export_code ${export_code}) #Hack to get around CMake using ";" as delimiters and hence removing them
				file(WRITE "${export_header_path}" ${export_code})			
			ELSE()
			   #MESSAGE( STATUS "NOT Generating export header for: " ${library_name} " as it already exists.")
			ENDIF()
		
		FOREACH(interface ${interface_names_list})
			#We always first check if a file already exists with the specified macro name
			#If it does, we should not regenerate it
			
			SET(interface_header_path "${header_path}/I${interface}.h")
			IF(NOT EXISTS ${interface_header_path})
				MESSAGE( STATUS "Generating interface for: " ${interface} )
				SET(interface_name_in_caps "" )
				STRING(TOUPPER ${interface} interface_name_in_caps)
				SET(library_name_in_caps "" )
				STRING(TOUPPER ${library_name} library_name_in_caps)
				SET(interface_code "" )		
				STRING(REPLACE "@@INTERFACENAMEINCAPS@@" ${interface_name_in_caps} interface_code ${interfaceBoilerPlateText})
				STRING(REPLACE "@@LIBNAMEINCAPS@@" ${library_name_in_caps} interface_code ${interface_code})
				STRING(REPLACE "@@INTERFACENAME@@" ${interface} interface_code ${interface_code})
				STRING(REPLACE "@@LIBNAME@@" ${library_name} interface_code ${interface_code})
				STRING(REPLACE "@@SEMI@@" "\;" interface_code ${interface_code}) #Hack to get around CMake using ";" as delimiters and hence removing them
				file(WRITE "${interface_header_path}" ${interface_code})			
			ELSE()
			   #MESSAGE( STATUS "NOT Generating interface for: " ${interface} " as it already exists.")
			ENDIF()
			
			SET(implementation_header_path "${header_path}/${interface}.h")
			IF(NOT EXISTS ${implementation_header_path})
				MESSAGE( STATUS "Generating interface for: " ${interface} )
				SET(interface_name_in_caps "" )
				STRING(TOUPPER ${interface} interface_name_in_caps)
				SET(library_name_in_caps "" )
				STRING(TOUPPER ${library_name} library_name_in_caps)
				SET(implementation_code "" )		
				STRING(REPLACE "@@INTERFACENAMEINCAPS@@" ${interface_name_in_caps} implementation_code ${implementationBoilerPlateText})
				STRING(REPLACE "@@LIBNAMEINCAPS@@" ${library_name_in_caps} implementation_code ${implementation_code})
				STRING(REPLACE "@@INTERFACENAME@@" ${interface} implementation_code ${implementation_code})
				STRING(REPLACE "@@LIBNAME@@" ${library_name} implementation_code ${implementation_code})
				STRING(REPLACE "@@SEMI@@" "\;" implementation_code ${implementation_code}) #Hack to get around CMake using ";" as delimiters and hence removing them
				file(WRITE "${implementation_header_path}" ${implementation_code})			
			ELSE()
			   #MESSAGE( STATUS "NOT Generating implementation for: " ${interface} " as it already exists.")
			ENDIF()

			SET(definition_header_path "${source_path}/${interface}.cpp")	
			IF(NOT EXISTS ${definition_header_path})
				MESSAGE( STATUS "Generating definition for: " ${interface} )
				SET(interface_name_in_caps "" )
				STRING(TOUPPER ${interface} interface_name_in_caps)
				SET(library_name_in_caps "" )
				STRING(TOUPPER ${library_name} library_name_in_caps)
				SET(definition_code "" )
				STRING(REPLACE "@@INTERFACENAMEINCAPS@@" ${interface_name_in_caps} definition_code ${definitionBoilerPlateText})
				STRING(REPLACE "@@LIBNAMEINCAPS@@" ${library_name_in_caps} definition_code ${definition_code})
				STRING(REPLACE "@@INTERFACENAME@@" ${interface} definition_code ${definition_code})
				STRING(REPLACE "@@LIBNAME@@" ${library_name} definition_code ${definition_code})
				STRING(REPLACE "@@SEMI@@" "\;" definition_code ${definition_code}) #Hack to get around CMake using ";" as delimiters and hence removing them
				file(WRITE "${definition_header_path}" ${definition_code})			
			ELSE()
			   #MESSAGE( STATUS "NOT Generating definition for: " ${interface} " as it already exists.")
			ENDIF()
			
		ENDFOREACH(interface)
		
ENDMACRO(GENERATE_INTERFACES)

# -------------------------------------------------------------------------------------------------------
# Convenience macro that automatically globs all files (interface, implementation and definition)
# and provides you with a consolided output_lib_public_headers list and an output_lib_sources list
# which you can then add to your library. Ideally, its meant to be used along with the GENERATE_INTERFACES
# macro described above.
# 
# NOTE: It also adds the plugin.h, plugin.cpp and export.h files for this module
# into the library. It assumes these files already exist.
#
# Example Usage:
#
#	---------------------------(example start)---------------------------------
#
#	#FILE: CMakeLists.txt in VDGrid Source directory
# 	SET(LIB_NAME    ${VD_GRID_LIBRARY})
#	SET(HEADER_PATH ${TERRAINSDK_DIR}/include/VDGrid)
#	SET(SOURCE_PATH ${TERRAINSDK_DIR}/src/VDGrid)
#
#	#Define all interface names here
#	SET(INTERFACE_LIST 	GridMesh
#						GridQuad 
#						GridVertex 
#						GridController
#						GridTest)
#
#	GENERATE_INTERFACES(${LIB_NAME} ${HEADER_PATH} ${SOURCE_PATH} "${INTERFACE_LIST}")
# 	ADD_SOURCE_TO_LIBRARY(${LIB_NAME} ${HEADER_PATH} ${SOURCE_PATH} LIB_PUBLIC_HEADERS LIB_SOURCES "${INTERFACE_LIST}")
#
#	ADD_LIBRARY(${LIB_NAME} SHARED
#    ${LIB_PUBLIC_HEADERS}
#    ${LIB_SOURCES}
#	)
#	---------------------------(example end)----------------------------------------
# -----------------------------------------------------------------------------------------------------------------
MACRO(ADD_SOURCE_TO_LIBRARY library_name header_path source_path ouput_lib_public_headers ouput_lib_sources interface_list)
	SET(interface_header_names_list)
	SET(implementation_header_names_list)
	SET(definition_header_names_list)

	FOREACH(interface ${interface_list})
		SET(interface_header_path "${header_path}/I${interface}.h")
		IF(EXISTS ${interface_header_path})
			#MESSAGE(STATUS "Adding ${interface_header_path} to header list")
			LIST(APPEND interface_header_names_list ${interface_header_path})				
		ELSE()
			MESSAGE( STATUS "WARNING: Cannot find I${interface}.h")
		ENDIF()
		
		SET(implementation_header_path "${header_path}/${interface}.h")
		IF(EXISTS ${implementation_header_path})
			#MESSAGE(STATUS "Adding ${implementation_header_path} to header list")
			LIST(APPEND implementation_header_names_list ${implementation_header_path})				
		ELSE()
			MESSAGE( STATUS "WARNING: Cannot find ${interface}.h")
		ENDIF()
		
		SET(definition_header_path "${source_path}/${interface}.cpp")	
		IF(EXISTS ${definition_header_path})
			#MESSAGE(STATUS "Adding ${definition_header_path} to source list")
			LIST(APPEND definition_header_names_list ${definition_header_path})			
		ELSE()
			MESSAGE( STATUS "WARNING: Cannot find ${interface}.cpp")
		ENDIF()
		
	ENDFOREACH(interface)
	
	FILE(GLOB lib_headers ${implementation_header_names_list})        
	FILE(GLOB lib_interfaces ${interface_header_names_list})
	FILE(GLOB lib_misc_headers ${header_path}/export.h ${header_path}/${library_name}Plugin.h)
	FILE(GLOB lib_sources ${definition_header_names_list})
	FILE(GLOB lib_misc_sources ${source_path}/${library_name}Plugin.cpp)
	
	#QT_WRAP_CPP(${library_name} lib_moc_src ${lib_headers})

	source_group("Header Files\\Elements"     FILES ${lib_headers})
	source_group("Header Files\\Interfaces"         FILES ${lib_interfaces})
	source_group("Source Files\\Elements"               FILES ${lib_sources})
	source_group("Source Files\\QtMoc"				FILES ${lib_moc_src})
	source_group("Header Files\\Misc"               FILES ${lib_misc_headers})
	source_group("Source Files\\Misc"               FILES ${lib_misc_sources})

	SET(${ouput_lib_public_headers}
		${lib_headers}
		${lib_interfaces}
		${lib_misc_headers}
	) 

	SET(${ouput_lib_sources}
		${lib_sources}
		${lib_moc_src}
		${lib_misc_sources}
	)
ENDMACRO(ADD_SOURCE_TO_LIBRARY)

MACRO(SETUP_PLUGIN PLUGIN_NAME)
    SET_TARGET_PROPERTIES(${PLUGIN_NAME} PROPERTIES FOLDER "Plugins")
ENDMACRO(SETUP_PLUGIN)

MACRO(SETUP_SAMPLE SAMPLE_NAME)
    SET_TARGET_PROPERTIES(${SAMPLE_NAME} PROPERTIES FOLDER "Samples")
ENDMACRO(SETUP_SAMPLE)

MACRO(SETUP_TEST TEST_NAME)
    SET_TARGET_PROPERTIES(${TEST_NAME} PROPERTIES FOLDER "Tests")
ENDMACRO(SETUP_TEST)

# -------------------------------------------------------------------------------------------------------
# Small custom macro to add a subdirectory as target
# while at the same time defining the target name as <libnameincaps>_LIBRARY
# It also adds a definition for the include directory needed to access the headers
# corresponding to that directory as <librarynameassubfolder>/include
# -------------------------------------------------------------------------------------------------------
macro(add_library_subdirectory TARGETNAME)
    string(TOUPPER ${TARGETNAME} TARGETNAME_IN_CAPS)
    set(${TARGETNAME_IN_CAPS}_LIBRARY ${TARGETNAME})
    set(${TARGETNAME_IN_CAPS}_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libraries/${TARGETNAME}/include
                                          ${CMAKE_CURRENT_SOURCE_DIR}/libraries/${TARGETNAME}
                                          ${CMAKE_CURRENT_SOURCE_DIR}/libraries
    )
    add_subdirectory(libraries/${TARGETNAME})
endmacro(add_library_subdirectory TARGETNAME)
