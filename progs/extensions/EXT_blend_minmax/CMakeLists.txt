include_directories(.)
set(SOURCES
	main.c
)
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLUT_INCLUDE_DIR})
add_executable(EXT_blend_minmax  ${SOURCES})
target_link_libraries(EXT_blend_minmax crutil crextensionscommon ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
target_link_libraries(EXT_blend_minmax ${EXTRA_LIBS})
if(X11_PLATFORM)
	target_link_libraries(EXT_blend_minmax ${X11_LIBRARIES})
endif()
install(TARGETS EXT_blend_minmax
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION apps COMPONENT runtime)
SET_TARGET_PROPERTIES(EXT_blend_minmax PROPERTIES FOLDER "Programs")