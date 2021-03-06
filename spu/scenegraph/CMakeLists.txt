include_directories(.)
include_directories(${OSG_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/TransVizUtil)

set(SOURCES
	scenegraphspu_config.cpp
	scenegraphspu_enums.cpp
	scenegraphspu_get.cpp
	scenegraphspu_init.cpp
	scenegraphspu_lighting.cpp
	scenegraphspu_lists.cpp
	scenegraphspu_matrices.cpp
	scenegraphspu_misc.cpp
	scenegraphspu_tex.cpp
	scenegraphspu.cpp
	scenegraph.def
)

#TODO
#To generate a new printspu_enums.c file, do this:
#	python enums.py /usr/include/GL/gl.h ../../include/GL/glext.h ../../include/chromium.h > printspu_enums.c

add_library(scenegraphspu SHARED ${SOURCES})
target_link_libraries(scenegraphspu crutil crmothership spuload)
target_link_libraries(scenegraphspu 
	debug		${OSG_LIBRARY_D} debug ${OPENTHREADS_LIBRARY_D} debug ${OSG_VIEWER_LIBRARY_D}   debug ${OSGDB_LIBRARY_D} debug ${OSGUTIL_LIBRARY_D} 
	optimized	${OSG_LIBRARY} optimized ${OPENTHREADS_LIBRARY} optimized ${OSG_VIEWER_LIBRARY} optimized ${OSGDB_LIBRARY} optimized ${OSGUTIL_LIBRARY}
 )
target_link_libraries(scenegraphspu ${EXTRA_LIBS})
install(TARGETS scenegraphspu
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)
	SET_TARGET_PROPERTIES(scenegraphspu  PROPERTIES FOLDER "SPU")
