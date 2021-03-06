include_directories(.)
include_directories(${FLEXERA_INCLUDE_DIR})

set(SOURCES
	load.c
	stub.c
	context.c
	${CMAKE_CURRENT_BINARY_DIR}/getprocaddress.c
	${CMAKE_CURRENT_BINARY_DIR}/opengl.def
	${CMAKE_CURRENT_BINARY_DIR}/NULLfuncs.c
	${CMAKE_CURRENT_BINARY_DIR}/tsfuncs.c
)

if(${ARCH} STREQUAL "Linux")
	set(EXPORTS ${ARCH}_${MACHTYPE}_exports)
elseif(WIN32)
	set(EXPORTS windows_exports)
	list(APPEND SOURCES wgl.c)
else()
	set(EXPORTS ${ARCH}_exports)
endif()

if(X11_PLATFORM)
	list(APPEND SOURCES glx.c xfont.c)
	find_package(X11 REQUIRED)
elseif(APPLE)
	list(APPEND SOURCES cgl.c)
endif()
find_package(OpenGL REQUIRED)

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	generate_with_python(${EXPORTS}_x64.py ${EXPORTS}.c ${APIFILES} entrypoints.py)
else()
	generate_with_python(${EXPORTS}.py ${EXPORTS}.c ${APIFILES} entrypoints.py)
endif()


list(APPEND SOURCES ${CMAKE_CURRENT_BINARY_DIR}/${EXPORTS}.c)

generate_with_python(tsfuncs.py tsfuncs.c ${APIFILES})
generate_with_python(NULLfuncs.py NULLfuncs.c ${APIFILES})

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	generate_with_python(defs_x64.py opengl.def ${APIFILES})
else()
	generate_with_python(defs.py opengl.def ${APIFILES})
endif()

if(WIN32)
	generate_with_python(windows_getprocaddress.py getprocaddress.c ${APIFILES})
else()
	generate_with_python(getprocaddress.py getprocaddress.c ${APIFILES})
endif()

if(C_USE_LICENSE STREQUAL "YES")
	add_library(crfaker SHARED  ${SOURCES} ${FLEXERA_TARGET_OBJ})
	target_link_libraries(crfaker ${FLEXERA_LIBRARIES})
else()
	add_library(crfaker SHARED ${SOURCES})
endif()

target_link_libraries(crfaker crmothership crutil spuload)
target_link_libraries(crfaker ${EXTRA_LIBS})

if(X11_PLATFORM)
	target_link_libraries(crfaker ${X11_LIBRARIES})
endif()
install(TARGETS crfaker
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)
target_link_libraries(crfaker ${OPENGL_LIBRARIES})

# Uncomment this to rename crfaker.dll to opengl32.dll
# set_target_properties(crfaker PROPERTIES OUTPUT_NAME "opengl32")

if(X11_PLATFORM)
	target_link_libraries(crfaker ${X11_LIBRARIES})
endif()
if(APPLE)
	target_link_libraries(crfaker -framework Carbon -compatibility_version 1.0.0 -current_version 1.0.0 -sub_library libGLU)
endif()
if(WIN32)
	target_link_libraries(crfaker user32 ws2_32 gdi32)
endif()
