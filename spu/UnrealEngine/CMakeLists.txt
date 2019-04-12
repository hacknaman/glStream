include_directories(.)
include_directories(${PROJECT_SOURCE_DIR}/TransVizUtilUE)

set(SOURCES
	UnrealEnginespu_config.cpp
	UnrealEnginespu_enums.cpp
	UnrealEnginespu_get.cpp
	UnrealEnginespu_init.cpp
	UnrealEnginespu_lighting.cpp
	UnrealEnginespu_lists.cpp
	UnrealEnginespu_matrices.cpp
	UnrealEnginespu_misc.cpp
	UnrealEnginespu_tex.cpp
	UnrealEnginespu.cpp
	UnrealEngine.def
)

add_library(UnrealEnginespu SHARED ${SOURCES})

target_link_libraries(UnrealEnginespu crutil crmothership spuload)
target_link_libraries(UnrealEnginespu ${EXTRA_LIBS})

install(TARGETS UnrealEnginespu
	LIBRARY DESTINATION lib COMPONENT runtime
	ARCHIVE DESTINATION lib COMPONENT dev
	RUNTIME DESTINATION bin COMPONENT runtime)
	
SET_TARGET_PROPERTIES(UnrealEnginespu  PROPERTIES FOLDER "SPU")