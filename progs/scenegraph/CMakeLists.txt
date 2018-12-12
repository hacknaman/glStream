include_directories(.)
include_directories(../../TransVizUtil)
set(SOURCES
	scenegraphApp.cpp
)

include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLUT_INCLUDE_DIR})
include_directories(${OSG_INCLUDE_DIR})
add_executable(scenegraphApp  ${SOURCES})
target_link_libraries(scenegraphApp ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} )

target_link_libraries(scenegraphApp 
	debug		${OSG_GA_LIBRARY_D} debug ${OSG_LIBRARY_D} debug ${OSG_VIEWER_LIBRARY_D} debug ${OSGDB_LIBRARY_D} debug ${OPENTHREADS_LIBRARY_D} debug ${OSGUTIL_LIBRARY_D}
	optimized	${OSG_GA_LIBRARY} optimized ${OSG_LIBRARY} optimized ${OSG_VIEWER_LIBRARY} optimized ${OSGDB_LIBRARY} optimized ${OPENTHREADS_LIBRARY} optimized ${OSGUTIL_LIBRARY}
)

target_link_libraries(scenegraphApp TransVizUtil)
target_link_libraries(scenegraphApp ${EXTRA_LIBS})
if(X11_PLATFORM)
	target_link_libraries(scenegraphApp ${X11_LIBRARIES})
endif()
install(TARGETS scenegraphApp
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)
	SET_TARGET_PROPERTIES(scenegraphApp PROPERTIES FOLDER "Programs")
