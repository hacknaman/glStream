include_directories(.)
set(SOURCES
	zpixspu_config.c
	zpixspu_init.c
	zpixspu.c
	zpixspu.def
)
add_library(zpixspu SHARED ${SOURCES})
target_link_libraries(zpixspu spuload crutil crmothership)
target_link_libraries(zpixspu ${EXTRA_LIBS})
install(TARGETS zpixspu
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)
	SET_TARGET_PROPERTIES( zpixspu PROPERTIES FOLDER "SPU")
