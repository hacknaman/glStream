include_directories(.)
set(SOURCES
	threadtest.c
)
add_executable(threadtest  ${SOURCES})
target_link_libraries(threadtest crutil crfaker crmothership spuload)
target_link_libraries(threadtest ${EXTRA_LIBS})
install(TARGETS threadtest
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION apps COMPONENT runtime)
	SET_TARGET_PROPERTIES(threadtest PROPERTIES FOLDER "Programs")
