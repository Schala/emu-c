#ifndef _Z80_BUS_H
#define _Z80_BUS_H

#include <stdint.h>
#include <string.h>

#define DF_OWNS_RAM 1

// Bus device tree node
typedef struct _Z80_DEV
{
	uint8_t flags; // DF_*
	uint16_t ram_offset; // start offset in RAM on bus
	uint16_t ram_size; // how much RAM does the device use?
	void *data; // Actual device pointer
	struct _Z80_DEV *next;
} Z80_DEV;

// The bus is responsible for making available data to various devices
typedef struct _Z80_BUS
{
	uint16_t ram_size;
	uint8_t *ram;
	Z80_DEV *dev_list;
} Z80_BUS;

// Allocate a new bus with the specified RAM amount
Z80_BUS * z80bus_alloc(uint16_t);

// Add a device to a bus mapped to an offset in RAM and how much RAM used
Z80_DEV * z80bus_add_device(Z80_BUS *, void *, uint16_t, uint16_t, uint8_t);

// Get a device from bus's device tree at given index
void * z80bus_device(Z80_BUS *, size_t);

// Deallocate bus
void z80bus_free(Z80_BUS *);

// Remove device from bus device tree
void z80bus_free_device(Z80_BUS *, void *);

// Loads into RAM
static inline uint8_t * z80bus_load(Z80_BUS *bus, const uint8_t *data, size_t length, uint16_t addr)
{
	return (uint8_t *)memcpy(&bus->ram[addr], data, length);
}

// Print RAM to stdout
void z80bus_print_ram(const Z80_BUS *);

// Dump bus RAM to file with iteration appended to filename
int z80bus_ram_dump(const Z80_BUS *, size_t);

// Read byte from RAM address
static inline uint8_t z80bus_read(const Z80_BUS *bus, uint16_t addr)
{
	return bus->ram[addr];
}

// Read address from RAM address
static inline uint16_t z80bus_read_addr(const Z80_BUS *bus, uint16_t addr)
{
	return z80bus_read(bus, addr) | (z80bus_read(bus, addr + 1) << 8);
}

// Sets the reset vector in RAM
static inline uint8_t * z80bus_reset_vec(Z80_BUS *bus, uint16_t addr)
{
	return (uint8_t *)memcpy(&bus->ram[65532], &addr, 2);
}

// Write byte to RAM address
static inline void z80bus_write(Z80_BUS *bus, uint16_t addr, uint8_t data)
{
	bus->ram[addr] = data;
}

#endif // _Z80_BUS_H
