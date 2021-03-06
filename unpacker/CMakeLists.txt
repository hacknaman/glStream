include_directories(.)
include_directories(${CMAKE_CURRENT_BINARY_DIR})


set(SOURCES
	unpack_arrays.c
	unpack_bounds.c
	unpack_bufferobject.c
	unpack_calllists.c
	unpack_clipplane.c
	unpack_context.c
	unpack_drawpixels.c
	unpack_fence.c
	unpack_fog.c
	unpack_lights.c
	unpack_map.c
	unpack_materials.c
	unpack_matrices.c
	unpack_misc.c
	unpack_pixelmap.c
	unpack_point.c
	unpack_program.c
	unpack_readpixels.c
	unpack_regcombiner.c
	unpack_stipple.c
	unpack_texture.c
	unpack_writeback.c
	${CMAKE_CURRENT_BINARY_DIR}/unpack.c
	${CMAKE_CURRENT_BINARY_DIR}/unpack_extend.h
	unpack.def
)
generate_with_python(unpack_extend.py unpack_extend.h unpacker_special ${APIFILES})
generate_with_python(unpack.py unpack.c unpacker_special unpack_extend.h ${APIFILES})

add_library(crunpacker SHARED ${SOURCES})
target_link_libraries(crunpacker crutil spuload crpacker)
target_link_libraries(crunpacker ${EXTRA_LIBS})
install(TARGETS crunpacker
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)
add_library(crserver_crunpacker_copy SHARED ${SOURCES})
target_link_libraries(crserver_crunpacker_copy crutil spuload crpacker)
target_link_libraries(crserver_crunpacker_copy ${EXTRA_LIBS})
install(TARGETS crserver_crunpacker_copy
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)

add_library(archsplitspu_crunpacker_copy SHARED ${SOURCES})
target_link_libraries(archsplitspu_crunpacker_copy crutil spuload crpacker)
target_link_libraries(archsplitspu_crunpacker_copy ${EXTRA_LIBS})
install(TARGETS archsplitspu_crunpacker_copy
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)
add_library(hiddenlinespu_crunpacker_copy SHARED ${SOURCES})
target_link_libraries(hiddenlinespu_crunpacker_copy crutil spuload crpacker)
target_link_libraries(hiddenlinespu_crunpacker_copy ${EXTRA_LIBS})
install(TARGETS hiddenlinespu_crunpacker_copy
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)
add_library(injectorspu_crunpacker_copy SHARED ${SOURCES})
target_link_libraries(injectorspu_crunpacker_copy crutil spuload crpacker)
target_link_libraries(injectorspu_crunpacker_copy ${EXTRA_LIBS})
install(TARGETS injectorspu_crunpacker_copy
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)

SET_TARGET_PROPERTIES(crunpacker  PROPERTIES FOLDER "Unpacker")
SET_TARGET_PROPERTIES(crserver_crunpacker_copy  PROPERTIES FOLDER "Unpacker")
SET_TARGET_PROPERTIES(archsplitspu_crunpacker_copy  PROPERTIES FOLDER "Unpacker")
SET_TARGET_PROPERTIES(hiddenlinespu_crunpacker_copy  PROPERTIES FOLDER "Unpacker")
SET_TARGET_PROPERTIES(injectorspu_crunpacker_copy  PROPERTIES FOLDER "Unpacker")