include_directories(.)
include_directories(${OSG_INCLUDE_DIR})
include_directories(${VRTRACK_INCLUDE_DIR})

SET(LIB_HEADERS
	SceneGraphUtil.h
	export.h
	)
SET(LIB_SOURCES
	SceneGraphUtil.cpp
	)
	
ADD_LIBRARY(ScenegraphUtil SHARED
    ${LIB_HEADERS}
    ${LIB_SOURCES}
)

target_link_libraries(ScenegraphUtil ${OSG_LIBRARY} ${OPENTHREADS_LIBRARY} ${OSG_VIEWER_LIBRARY} ${VRCORE_LIBRARY})
target_link_libraries(ScenegraphUtil scenegraphspu crserverlib spuload crmothership crstate crpacker crutil crserver_crunpacker_copy)
target_link_libraries(ScenegraphUtil ${EXTRA_LIBS})

SET_TARGET_PROPERTIES(ScenegraphUtil
                    PROPERTIES DEFINE_SYMBOL SCENEGRAPH_UTIL_LIBRARY
                    )
install(TARGETS ScenegraphUtil
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)