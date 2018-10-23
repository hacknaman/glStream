include_directories(.)
set(SOURCES
	gears.c
)
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLUT_INCLUDE_DIR})
add_executable(gears  ${SOURCES})
target_link_libraries(gears ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
target_link_libraries(gears ${EXTRA_LIBS})
if(X11_PLATFORM)
	target_link_libraries(gears ${X11_LIBRARIES})
endif()
install(TARGETS gears
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)
	SET_TARGET_PROPERTIES(gears PROPERTIES FOLDER "Programs")
