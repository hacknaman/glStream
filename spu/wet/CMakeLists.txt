include_directories(.)
set(SOURCES
	wetspu.c
	wetspu_config.c
	wetspu_init.c
	wetspu.def
)
add_library(wetspu SHARED ${SOURCES})
target_link_libraries(wetspu spuload crutil crmothership)
target_link_libraries(wetspu ${EXTRA_LIBS})
install(TARGETS wetspu
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)
	SET_TARGET_PROPERTIES( wetspu PROPERTIES FOLDER "SPU")
