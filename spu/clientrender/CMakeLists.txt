include_directories(.)
set(SOURCES
	clientrenderspu.c
	clientrenderspu_config.c
	clientrenderspu_init.c
	clientrender.def
)
if(WIN32)
	list(APPEND SOURCES clientrenderspu_wgl.c)
elseif(APPLE)
	list(APPEND SOURCES clientrenderspu_agl.c)
else()
	list(APPEND SOURCES clientrenderspu_glx.c)
endif()
add_library(clientrenderspu SHARED ${SOURCES})
target_link_libraries(clientrenderspu crutil crmothership spuload)
target_link_libraries(clientrenderspu ${EXTRA_LIBS})
if(X11_PLATFORM)
	target_link_libraries(clientrenderspu ${X11_LIBRARIES})
endif()
install(TARGETS clientrenderspu
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)
	SET_TARGET_PROPERTIES( clientrenderspu PROPERTIES FOLDER "SPU")

if(WIN32)
	target_link_libraries(clientrenderspu user32 gdi32)
elseif(APPLE)
	target_link_libraries(clientrenderspu -framework Carbon -framework ApplicationServices)
else()
	#TODO X11 libraries
endif()
