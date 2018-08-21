include_directories(.)

set(SOURCES
	scenegraphspu_config.cpp
	scenegraphspu_enums.cpp
	scenegraphspu_get.cpp
	scenegraphspu_init.cpp
	scenegraphspu_lighting.cpp
	scenegraphspu_lists.cpp
	scenegraphspu_matrices.cpp
	scenegraphspu_misc.cpp
	scenegraphspu_tex.cpp
	scenegraphspu.cpp
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
