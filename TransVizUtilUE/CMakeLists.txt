include_directories(.)

SET(LIB_HEADERS
	TransVizUtilUE.h
	ueexport.h
	)
SET(LIB_SOURCES
	TransVizUtilUE.cpp
	)
	
ADD_LIBRARY(TransVizUtilUE SHARED
    ${LIB_HEADERS}
    ${LIB_SOURCES}
)

target_link_libraries(TransVizUtilUE crserverlib spuload crmothership crstate crpacker crutil crserver_crunpacker_copy)
target_link_libraries(TransVizUtilUE ${EXTRA_LIBS})
SET_TARGET_PROPERTIES(TransVizUtilUE
                    PROPERTIES DEFINE_SYMBOL TRANSVIZ_UTIL_UE_LIBRARY
                    )

install(DIRECTORY  ./  ${PROJECT_BINARY_DIR}/include/
		DESTINATION include
		COMPONENT dev
		PATTERN *.cpp EXCLUDE
		PATTERN *.txt EXCLUDE)

install(TARGETS TransVizUtilUE
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)