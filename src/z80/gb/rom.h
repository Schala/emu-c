#ifndef _GB_ROM_H
#define _GB_ROM_H

#include <stdint.h>

typedef int (*GB_MAPPER)(GB_ROM *, uint16_t, uint32_t *);

// ROM file header
typedef struct _GB_ROM_HEADER
{
	uint8_t entry[4];
	uint8_t logo[48];
	char title[16];
	char manufacturer[4]; // ignored
	uint8_t cgb_attr;
	uint8_t licensee[2];
	uint8_t sgb_attr;
	uint8_t type;
	uint8_t rom_size;
	uint8_t ram_size;
	uint8_t is_not_jp;
	uint8_t old_licensee;
	uint8_t version;
	uint8_t header_checksum;
	uint8_t global_checksum;
} GB_ROM_HEADER;

// GB ROM (cartridge)
struct _GB_ROM
{
	Z80_DEV *cpu_node; // program
	GB_ROM_HEADER header;
};

// Validate ROM and load into RAM, returning a new ROM pointer
GB_ROM * gb_rom_alloc(GB *, const char *);

// Deallocate ROM
void gb_rom_free(GB *);

void gb_print_rom_info(const GB_ROM *);

// ROM read from CPU
uint8_t gb_rom_read_cpu(GB *, uint16_t);

// ROM write to CPU
void gb_rom_write_cpu(GB *, uint16_t, uint8_t);

#endif // _GB_ROM_H
