#ifndef _6500_BUS_H
#define _6500_BUS_H

#include <stdint.h>
#include <string.h>

#include "disasm.h"

#define DF_OWNS_RAM 1

// Bus device tree node
typedef struct _DEV_6500
{
	uint8_t flags; // DF_* in internal.h
	uint16_t ram_offset; // start offset in RAM on bus
	uint16_t ram_size; // how much RAM does the device use?
	void *data; // Actual device pointer
	struct _DEV_6500 *next;
} DEV_6500;

// The bus is responsible for making available data to various devices
typedef struct _BUS_6500
{
	uint16_t ram_size;
	uint8_t *ram;
	DEV_6500 *dev_list;
} BUS_6500;

// Allocate a new bus with the specified RAM amount
BUS_6500 * bus6500_alloc(uint16_t);

// Add a device to a bus mapped to an offset in RAM and how much RAM used
DEV_6500 * bus6500_add_device(BUS_6500 *, void *, uint16_t, uint16_t, uint8_t);

// Get a device from bus's device tree at given index
void * bus6500_device(BUS_6500 *, size_t);

// Disassemble from the specified address for the specified length
DISASM_6500 * bus6500_disasm(BUS_6500 *, uint16_t, uint16_t);

// Deallocate bus
void bus6500_free(BUS_6500 *);

// Remove device from bus device tree
void bus6500_free_device(BUS_6500 *, void *);

// Loads into RAM
static inline uint8_t * bus6500_load(BUS_6500 *bus, const uint8_t *data, size_t length, uint16_t addr)
{
	return (uint8_t *)memcpy(&bus->ram[addr], data, length);
}

// Print RAM to stdout
void bus6500_print_ram(const BUS_6500 *);

// Dump bus RAM to file with iteration appended to filename
int bus6500_ram_dump(const BUS_6500 *, size_t);

// Read byte from RAM address
static inline uint8_t bus6500_read(const BUS_6500 *bus, uint16_t addr)
{
	return bus->ram[addr];
}

// Read address from RAM address
static inline uint16_t bus6500_read_addr(const BUS_6500 *bus, uint16_t addr)
{
	return bus6500_read(bus, addr) | (bus6500_read(bus, addr + 1) << 8);
}

// Sets the reset vector in RAM
static inline uint8_t * bus6500_reset_vec(BUS_6500 *bus, uint16_t addr)
{
	return (uint8_t *)memcpy(&bus->ram[65532], &addr, 2);
}

// Write byte to RAM address
static inline void bus6500_write(BUS_6500 *bus, uint16_t addr, uint8_t data)
{
	bus->ram[addr] = data;
}

#endif // _6500_BUS_H
