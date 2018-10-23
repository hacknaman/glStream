include_directories(.)
set(SOURCES
	cameraspu.c
	cameraspu_config.c
	cameraspu_init.c
	cameraspu.def
)
add_library(cameraspu SHARED ${SOURCES})
target_link_libraries(cameraspu spuload crutil crmothership)
target_link_libraries(cameraspu ${EXTRA_LIBS})
install(TARGETS cameraspu
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)
	SET_TARGET_PROPERTIES( cameraspu PROPERTIES FOLDER "SPU")
