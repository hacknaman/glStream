include_directories(.)
include_directories(../../TransVizUtil)
set(SOURCES
	scenegraphApp.cpp
)

include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLUT_INCLUDE_DIR})
include_directories(${OSG_INCLUDE_DIR})
add_executable(scenegraphApp  ${SOURCES})
target_link_libraries(scenegraphApp ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${OSG_GA_LIBRARY} ${OSG_LIBRARY} ${OSG_VIEWER_LIBRARY} ${OSGDB_LIBRARY} ${OPENTHREADS_LIBRARY})
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
