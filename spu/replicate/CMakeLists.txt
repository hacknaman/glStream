include_directories(.)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
set(GENERATED
	replicatespu.c
	replicatespu_beginend.c
	replicatespu_proto.h
	replicatespu_diffapi.c
	replicatespu_generate.c
	replicatespu_stateapi.c
)
# TODO: generate these files!


set(SOURCES
	replicatespu_bufferobject.c
	replicatespu_client.c
	replicatespu_config.c
	replicatespu_context.c
	replicatespu_getstring.c
	replicatespu_init.c
	replicatespu_lists.c
	replicatespu_misc.c
	replicatespu_net.c
	replicatespu_redirect.c
	replicatespu_swapbuf.c
	replicatespu_pixel.c
	${CMAKE_CURRENT_BINARY_DIR}/replicatespu.c
	${CMAKE_CURRENT_BINARY_DIR}/replicatespu_beginend.c
	${CMAKE_CURRENT_BINARY_DIR}/replicatespu_proto.h
	${CMAKE_CURRENT_BINARY_DIR}/replicatespu_diffapi.c
	${CMAKE_CURRENT_BINARY_DIR}/replicatespu_generate.c
	${CMAKE_CURRENT_BINARY_DIR}/replicatespu_stateapi.c
	replicate.def
)
add_library(replicatespu SHARED ${SOURCES})
target_link_libraries(replicatespu crutil spuload crmothership crstate crdlm replicatespu_crpacker_copy)
target_link_libraries(replicatespu ${EXTRA_LIBS})
if(X11_PLATFORM)
	target_link_libraries(replicatespu ${X11_LIBRARIES})
endif()
install(TARGETS replicatespu
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)
	SET_TARGET_PROPERTIES( replicatespu PROPERTIES FOLDER "SPU")
