include_directories(.)
set(SOURCES
	scenegraphApp.cpp
)
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLUT_INCLUDE_DIR})
add_executable(scenegraphApp  ${SOURCES})
target_link_libraries(scenegraphApp ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
target_link_libraries(scenegraphApp ${EXTRA_LIBS})
target_link_libraries(scenegraphApp scenegraphspu crutil crmothership spuload)
if(X11_PLATFORM)
	target_link_libraries(scenegraphApp ${X11_LIBRARIES})
endif()
install(TARGETS scenegraphApp
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)
