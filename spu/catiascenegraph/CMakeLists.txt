include_directories(.)
include_directories(${OSG_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/TransVizUtil)
include_directories(${CatiaCppApi_INCLUDE_DIR})

set(SOURCES
	catiascenegraphspu_config.cpp
	catiascenegraphspu_enums.cpp
	catiascenegraphspu_get.cpp
	catiascenegraphspu_init.cpp
	catiascenegraphspu_lighting.cpp
	catiascenegraphspu_lists.cpp
	catiascenegraphspu_matrices.cpp
	catiascenegraphspu_misc.cpp
	catiascenegraphspu_tex.cpp
	catiascenegraphspu.cpp
	catiascenegraph.def
)

#TODO
#To generate a new printspu_enums.c file, do this:
#	python enums.py /usr/include/GL/gl.h ../../include/GL/glext.h ../../include/chromium.h > printspu_enums.c

add_library(catiascenegraphspu SHARED ${SOURCES})
target_link_libraries(catiascenegraphspu crutil crmothership spuload)
target_link_libraries(catiascenegraphspu 
	debug		${OSG_LIBRARY_D} debug ${OPENTHREADS_LIBRARY_D} debug ${OSG_VIEWER_LIBRARY_D}   debug ${OSGDB_LIBRARY_D}
	optimized	${OSG_LIBRARY} optimized ${OPENTHREADS_LIBRARY} optimized ${OSG_VIEWER_LIBRARY} optimized ${OSGDB_LIBRARY}
 )
target_link_libraries(catiascenegraphspu ${EXTRA_LIBS})
target_link_libraries(catiascenegraphspu debug ${CatiaCppApi_LIBRARY_DEBUG_DIR} optimized ${CatiaCppApi_LIBRARY_DIR})
install(TARGETS catiascenegraphspu
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)
	SET_TARGET_PROPERTIES(catiascenegraphspu  PROPERTIES FOLDER "SPU")
