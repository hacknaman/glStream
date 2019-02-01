include_directories(.)
include_directories(../../TransVizUtilUE)
set(SOURCES
	UETestApp.cpp
)
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLUT_INCLUDE_DIR})
include_directories(${OSG_INCLUDE_DIR})

add_executable(UETestApp  ${SOURCES})
target_link_libraries(UETestApp ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} )

target_link_libraries(scenegraphApp 
	debug ${OPENTHREADS_LIBRARY_D} 
	optimized ${OPENTHREADS_LIBRARY} 
)

target_link_libraries(UETestApp TransVizUtilUE)
target_link_libraries(UETestApp ${EXTRA_LIBS})

if(X11_PLATFORM)
	target_link_libraries(UETestApp ${X11_LIBRARIES})
endif()
install(TARGETS UETestApp
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)
	SET_TARGET_PROPERTIES(UETestApp PROPERTIES FOLDER "Programs")
