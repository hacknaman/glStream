include_directories(.)
include_directories(${OSG_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/TransVizUtil)
include_directories(${SERVER_CONTENT_API_INCLUDE_DIR})
#include_directories(${CatiaCppApi_INCLUDE_DIR})

set(SOURCES
	catiacomposerplayerspu_config.cpp
	catiacomposerplayerspu_init.cpp
	catiacomposerplayerspu.cpp
	catiacomposerplayer.def
	)

#TODO
#To generate a new printspu_enums.c file, do this:
#	python enums.py /usr/include/GL/gl.h ../../include/GL/glext.h ../../include/chromium.h > printspu_enums.c

add_library(catiacomposerplayerspu SHARED ${SOURCES})
target_link_libraries(catiacomposerplayerspu crutil crmothership spuload scenegraphspu)
target_link_libraries(catiacomposerplayerspu 
	debug		${OSG_LIBRARY_D} debug ${OPENTHREADS_LIBRARY_D} debug ${OSG_VIEWER_LIBRARY_D}   debug ${OSGDB_LIBRARY_D}
	optimized	${OSG_LIBRARY} optimized ${OPENTHREADS_LIBRARY} optimized ${OSG_VIEWER_LIBRARY} optimized ${OSGDB_LIBRARY}
 )
target_link_libraries(catiacomposerplayerspu ${EXTRA_LIBS})
#target_link_libraries(catiacomposerplayerspu debug ${CatiaCppApi_LIBRARY_DEBUG_DIR} optimized ${CatiaCppApi_LIBRARY_DIR})
install(TARGETS catiacomposerplayerspu
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)
	SET_TARGET_PROPERTIES(catiacomposerplayerspu  PROPERTIES FOLDER "SPU")
