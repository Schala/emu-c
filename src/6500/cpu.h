#pragma once

#include "disasm.h"

//! MOS 6500 state
typedef union
{
	struct
	{
		u8
			c : 1,
			z : 1,
			i : 1,
			d : 1,
			b : 1,
			u : 1,
			v : 1,
			n : 1;
	} f;
	u8 v;
} MOS6500State;


//! MOS 6500 registers
typedef struct
{
	u8 a; // accumulator
	u8 x;
	u8 y;
	MOS6500State p;
	u8 sp; // stack pointer
	u8 *pc; // program counter
} MOS6500Regs;


//! The MOS 6500 CPU processes data available via its bus
typedef struct
{
	u8 cache; // last read byte
	u8 cycles; // remaining cycles for current operation
	u8 last_op;
	u8 frame_index; // for disassembly frame
	u8 stack[256];
	u16 frame[MOS6500_FRAME_LEN]; // disassembly scope
	u16 abs;
	u16 rel;
	u16 *nmi; // offset 65530
	u16 *reset; // offset 65532
	u16 *irq; // offset 65534
	MOS6500Regs regs;
} MOS6500;

//! Allocate a new CPU
MOS6500 * mos6500Alloc();

//! Disassemble from the specified address for the specified length
MOS6500Disasm * mos6500Disasm(MOS6500 *cpu, u16 start, u16 length);

//! Deallocate CPU
void mos6500Free(MOS6500 *cpu);

//! Prints the disassembly in range of the current address
void mos6500DisasmDump(const MOS6500 *cpu);

//! Prints the CPU's register states
void mos6500RegsDump(const MOS6500 *cpu);

//! Prints the stack values
void mos6500StackDump(const MOS6500 *cpu);

//! Reset CPU state
void mos6500Reset(MOS6500 *cpu);

//! Common functionality for branch instructions
void mos6500Branch(MOS6500 *cpu);

// Read byte from stack
u8 mos6500StackRead(MOS6500 *cpu);

// Read address from stack
u16 mos6500StackReadAddr(MOS6500 *cpu);

// Write byte to stack
void mos6500StackWrite(MOS6500 *cpu, u8 data);

// Write address to stack
void mos6500StackWriteAddr(MOS6500 *cpu, u16 addr);
