set (MOS6500_SOURCES
	bus.c
	bus.h
	cpu.c
	cpu.h
	disasm.c
	disasm.h
	isa.c
	isa.h
)

add_library (mos6500 ${MOS6500_SOURCES})
target_link_libraries (mos6500 PUBLIC common)
target_include_directories (mos6500 PUBLIC "${PROJECT_SOURCE_DIR}/src/common")
set_target_properties (mos6500 PROPERTIES LINKER_LANGUAGE C)
set_target_properties (mos6500 PROPERTIES POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS})
set_property (TARGET mos6500 PROPERTY C_STANDARD 23)
add_dependencies (mos6500 common)

add_subdirectory ("nes")
