include_directories(.)
set(SOURCES
	../plyfile.c
	bbox.c
)
add_executable(plybbox  ${SOURCES})
target_link_libraries(plybbox crutil)
target_link_libraries(plybbox ${EXTRA_LIBS})
install(TARGETS plybbox
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)
	SET_TARGET_PROPERTIES(plybox PROPERTIES FOLDER "Programs")
