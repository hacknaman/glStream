include_directories(.)
set(SOURCES
	readbackspu.c
	readbackspu_config.c
	readbackspu_init.c
	readbackspu.def
)
add_library(readbackspu SHARED ${SOURCES})
target_link_libraries(readbackspu spuload crutil crmothership)
target_link_libraries(readbackspu ${EXTRA_LIBS})
install(TARGETS readbackspu
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)
	SET_TARGET_PROPERTIES( readbackspu PROPERTIES FOLDER "SPU")
