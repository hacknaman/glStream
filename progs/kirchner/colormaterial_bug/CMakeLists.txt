include_directories(.)
set(SOURCES
	bug.c
)
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLUT_INCLUDE_DIR})
add_executable(kirchner_colormaterial  ${SOURCES})
target_link_libraries(kirchner_colormaterial ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
target_link_libraries(kirchner_colormaterial ${EXTRA_LIBS})
if(X11_PLATFORM)
	target_link_libraries(kirchner_colormaterial ${X11_LIBRARIES})
endif()
install(TARGETS kirchner_colormaterial
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION apps COMPONENT runtime)
SET_TARGET_PROPERTIES(kirchner_colormaterial PROPERTIES FOLDER "Programs")