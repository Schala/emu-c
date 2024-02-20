set (COMMON_SOURCES
	macros.h
	util.c
	util.h
)

add_library (common ${COMMON_SOURCES})
set_target_properties (common PROPERTIES LINKER_LANGUAGE C)
set_target_properties (common PROPERTIES POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS})
set_property (TARGET common PROPERTY C_STANDARD 17)
