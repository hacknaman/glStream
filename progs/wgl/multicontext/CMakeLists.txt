include_directories(.)
set(SOURCES
	multicontext.c
)
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLUT_INCLUDE_DIR})
add_executable(multicontext WIN32 ${SOURCES})
target_link_libraries(multicontext crextensionscommon ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
target_link_libraries(multicontext ${EXTRA_LIBS})
install(TARGETS multicontext
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION apps COMPONENT runtime)
	SET_TARGET_PROPERTIES(multicontext PROPERTIES FOLDER "Programs")
