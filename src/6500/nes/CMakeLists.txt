set (NES_SOURCES
	internal.h
	mappers.c
	mappers.h
	nes.c
	nes.h
	ppu.c
	ppu.h
	rom.c
	rom.h
)

add_library (nes ${NES_SOURCES})
target_link_libraries (nes PUBLIC mos6500 common ${SDL2_LIBRARIES})
set_property (TARGET nes PROPERTY C_STANDARD 17)
set_target_properties (nes PROPERTIES LINKER_LANGUAGE C)
set_target_properties (nes PROPERTIES POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS})
add_dependencies (nes mos6500 common)
