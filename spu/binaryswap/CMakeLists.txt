include_directories(.)
set(SOURCES
	binaryswapspu.c
	binaryswapspu_config.c
	binaryswapspu_init.c
	binaryswapspu.def
)
add_library(binaryswapspu SHARED ${SOURCES})
target_link_libraries(binaryswapspu spuload crutil crmothership)
target_link_libraries(binaryswapspu ${EXTRA_LIBS})
install(TARGETS binaryswapspu
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)

	SET_TARGET_PROPERTIES(binaryswapspu  PROPERTIES FOLDER "SPU")
