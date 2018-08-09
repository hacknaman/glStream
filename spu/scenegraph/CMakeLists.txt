include_directories(.)

set(SOURCES
	scenegraphspu_config.c
	scenegraphspu_enums.c
	scenegraphspu_get.c
	scenegraphspu_init.c
	scenegraphspu_lighting.c
	scenegraphspu_lists.c
	scenegraphspu_matrices.c
	scenegraphspu_misc.c
	scenegraphspu_tex.c
	scenegraphspu.c
	scenegraph.def
)
#generate_with_python(print.py printspu.c printspu_unimplemented_special printspu_special ${APIFILES})

#TODO
#To generate a new printspu_enums.c file, do this:
#	python enums.py /usr/include/GL/gl.h ../../include/GL/glext.h ../../include/chromium.h > printspu_enums.c

add_library(scenegraphspu SHARED ${SOURCES})
target_link_libraries(scenegraphspu crutil crmothership spuload)
target_link_libraries(scenegraphspu ${EXTRA_LIBS})
install(TARGETS scenegraphspu
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)
