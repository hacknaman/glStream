include_directories(.)
set(SOURCES
	simpleapp.c
)
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLUT_INCLUDE_DIR})
add_executable(simpleapp  ${SOURCES})
target_link_libraries(simpleapp ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
target_link_libraries(simpleapp ${EXTRA_LIBS})
if(X11_PLATFORM)
	target_link_libraries(simpleapp ${X11_LIBRARIES})
endif()
install(TARGETS simpleapp
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION apps COMPONENT runtime)
	SET_TARGET_PROPERTIES(simpleapp PROPERTIES FOLDER "Programs")
