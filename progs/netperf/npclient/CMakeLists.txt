include_directories(.)
set(SOURCES
	npclient.c
)
add_executable(npclient  ${SOURCES})
target_link_libraries(npclient crmothership crutil)
target_link_libraries(npclient ${EXTRA_LIBS})
install(TARGETS npclient
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION apps COMPONENT runtime)
SET_TARGET_PROPERTIES(npclient PROPERTIES FOLDER "Programs")