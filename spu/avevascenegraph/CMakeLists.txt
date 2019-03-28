include_directories(.)
include_directories(${OSG_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/TransVizUtil)
include_directories(${SERVER_CONTENT_API_INCLUDE_DIR})
set(SOURCES
	avevascenegraphspu_config.cpp
	avevascenegraphspu_init.cpp
	avevascenegraphspu.cpp
	avevascenegraph.def
)

#TODO
#To generate a new printspu_enums.c file, do this:
#	python enums.py /usr/include/GL/gl.h ../../include/GL/glext.h ../../include/chromium.h > printspu_enums.c

add_library(avevascenegraphspu SHARED ${SOURCES})
target_link_libraries(avevascenegraphspu crutil crmothership spuload scenegraphspu) 

target_link_libraries(avevascenegraphspu	
	debug		${OSG_LIBRARY_D} debug ${OPENTHREADS_LIBRARY_D} debug ${OSG_VIEWER_LIBRARY_D} debug ${OSGDB_LIBRARY_D}
	optimized	${OSG_LIBRARY} optimized ${OPENTHREADS_LIBRARY} optimized ${OSG_VIEWER_LIBRARY}  optimized ${OSGDB_LIBRARY}
)
target_link_libraries(avevascenegraphspu ${EXTRA_LIBS})
target_link_libraries(avevascenegraphspu debug ${SERVER_CONTENT_API_LIBRARY_D} optimized ${SERVER_CONTENT_API_LIBRARY}) 

install(TARGETS avevascenegraphspu
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)
SET_TARGET_PROPERTIES(avevascenegraphspu  PROPERTIES FOLDER "SPU")