#ifndef _GB_ROM_H
#define _GB_ROM_H

#include <stdint.h>

typedef int (*GB_MAPPER)(GB_ROM *, uint16_t, uint32_t *);

// ROM file header
typedef struct _GB_ROM_HEADER
{
	uint8_t entry[4];
	uint8_t logo[48];
	char title[11];
	char manufacturer[4];
	uint8_t cgb_attr;
	uint8_t licensee[2];
	uint8_t sgb_attr;
	uint8_t type;
	uint8_t rom_size;
	uint8_t ram_size;
	uint8_t dest;
	uint8_t old_licensee;
	uint8_t version;
	uint8_t header_checksum;
	uint8_t global_checksum;
} GB_ROM_HEADER;

// GB ROM (cartridge)
struct _GB_ROM
{
	uint32_t size;
	uint8_t *data;
	Z80_DEV *node;
};

// Validate ROM and load into RAM, returning a new ROM pointer
GB_ROM * gb_rom_alloc(GB *, const char *);

// Deallocate ROM
void gb_rom_free(GB *);

// Returns the header of the ROM
static inline const GB_ROM_HEADER * gb_rom_header(const GB_ROM *rom)
{
	if (!rom) return NULL;
	return (GB_ROM_HEADER *)(rom->data + 256);
}

// Get the string representation of the licensee code
const char * gb_rom_licensee_str(uint8_t);

void gb_rom_print_info(const GB_ROM *);

// ROM read from CPU
uint8_t gb_rom_read_cpu(GB *, uint16_t);

// ROM write to CPU
void gb_rom_write_cpu(GB *, uint16_t, uint8_t);

#endif // _GB_ROM_H
