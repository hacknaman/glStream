cmake_minimum_required(VERSION 2.8)
project(TransViz)
SET(EXTERNAL_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ExternalLibrary")
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ExternalLibrary;${CMAKE_MODULE_PATH}")
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/build")

find_package(PythonInterp REQUIRED)
find_package(Threads)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(OSG REQUIRED)

set(EXTRA_LIBS)
if(CMAKE_THREAD_LIBS_INIT)
	list(APPEND EXTRA_LIBS ${CMAKE_THREAD_LIBS_INIT})
	set(CHROMIUM_THREADSAFE ON)
else()
	set(CHROMIUM_THREADSAFE OFF)
endif()
if(WIN32)
	set(CHROMIUM_THREADSAFE ON)
endif()
if(CHROMIUM_THREADSAFE)
	add_definitions(-DCHROMIUM_THREADSAFE=1)
endif()

include(arch.cmake)

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/include ${PROJECT_BINARY_DIR}/include/state)
include_directories(include ${PROJECT_BINARY_DIR}/include)

include(generate.cmake)

Set(C_USE_LICENSE "YES")
add_definitions(/DUSE_LICENSE)
		
# not use license for x86 arch
if( "$ENV{BUILD_ARCH}" STREQUAL "x86" )
	Set(C_USE_LICENSE "NO")
	remove_definitions(/DUSE_LICENSE)
endif()

if( "$ENV{BUILD_PARA}" STREQUAL "UE" )
	Set(C_USE_LICENSE "NO")
	remove_definitions(/DUSE_LICENSE)
endif()

add_subdirectory(util)
add_subdirectory(unpacker)
add_subdirectory(packer)
add_subdirectory(mothership)
add_subdirectory(spu_loader)
add_subdirectory(state_tracker)
add_subdirectory(dlm)
add_subdirectory(spu)
add_subdirectory(app_faker)
add_subdirectory(opengl_stub)
add_subdirectory(crserverlib)
add_subdirectory(crserver)
add_subdirectory(crutapi)
add_subdirectory(crutclientapi)
add_subdirectory(crutproxy)
add_subdirectory(crutserver)
add_subdirectory(progs)

add_subdirectory(TransVizUtil)
add_subdirectory(TransVizUtilUE)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ ${PROJECT_BINARY_DIR}/include/
	DESTINATION include
	COMPONENT dev
	PATTERN .gitignore EXCLUDE)
