include_directories(.)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
	passpackspu_bufferobject.c
	passpackspu_client.c
	passpackspu_config.c
	passpackspu_context.c
	passpackspu_getstring.c
	passpackspu_init.c
	passpackspu_misc.c
	passpackspu_net.c
	passpackspu_swapbuf.c
	passpackspu_pixel.c
	${CMAKE_CURRENT_BINARY_DIR}/passpackspu.c
	${CMAKE_CURRENT_BINARY_DIR}/passpackspu_get.c
	${CMAKE_CURRENT_BINARY_DIR}/passpackspu_flush.c
	${CMAKE_CURRENT_BINARY_DIR}/passpackspu_beginend.c
	${CMAKE_CURRENT_BINARY_DIR}/passpackspu_proto.h
	passpack.def
)

generate_with_python(passpackspu_proto.py
	passpackspu_proto.h
	passpackspu_special passpackspu_unimplemented_special
	${APIFILES})
generate_with_python(passpack.py
	passpackspu.c
	passpackspu_special passpackspu_unimplemented_special ${CMAKE_CURRENT_BINARY_DIR}/passpackspu_proto.h
	${APIFILES})
generate_with_python(passpackspu_get.py
	passpackspu_get.c
	passpackspu_special ${PROJECT_SOURCE_DIR}/crserverlib/get_sizes.py ${CMAKE_CURRENT_BINARY_DIR}/passpackspu_proto.h
	${APIFILES})
generate_with_python(passpackspu_beginend.py
	passpackspu_beginend.c
	passpackspu_vertex_special ${CMAKE_CURRENT_BINARY_DIR}/packspu_proto.h
	${APIFILES})
generate_with_python(passpackspu_flush.py
	passpackspu_flush.c
	passpackspu_flush_special
	${APIFILES})
add_library(passpackspu SHARED ${SOURCES})
target_link_libraries(passpackspu crutil spuload crmothership crstate packspu_crpacker_copy)
target_link_libraries(passpackspu ${EXTRA_LIBS})
install(TARGETS passpackspu
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)
	SET_TARGET_PROPERTIES(passpackspu  PROPERTIES FOLDER "SPU")
