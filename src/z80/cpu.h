#ifndef _6500_CPU_H
#define _6500_CPU_H

#include "../macros.h"
#include "bus.h"

// Disassembly line length
#define DISASM_STR_LEN 16

// Disassembly cache
typedef struct _DISASM_6500
{
	char lhs[DISASM_STR_LEN];
	char rhs[DISASM_STR_LEN];
	uint16_t addr;
	struct _DISASM_6500 *next;
} DISASM_6500;

// CPU registers
typedef struct _Z80_REGS
{
	union
	{
		struct
		{
			uint16_t af;
			uint16_t bc;
			uint16_t de;
			uint16_t hl;
		};
		struct
		{
			uint8_t a;
			uint8_t f;
			uint8_t b;
			uint8_t c;
			uint8_t d;
			uint8_t e;
			uint8_t h;
			uint8_t l;
		};
	};

	uint8_t i;
	uint8_t r;
	uint16_t ix;
	uint16_t iy;
	uint16_t sp;
	uint16_t pc;
} Z80_REGS;

// The CPU processes data available via its bus
struct _Z80
{
	uint8_t cycles; // remaining cycles for current operation
	uint8_t last_op;
	uint16_t last_abs_addr;
	uint16_t last_rel_addr;
	Z80_BUS *bus;
	Z80_DEV *node; // Pointer to device node in the bus's device tree
	Z80_REGS regs;
} Z80;

// Allocate a new CPU, given a parent bus, and start and end addresses in RAM
Z80 * z80_alloc(Z80_BUS *, uint16_t, uint16_t);

// CPU clock operation (execute one instruction)
void z80_clock(Z80 *);

// Disassemble from the specified address for the specified length
void z80_disasm(Z80 *, uint16_t, uint16_t);

// Deallocate disassembly cache
void z80_disasm_free(Z80 *);

// Retrieve the disassembly line at a given index
const DISASM_6500 * z80_disasm_get(const Z80 *, uint16_t);

// Fetch and cache a byte from the cached absolute address
uint8_t z80_fetch(Z80 *);

// Deallocate CPU
void z80_free(Z80 *);

// Map the start and end RAM addresses if not specified during allocation
void z80_map(Z80 *, uint16_t, uint16_t);

// Prints all disassembly in tree
void disasm6500_print(DISASM_6500 *);

// Prints the disassembly in range of the current address
void z80_print_disasm(const Z80 *, size_t);

// Prints the CPU's register states
void z80_print_regs(const Z80 *);

// Reset CPU state
void z80_reset(Z80 *);

// Generic variant of the CPU disassembly function for plain bytes
DISASM_6500 * z80_disasm(const uint8_t *, size_t);

// Deallocating disassembly tree
void z80disasm_free(DISASM_6500 *);

// Read byte from ROM
static inline uint8_t z80_read_rom(Z80 *cpu)
{
	return z80bus_read(cpu->bus, cpu->regs.c++);
}

// Read address from ROM
static inline uint16_t z80_read_rom_addr(Z80 *cpu)
{
	return (z80_read_rom(cpu) << 8) | z80_read_rom(cpu);
}

// Write byte to last used address
static inline void z80_write_last(Z80 *cpu, uint8_t data)
{
	z80bus_write(cpu->bus, cpu->last_abs_addr, data);
}

// Read address from RAM
static inline uint16_t z80_fetch_addr(const Z80 *cpu)
{
	return z80_fetch(cpu) | (z80_fetch(cpu) << 8);
}

// Read byte from stack
static inline uint8_t z80_stack_read(Z80 *cpu)
{
	return z80bus_read(cpu->bus, 256 + (++cpu->regs.s));
}

// Read address from stack
static inline uint16_t z80_stack_read_addr(Z80 *cpu)
{
	return z80_stack_read(cpu) | (z80_stack_read(cpu) << 8);
}

// Write byte to stack
static inline void z80_stack_write(Z80 *cpu, uint8_t data)
{
	z80bus_write(cpu->bus, 256 + (cpu->regs.s--), data);
}

// Write address to stack
static inline void z80_stack_write_addr(Z80 *cpu, uint16_t addr)
{
	z80_stack_write(cpu, (addr >> 8) & 255);
	z80_stack_write(cpu, addr & 255);
}

#endif // _6500_CPU_H
