#ifndef _NES_ROM_H
#define _NES_ROM_H

#include <stdint.h>

#include "nes.h"

typedef int (*NES_MAPPER)(NES_ROM *, uint16_t, uint32_t *);

// ROM file header
typedef struct _NES_ROM_HEADER
{
	char magic[4]; // must be NES\x1A
	uint8_t prg_pages; // program data
	uint8_t chr_pages; // graphics data
	uint16_t mapper_info; // MI_*
	uint8_t ram_pages;
	uint8_t is_pal;
	uint8_t _reserved10[6];
} NES_ROM_HEADER;

// NES ROM (cartridge)
struct _NES_ROM
{
	uint8_t mapper_id;
	DEV_6500 *cpu_node; // program
	DEV_6500 *ppu_node; // sprite patterns
	NES_MAPPER prg_map;
	NES_MAPPER chr_map;
	NES_ROM_HEADER header;
};

// Validate ROM and load into RAM, returning a new ROM pointer
NES_ROM * nes_rom_alloc(NES *, const char *);

// Deallocate ROM
void nes_rom_free(NES *);

void nes_print_rom_info(const NES_ROM *);

// ROM read from CPU
uint8_t nes_rom_read_cpu(NES *, uint16_t);

// ROM write to CPU
void nes_rom_write_cpu(NES *, uint16_t, uint8_t);

// ROM read from PPU
uint8_t nes_rom_read_ppu(NES *, uint16_t);

// ROM write to PPU
void nes_rom_write_ppu(NES *, uint16_t, uint8_t);

#endif // _NES_ROM_H
