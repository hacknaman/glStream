include_directories(.)
set(SOURCES
	injectorspu.c
	injectorspu_config.c
	injectorspu_init.c
	injectorspu.def
)
add_library(injectorspu SHARED ${SOURCES})
add_dependencies(injectorspu injectorspu_crunpacker_copy)
target_link_libraries(injectorspu spuload crutil crmothership injectorspu_crunpacker_copy)
target_link_libraries(injectorspu ${EXTRA_LIBS})
install(TARGETS injectorspu
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)
	SET_TARGET_PROPERTIES(injectorspu  PROPERTIES FOLDER "SPU")
