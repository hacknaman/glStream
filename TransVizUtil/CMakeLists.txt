include_directories(.)
include_directories(${OSG_INCLUDE_DIR})

SET(LIB_HEADERS
	TransVizUtil.h
	export.h
	)
SET(LIB_SOURCES
	TransVizUtil.cpp
	)
	
ADD_LIBRARY(TransVizUtil SHARED
    ${LIB_HEADERS}
    ${LIB_SOURCES}
)

target_link_libraries(TransVizUtil ${OSG_LIBRARY} ${OPENTHREADS_LIBRARY} ${OSG_VIEWER_LIBRARY} ${OSGDB_LIBRARY})
target_link_libraries(TransVizUtil crserverlib spuload crmothership crstate crpacker crutil crserver_crunpacker_copy)
target_link_libraries(TransVizUtil ${EXTRA_LIBS})

SET_TARGET_PROPERTIES(TransVizUtil
                    PROPERTIES DEFINE_SYMBOL TRANSVIZ_UTIL_LIBRARY
                    )
install(TARGETS TransVizUtil
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)