#ifndef _6500_CPU_H
#define _6500_CPU_H

#include "../common/macros.h"
#include "bus.h"

#define MOS6500_FRAME_LEN 16

// CPU registers
typedef struct _REGS_6500
{
	uint8_t a; // accumulator
	uint8_t x;
	uint8_t y;
	uint8_t s; // stack pointer
	uint8_t p; // P_* in internal.h
	uint16_t c; // program counter
} REGS_6500;

// The CPU processes data available via its bus
typedef struct _MOS_6500
{
	uint8_t cache; // last read byte
	uint8_t cycles; // remaining cycles for current operation
	uint8_t last_op;
	uint8_t frame_index; // for disassembly frame
	uint16_t last_abs_addr;
	uint16_t last_rel_addr;
	uint16_t frame[MOS6500_FRAME_LEN]; // disassembly scope
	const uint8_t *stack_start; // beginning of stack for diagnostics
	BUS_6500 *bus;
	DEV_6500 *node; // Pointer to device node in the bus's device tree
	REGS_6500 regs;
} MOS_6500;

// Allocate a new CPU, given a parent bus, and start and end addresses in RAM
MOS_6500 * mos6500_alloc(BUS_6500 *, uint16_t, uint16_t);

// Deallocate CPU
void mos6500_free(MOS_6500 *);

// Map the start and end RAM addresses if not specified during allocation
void mos6500_map(MOS_6500 *, uint16_t, uint16_t);

// Prints the disassembly in range of the current address
void mos6500_print_disasm(const MOS_6500 *);

// Prints the CPU's register states
void mos6500_print_regs(const MOS_6500 *);

// Prints the stack values
void mos6500_print_stack(const MOS_6500 *);

// Read byte from ROM
static inline uint8_t mos6500_read_rom(MOS_6500 *cpu)
{
	return bus6500_read(cpu->bus, cpu->regs.c++);
}

// Read address from ROM
static inline uint16_t mos6500_read_rom_addr(MOS_6500 *cpu)
{
	return (mos6500_read_rom(cpu) << 8) | mos6500_read_rom(cpu);
}

// Reset CPU state
void mos6500_reset(MOS_6500 *);

// Get a host pointer to the guest CPU's stack pointer
static inline uint8_t * mos6500_stack_ptr(MOS_6500 *cpu)
{
	return &cpu->bus->ram[256 + cpu->regs.s];
}

// Read byte from stack
static inline uint8_t mos6500_stack_read(MOS_6500 *cpu)
{
	return bus6500_read(cpu->bus, 256 + (++cpu->regs.s));
}

// Read address from stack
static inline uint16_t mos6500_stack_read_addr(MOS_6500 *cpu)
{
	return mos6500_stack_read(cpu) | (mos6500_stack_read(cpu) << 8);
}

// Write byte to stack
static inline void mos6500_stack_write(MOS_6500 *cpu, uint8_t data)
{
	bus6500_write(cpu->bus, 256 + (cpu->regs.s--), data);
}

// Write address to stack
static inline void mos6500_stack_write_addr(MOS_6500 *cpu, uint16_t addr)
{
	mos6500_stack_write(cpu, (addr >> 8) & 255);
	mos6500_stack_write(cpu, addr & 255);
}

// Write byte to last used address
static inline void mos6500_write_last(MOS_6500 *cpu, uint8_t data)
{
	bus6500_write(cpu->bus, cpu->last_abs_addr, data);
}

#endif // _6500_CPU_H
