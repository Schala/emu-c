#include "internal.h"
#include "isa.h"

#define _HI16(x) ((x) & 0xFF00)

// Metadata for the CPU's various operations
static const struct
{
	u8 cycles;
	const u8 (*mode)(MOS6500 *);
	const u8 (*op)(MOS6500 *);
} _opcode[] = {
	// 0x
	{ 7, &mos6500Implied, &mos6500BRK },
	{ 6, &mos6500IndirectX, &mos6500ORA },
	{ 0, &mos6500Implied, &mos6500NOP },
	{ 8, &mos6500IndirectX, &mos6500NOP },
	{ 2, &mos6500ZeroPage, &mos6500NOP },
	{ 3, &mos6500ZeroPage, &mos6500ORA },
	{ 5, &mos6500ZeroPage, &mos6500ASL },
	{ 5, &mos6500ZeroPage, &mos6500NOP },
	{ 3, &mos6500Implied, &mos6500PHP },
	{ 2, &mos6500Immediate, &mos6500ORA },
	{ 2, &mos6500Implied, &mos6500ASL },
	{ 2, &mos6500Immediate, &mos6500NOP },
	{ 4, &mos6500Absolute, &mos6500NOP },
	{ 4, &mos6500Absolute, &mos6500ORA },
	{ 6, &mos6500Absolute, &mos6500ASL },
	{ 6, &mos6500Absolute, &mos6500NOP },

	// 1x
	{ 2, &mos6500Relative, &mos6500BPL },
	{ 5, &mos6500IndirectY, &mos6500ORA },
	{ 0, &mos6500Implied, &mos6500NOP },
	{ 8, &mos6500IndirectY, &mos6500NOP },
	{ 4, &mos6500ZeroPageX, &mos6500NOP },
	{ 4, &mos6500ZeroPageX, &mos6500ORA },
	{ 6, &mos6500ZeroPageX, &mos6500ASL },
	{ 6, &mos6500ZeroPageX, &mos6500NOP },
	{ 2, &mos6500Implied, &mos6500CLC },
	{ 4, &mos6500AbsoluteY, &mos6500ORA },
	{ 2, &mos6500Implied, &mos6500NOP },
	{ 7, &mos6500AbsoluteY, &mos6500NOP },
	{ 4, &mos6500AbsoluteX, &mos6500NOP },
	{ 4, &mos6500AbsoluteX, &mos6500ORA },
	{ 7, &mos6500AbsoluteX, &mos6500ASL },
	{ 7, &mos6500AbsoluteX, &mos6500NOP },

	// 2x
	{ 6, &mos6500Absolute, &mos6500JSR },
	{ 6, &mos6500IndirectX, &mos6500AND },
	{ 0, &mos6500Implied, &mos6500NOP },
	{ 8, &mos6500IndirectX, &mos6500NOP },
	{ 3, &mos6500ZeroPage, &mos6500BIT },
	{ 3, &mos6500ZeroPage, &mos6500AND },
	{ 5, &mos6500ZeroPage, &mos6500ROL },
	{ 5, &mos6500ZeroPage, &mos6500NOP },
	{ 4, &mos6500Implied, &mos6500PLP },
	{ 2, &mos6500Immediate, &mos6500AND },
	{ 2, &mos6500Implied, &mos6500ROL },
	{ 2, &mos6500Immediate, &mos6500NOP },
	{ 4, &mos6500Absolute, &mos6500BIT },
	{ 4, &mos6500Absolute, &mos6500AND },
	{ 6, &mos6500Absolute, &mos6500ROL },
	{ 6, &mos6500Absolute, &mos6500NOP },

	// 3x
	{ 2, &mos6500Relative, &mos6500BMI },
	{ 5, &mos6500IndirectY, &mos6500AND },
	{ 0, &mos6500Implied, &mos6500NOP },
	{ 8, &mos6500IndirectY, &mos6500NOP },
	{ 4, &mos6500ZeroPageX, &mos6500NOP },
	{ 4, &mos6500ZeroPageX, &mos6500AND },
	{ 6, &mos6500ZeroPageX, &mos6500ROL },
	{ 6, &mos6500ZeroPageX, &mos6500NOP },
	{ 2, &mos6500Implied, &mos6500SEC },
	{ 4, &mos6500AbsoluteY, &mos6500AND },
	{ 2, &mos6500Implied, &mos6500NOP },
	{ 7, &mos6500AbsoluteY, &mos6500NOP },
	{ 4, &mos6500AbsoluteX, &mos6500NOP },
	{ 4, &mos6500AbsoluteX, &mos6500AND },
	{ 7, &mos6500AbsoluteX, &mos6500ROL },
	{ 7, &mos6500AbsoluteX, &mos6500NOP },

	// 4x
	{ 6, &mos6500Implied, &mos6500RTI },
	{ 6, &mos6500IndirectX, &mos6500EOR },
	{ 0, &mos6500Implied, &mos6500NOP },
	{ 8, &mos6500IndirectX, &mos6500NOP },
	{ 3, &mos6500ZeroPage, &mos6500NOP },
	{ 3, &mos6500ZeroPage, &mos6500EOR },
	{ 5, &mos6500ZeroPage, &mos6500LSR },
	{ 5, &mos6500ZeroPage, &mos6500NOP },
	{ 3, &mos6500Implied, &mos6500PHA },
	{ 2, &mos6500Immediate, &mos6500EOR },
	{ 2, &mos6500Implied, &mos6500LSR },
	{ 2, &mos6500Absolute, &mos6500NOP },
	{ 3, &mos6500Absolute, &mos6500JMP },
	{ 4, &mos6500Absolute, &mos6500EOR },
	{ 6, &mos6500Absolute, &mos6500LSR },
	{ 6, &mos6500Absolute, &mos6500NOP },

	// 5x
	{ 2, &mos6500Relative, &mos6500BVC },
	{ 5, &mos6500IndirectY, &mos6500EOR },
	{ 0, &mos6500Implied, &mos6500NOP },
	{ 8, &mos6500IndirectY, &mos6500NOP },
	{ 4, &mos6500ZeroPageX, &mos6500NOP },
	{ 4, &mos6500ZeroPageX, &mos6500EOR },
	{ 6, &mos6500ZeroPageX, &mos6500LSR },
	{ 6, &mos6500ZeroPageX, &mos6500NOP },
	{ 2, &mos6500Implied, &mos6500CLI },
	{ 4, &mos6500AbsoluteY, &mos6500EOR },
	{ 2, &mos6500Implied, &mos6500NOP },
	{ 7, &mos6500AbsoluteY, &mos6500NOP },
	{ 4, &mos6500AbsoluteX, &mos6500NOP },
	{ 4, &mos6500AbsoluteX, &mos6500EOR },
	{ 7, &mos6500AbsoluteX, &mos6500LSR },
	{ 7, &mos6500AbsoluteX, &mos6500NOP },

	// 6x
	{ 6, &mos6500Implied, &mos6500RTS },
	{ 6, &mos6500IndirectX, &mos6500ADC },
	{ 0, &mos6500Implied, &mos6500NOP },
	{ 8, &mos6500IndirectX, &mos6500NOP },
	{ 3, &mos6500ZeroPage, &mos6500NOP },
	{ 3, &mos6500ZeroPage, &mos6500ADC },
	{ 5, &mos6500ZeroPage, &mos6500ROR },
	{ 5, &mos6500ZeroPage, &mos6500NOP },
	{ 4, &mos6500Implied, &mos6500PLA },
	{ 2, &mos6500Immediate, &mos6500ADC },
	{ 2, &mos6500Implied, &mos6500ROR },
	{ 2, &mos6500Immediate, &mos6500NOP },
	{ 5, &mos6500Indirect, &mos6500JMP },
	{ 4, &mos6500Absolute, &mos6500ADC },
	{ 6, &mos6500Absolute, &mos6500ROR },
	{ 6, &mos6500Absolute, &mos6500NOP },

	// 7x
	{ 2, &mos6500Relative, &mos6500BVS },
	{ 5, &mos6500IndirectY, &mos6500ADC },
	{ 0, &mos6500Implied, &mos6500NOP },
	{ 8, &mos6500IndirectY, &mos6500NOP },
	{ 4, &mos6500ZeroPageX, &mos6500NOP },
	{ 4, &mos6500ZeroPageX, &mos6500ADC },
	{ 6, &mos6500ZeroPageX, &mos6500ROR },
	{ 6, &mos6500ZeroPageX, &mos6500NOP },
	{ 2, &mos6500Implied, &mos6500SEI },
	{ 4, &mos6500AbsoluteY, &mos6500ADC },
	{ 2, &mos6500Implied, &mos6500NOP },
	{ 7, &mos6500AbsoluteY, &mos6500NOP },
	{ 4, &mos6500AbsoluteX, &mos6500NOP },
	{ 4, &mos6500AbsoluteX, &mos6500ADC },
	{ 7, &mos6500AbsoluteX, &mos6500ROR },
	{ 7, &mos6500AbsoluteX, &mos6500NOP },

	// 8x
	{ 2, &mos6500Immediate, &mos6500NOP },
	{ 6, &mos6500IndirectX, &mos6500STA },
	{ 2, &mos6500Immediate, &mos6500NOP },
	{ 6, &mos6500IndirectX, &mos6500NOP },
	{ 3, &mos6500ZeroPage, &mos6500STY },
	{ 3, &mos6500ZeroPage, &mos6500STA },
	{ 3, &mos6500ZeroPage, &mos6500STX },
	{ 3, &mos6500ZeroPage, &mos6500NOP },
	{ 2, &mos6500Implied, &mos6500DEY },
	{ 2, &mos6500Immediate, &mos6500NOP },
	{ 2, &mos6500Implied, &mos6500TXA },
	{ 2, &mos6500Immediate, &mos6500NOP },
	{ 4, &mos6500Absolute, &mos6500STY },
	{ 4, &mos6500Absolute, &mos6500STA },
	{ 4, &mos6500Absolute, &mos6500STX },
	{ 4, &mos6500Absolute, &mos6500NOP },

	// 9x
	{ 2, &mos6500Relative, &mos6500BCC },
	{ 6, &mos6500IndirectY, &mos6500STA },
	{ 0, &mos6500Implied, &mos6500NOP },
	{ 6, &mos6500IndirectY, &mos6500NOP },
	{ 4, &mos6500ZeroPageX, &mos6500STY },
	{ 4, &mos6500ZeroPageX, &mos6500STA },
	{ 4, &mos6500ZeroPageY, &mos6500STX },
	{ 4, &mos6500ZeroPageY, &mos6500NOP },
	{ 2, &mos6500Implied, &mos6500TYA },
	{ 5, &mos6500AbsoluteY, &mos6500STA },
	{ 2, &mos6500Implied, &mos6500TXS },
	{ 5, &mos6500AbsoluteY, &mos6500NOP },
	{ 5, &mos6500AbsoluteX, &mos6500NOP },
	{ 5, &mos6500AbsoluteX, &mos6500STA },
	{ 6, &mos6500AbsoluteY, &mos6500NOP },
	{ 5, &mos6500AbsoluteY, &mos6500NOP },

	// Ax
	{ 2, &mos6500Immediate, &mos6500LDY },
	{ 6, &mos6500IndirectX, &mos6500LDA },
	{ 2, &mos6500Immediate, &mos6500LDX },
	{ 6, &mos6500IndirectX, &mos6500NOP },
	{ 3, &mos6500ZeroPage, &mos6500LDY },
	{ 3, &mos6500ZeroPage, &mos6500LDA },
	{ 3, &mos6500ZeroPage, &mos6500LDX },
	{ 3, &mos6500ZeroPage, &mos6500NOP },
	{ 2, &mos6500Implied, &mos6500TAY },
	{ 2, &mos6500Immediate, &mos6500LDA },
	{ 2, &mos6500Implied, &mos6500TAX },
	{ 2, &mos6500Immediate, &mos6500LXA },
	{ 4, &mos6500Absolute, &mos6500LDY },
	{ 4, &mos6500Absolute, &mos6500LDA },
	{ 4, &mos6500Absolute, &mos6500LDX },
	{ 4, &mos6500Absolute, &mos6500NOP },

	// Bx
	{ 2, &mos6500Relative, &mos6500BCS },
	{ 5, &mos6500IndirectY, &mos6500LDA },
	{ 0, &mos6500Implied, &mos6500NOP },
	{ 5, &mos6500IndirectY, &mos6500NOP },
	{ 4, &mos6500ZeroPageX, &mos6500LDY },
	{ 4, &mos6500ZeroPageX, &mos6500LDA },
	{ 4, &mos6500ZeroPageY, &mos6500LDX },
	{ 4, &mos6500ZeroPageY, &mos6500NOP },
	{ 2, &mos6500Implied, &mos6500CLV },
	{ 4, &mos6500AbsoluteY, &mos6500LDA },
	{ 2, &mos6500Implied, &mos6500TSX },
	{ 4, &mos6500AbsoluteY, &mos6500NOP },
	{ 4, &mos6500AbsoluteX, &mos6500LDY },
	{ 4, &mos6500AbsoluteX, &mos6500LDA },
	{ 4, &mos6500AbsoluteY, &mos6500LDX },
	{ 4, &mos6500AbsoluteY, &mos6500NOP },

	// Cx
	{ 2, &mos6500Immediate, &mos6500CPY },
	{ 6, &mos6500IndirectX, &mos6500CMP },
	{ 2, &mos6500Immediate, &mos6500NOP },
	{ 8, &mos6500IndirectX, &mos6500NOP },
	{ 3, &mos6500ZeroPage, &mos6500CPY },
	{ 3, &mos6500ZeroPage, &mos6500CMP },
	{ 5, &mos6500ZeroPage, &mos6500DEC },
	{ 5, &mos6500ZeroPage, &mos6500NOP },
	{ 2, &mos6500Implied, &mos6500INY },
	{ 2, &mos6500Immediate, &mos6500CMP },
	{ 2, &mos6500Implied, &mos6500DEX },
	{ 2, &mos6500Immediate, &mos6500NOP },
	{ 4, &mos6500Absolute, &mos6500CPY },
	{ 4, &mos6500Absolute, &mos6500CMP },
	{ 6, &mos6500Absolute, &mos6500DEC },
	{ 6, &mos6500Absolute, &mos6500NOP },

	// Dx
	{ 2, &mos6500Relative, &mos6500BNE },
	{ 5, &mos6500IndirectY, &mos6500CMP },
	{ 0, &mos6500Implied, &mos6500NOP },
	{ 8, &mos6500IndirectY, &mos6500NOP },
	{ 4, &mos6500ZeroPageX, &mos6500NOP },
	{ 4, &mos6500ZeroPageX, &mos6500CMP },
	{ 6, &mos6500ZeroPageX, &mos6500DEC },
	{ 6, &mos6500ZeroPageX, &mos6500NOP },
	{ 2, &mos6500Implied, &mos6500CLD },
	{ 4, &mos6500AbsoluteY, &mos6500CMP },
	{ 2, &mos6500Implied, &mos6500NOP },
	{ 7, &mos6500AbsoluteY, &mos6500NOP },
	{ 4, &mos6500AbsoluteX, &mos6500NOP },
	{ 4, &mos6500AbsoluteX, &mos6500CMP },
	{ 7, &mos6500AbsoluteX, &mos6500DEC },
	{ 7, &mos6500AbsoluteX, &mos6500NOP },

	// Ex
	{ 2, &mos6500Immediate, &mos6500CPX },
	{ 6, &mos6500IndirectX, &mos6500SBC },
	{ 2, &mos6500Immediate, &mos6500NOP },
	{ 8, &mos6500IndirectX, &mos6500NOP },
	{ 3, &mos6500ZeroPage, &mos6500CPX },
	{ 3, &mos6500ZeroPage, &mos6500SBC },
	{ 5, &mos6500ZeroPage, &mos6500INC },
	{ 5, &mos6500ZeroPage, &mos6500NOP },
	{ 2, &mos6500Implied, &mos6500INX },
	{ 2, &mos6500Immediate, &mos6500SBC },
	{ 2, &mos6500Implied, &mos6500NOP },
	{ 2, &mos6500Immediate, &mos6500NOP },
	{ 4, &mos6500Absolute, &mos6500CPX },
	{ 4, &mos6500Absolute, &mos6500SBC },
	{ 6, &mos6500Absolute, &mos6500INC },
	{ 6, &mos6500Absolute, &mos6500NOP },

	// Fx
	{ 2, &mos6500Relative, &mos6500BEQ },
	{ 5, &mos6500IndirectY, &mos6500SBC },
	{ 0, &mos6500Implied, &mos6500NOP },
	{ 4, &mos6500IndirectY, &mos6500NOP },
	{ 4, &mos6500ZeroPageX, &mos6500NOP },
	{ 4, &mos6500ZeroPageX, &mos6500SBC },
	{ 6, &mos6500ZeroPageX, &mos6500INC },
	{ 6, &mos6500ZeroPageX, &mos6500NOP },
	{ 2, &mos6500Implied, &mos6500SED },
	{ 4, &mos6500AbsoluteY, &mos6500SBC },
	{ 2, &mos6500Implied, &mos6500NOP },
	{ 7, &mos6500AbsoluteY, &mos6500NOP },
	{ 4, &mos6500AbsoluteX, &mos6500NOP },
	{ 4, &mos6500AbsoluteX, &mos6500SBC },
	{ 7, &mos6500AbsoluteX, &mos6500INC },
	{ 7, &mos6500AbsoluteX, &mos6500NOP }
};

void mos6500Clock(MOS6500 *cpu)
{
	if (!cpu) return;

	if (cpu->cycles == 0)
	{
		_setFlag(cpu, P_U, 1); // always set unused flag

		cpu->frame[cpu->frame_index] = cpu->regs.c;
		cpu->frame_index = cpu->frame_index-- % MOS6500_FRAME_LEN;

		// get and increment the counter
		cpu->last_op = mos6500ROMRead(cpu);

		// set cycles, see if any additional cycles are needed
		cpu->cycles = _opcode[cpu->last_op].cycles;
		u8 extra1 = _opcode[cpu->last_op].mode(cpu);
		u8 extra2 = _opcode[cpu->last_op].op(cpu);
		cpu->cycles += extra1 & extra2;

		_setFlag(cpu, P_U, 1); // always set unused flag
	}

	cpu->cycles--;
}


u8 mos6500Fetch(MOS6500 *cpu)
{
	if (!cpu) return 0;

	if (_opcode[cpu->last_op].mode != &mos6500Implied)
		cpu->cache = mos6500RAMRead(cpu->bus, mos6500ReadAddr(cpu->bus, cpu->last_abs_addr));

	return cpu->cache;
}

// Assign to accumulator, or write to bus, depending on the address mode
static inline void _checkMode(MOS6500 *cpu, u16 value)
{
	if (_opcode[cpu->last_op].mode == &mos6500Implied)
		cpu->regs.a = value;
	else
		mos6500WriteLast(cpu, value);
}

// Set negative and/or zero bits of state flags register, given a value
static inline void _setFlagsNZ(MOS6500 *cpu, u16 value)
{
	_setFlag(cpu, P_Z, value & 255 == 0);
	_setFlag(cpu, P_N, value & 128);
}

// Set carry, negative, and/or zero bits of state flags register, given a 16-bit value
static inline void _setFlagsCNZ(MOS6500 *cpu, u16 value)
{
	_setFlag(cpu, P_C, value > 255);
	_setFlagsNZ(cpu, value);
}

// Common functionality for interrupt operations
static inline void _interrupt(MOS6500 *cpu, u16 new_abs_addr, u8 new_cycles)
{
	// write the counter's current value to stack
	mos6500StackWriteAddr(cpu, cpu->regs.c);

	// set and write flags register to stack too
	_setFlag(cpu, P_B, 0);
	_setFlag(cpu, P_U, 1);
	_setFlag(cpu, P_I, 1);
	mos6500StackWrite(cpu, cpu->regs.p);

	// get a new counter
	cpu->last_abs_addr = new_abs_addr;
	cpu->regs.c = mos6500FetchAddr(cpu);

	cpu->cycles = new_cycles;
}

// address modes

u8 mos6500Absolute(MOS6500 *cpu)
{
	cpu->last_abs_addr = mos6500ROMReadAddr(cpu);
	return 0;
}

u8 mos6500AbsoluteX(MOS6500 *cpu)
{
	u16 addr = mos6500ROMReadAddr(cpu) + cpu->regs.x;
	cpu->last_abs_addr = addr;

	if (_HI16(cpu->last_abs_addr) != _HI16(addr))
		return 1;
	else
		return 0;
}

u8 mos6500AbsoluteY(MOS6500 *cpu)
{
	u16 addr = mos6500ROMReadAddr(cpu) + cpu->regs.y;
	cpu->last_abs_addr = addr;

	return _HI16(cpu->last_abs_addr) != _HI16(addr) ? 1 : 0;
}

u8 mos6500Immediate(MOS6500 *cpu)
{
	cpu->last_abs_addr = ++cpu->regs.c;
	return 0;
}

u8 mos6500Implied(MOS6500 *cpu)
{
	cpu->cache = cpu->regs.a;
	return 0;
}

u8 mos6500Indirect(MOS6500 *cpu)
{
	u16 ptr = mos6500ROMReadAddr(cpu);

	if (ptr & 255)
		// emulate page boundary hardware bug
		cpu->last_abs_addr = mos6500ReadAddr(cpu->bus, ptr);
	else
		// normal behavior
		cpu->last_abs_addr = mos6500ReadAddr(cpu->bus, ptr + 1);

	return 0;
}

u8 mos6500IndirectX(MOS6500 *cpu)
{
	cpu->last_abs_addr = mos6500ReadAddr(cpu->bus, (mos6500ROMRead(cpu) + cpu->regs.x) & 255);
	return 0;
}

u8 mos6500IndirectY(MOS6500 *cpu)
{
	u16 t = mos6500ROMRead(cpu);
	u16 lo = mos6500Read(cpu->bus, t & 255);
	u16 hi = mos6500Read(cpu->bus, (t + 1) & 255);

	cpu->last_abs_addr = (hi << 8) | lo + cpu->regs.y;

	return _HI16(cpu->last_abs_addr) != hi << 8 ? 1 : 0;
}

u8 mos6500Relative(MOS6500 *cpu)
{
	cpu->last_rel_addr = mos6500ROMRead(cpu);

	// check for signed bit
	if (cpu->last_rel_addr & 128)
		cpu->last_rel_addr |= 65280;

	return 0;
}

u8 mos6500ZeroPage(MOS6500 *cpu)
{
	cpu->last_abs_addr = mos6500ROMRead(cpu);
	return 0;
}

u8 mos6500ZeroPageX(MOS6500 *cpu)
{
	cpu->last_rel_addr = (mos6500ROMRead(cpu) + cpu->regs.x) & 255;
	return 0;
}

u8 mos6500ZeroPageY(MOS6500 *cpu)
{
	cpu->last_rel_addr = (mos6500ROMRead(cpu) + cpu->regs.y) & 255;
	return 0;
}


// _branching

u8 mos6500BCC(MOS6500 *cpu)
{
	if (!_checkFlag(cpu, P_C))
		_branch(cpu);
	return 0;
}

u8 mos6500BCS(MOS6500 *cpu)
{
	if (_checkFlag(cpu, P_C))
		_branch(cpu);
	return 0;
}

u8 mos6500BEQ(MOS6500 *cpu)
{
	if (_checkFlag(cpu, P_Z))
		_branch(cpu);
	return 0;
}

u8 mos6500BMI(MOS6500 *cpu)
{
	if (_checkFlag(cpu, P_N))
		_branch(cpu);
	return 0;
}

u8 mos6500BNE(MOS6500 *cpu)
{
	if (!_checkFlag(cpu, P_Z))
		_branch(cpu);
	return 0;
}

u8 mos6500BPL(MOS6500 *cpu)
{
	if (!_checkFlag(cpu, P_N))
		_branch(cpu);
	return 0;
}

u8 mos6500BVC(MOS6500 *cpu)
{
	if (!_checkFlag(cpu, P_V))
		_branch(cpu);
	return 0;
}

u8 mos6500BVS(MOS6500 *cpu)
{
	if (_checkFlag(cpu, P_V))
		_branch(cpu);
	return 0;
}


// state bit manipulation

u8 mos6500CLC(MOS6500 *cpu)
{
	_setFlag(cpu, P_C, 0);
	return 0;
}

u8 mos6500CLD(MOS6500 *cpu)
{
	_setFlag(cpu, P_D, 0);
	return 0;
}

u8 mos6500CLI(MOS6500 *cpu)
{
	_setFlag(cpu, P_I, 0);
	return 0;
}

u8 mos6500CLV(MOS6500 *cpu)
{
	_setFlag(cpu, P_V, 0);
	return 0;
}

u8 mos6500SEC(MOS6500 *cpu)
{
	_setFlag(cpu, P_C, 1);
	return 0;
}

u8 mos6500SED(MOS6500 *cpu)
{
	_setFlag(cpu, P_D, 1);
	return 0;
}

u8 mos6500SEI(MOS6500 *cpu)
{
	_setFlag(cpu, P_I, 1);
	return 0;
}


// _interrupts

u8 mos6500BRK(MOS6500 *cpu)
{
	// differs slightly from _interrupt

	cpu->regs.c++;

	_setFlag(cpu, P_I, 1);
	mos6500StackWriteAddr(cpu, cpu->regs.c);

	_setFlag(cpu, P_B, 1);
	mos6500StackWrite(cpu, cpu->regs.p);
	_setFlag(cpu, P_B, 0);

	cpu->regs.c = mos6500ReadAddr(cpu->bus, 65534);

	return 0;
}

void mos6500IRQ(MOS6500 *cpu)
{
	if (!_checkFlag(cpu, P_I))
		_interrupt(cpu, 65534, 7);
}

void mos6500NMI(MOS6500 *cpu)
{
	_interrupt(cpu, 65530, 8);
}

u8 mos6500RTI(MOS6500 *cpu)
{
	// restore flags
	cpu->regs.p = mos6500StackRead(cpu);
	_setFlag(cpu, P_B, 0);
	_setFlag(cpu, P_U, 0);

	// restore counter
	cpu->regs.c = mos6500StackReadAddr(cpu);

	return 0;
}

u8 mos6500RTS(MOS6500 *cpu)
{
	cpu->regs.c = mos6500StackReadAddr(cpu);
	return 0;
}


// pushing/popping

u8 mos6500PHA(MOS6500 *cpu)
{
	mos6500StackWrite(cpu, cpu->regs.a);
	return 0;
}

u8 mos6500PHP(MOS6500 *cpu)
{
	_setFlag(cpu, P_B, 1);
	_setFlag(cpu, P_U, 1);
	mos6500StackWrite(cpu, cpu->regs.p);
	_setFlag(cpu, P_B, 0);
	_setFlag(cpu, P_U, 0);
	return 0;
}

u8 mos6500PLA(MOS6500 *cpu)
{
	cpu->regs.a = mos6500StackRead(cpu);
	_setFlagsNZ(cpu, cpu->regs.a);

	return 0;
}

u8 mos6500PLP(MOS6500 *cpu)
{
	mos6500StackRead(cpu);
	_setFlag(cpu, P_U, 1);
	return 0;
}


// arith

u8 mos6500ADC(MOS6500 *cpu)
{
	u16 tmp = cpu->regs.a + mos6500Fetch(cpu) + _checkFlag(cpu, P_C);

	_setFlagsCNZ(cpu, tmp);
	_setFlag(cpu, P_V, ~((cpu->regs.a ^ cpu->cache) & (cpu->regs.a ^ tmp) & 128));

	cpu->regs.a = tmp & 255;

	return 1;
}

u8 mos6500DEC(MOS6500 *cpu)
{
	u8 tmp = mos6500Fetch(cpu) - 1;
	mos6500WriteLast(cpu, tmp);
	_setFlagsNZ(cpu, cpu->regs.x);

	return 0;
}

u8 mos6500DEX(MOS6500 *cpu)
{
	_setFlagsNZ(cpu, --cpu->regs.x);
	return 0;
}

u8 mos6500DEY(MOS6500 *cpu)
{
	_setFlagsNZ(cpu, --cpu->regs.y);
	return 0;
}

u8 mos6500INC(MOS6500 *cpu)
{
	u8 tmp = mos6500Fetch(cpu) + 1;
	mos6500WriteLast(cpu, tmp);
	_setFlagsNZ(cpu, tmp);

	return 0;
}

u8 mos6500INX(MOS6500 *cpu)
{
	_setFlagsNZ(cpu, ++cpu->regs.x);
	return 0;
}

u8 mos6500INY(MOS6500 *cpu)
{
	_setFlagsNZ(cpu, ++cpu->regs.y);
	return 0;
}

u8 mos6500SBC(MOS6500 *cpu)
{
	u16 value = mos6500Fetch(cpu) ^ 255; // invert the value
	u16 tmp = cpu->regs.a + value + _checkFlag(cpu, P_C);

	_setFlagsCNZ(cpu, tmp);

	_setFlag(cpu, P_V, (tmp ^ cpu->regs.a) & ((tmp ^ value) & 128));
	cpu->regs.a = tmp & 255;

	return 1;
}


// bitwise

u8 mos6500AND(MOS6500 *cpu)
{
	cpu->regs.a &= mos6500Fetch(cpu);
	_setFlagsNZ(cpu, cpu->regs.a);

	return 1;
}

u8 mos6500ASL(MOS6500 *cpu)
{
	u16 tmp = mos6500Fetch(cpu) << 1;

	_setFlagsCNZ(cpu, tmp);
	_checkMode(cpu, tmp & 255);

	return 0;
}

u8 mos6500EOR(MOS6500 *cpu)
{
	cpu->regs.a ^= mos6500Fetch(cpu);
	_setFlagsNZ(cpu, cpu->regs.a);

	return 1;
}

u8 mos6500LSR(MOS6500 *cpu)
{
	_setFlag(cpu, P_C, mos6500Fetch(cpu));

	u16 tmp = cpu->cache >> 1;

	_setFlagsNZ(cpu, tmp);
	_checkMode(cpu, tmp & 255);

	return 0;
}

u8 mos6500ORA(MOS6500 *cpu)
{
	cpu->regs.a |= mos6500Fetch(cpu);
	_setFlagsNZ(cpu, cpu->regs.a);

	return 1;
}

u8 mos6500ROL(MOS6500 *cpu)
{
	u16 tmp = (mos6500Fetch(cpu) << 1) | _checkFlag(cpu, P_C);

	_setFlagsCNZ(cpu, tmp);
	_checkMode(cpu, tmp & 255);

	return 0;
}

u8 mos6500ROR(MOS6500 *cpu)
{
	mos6500Fetch(cpu);
	u16 tmp = (cpu->cache << 7) | (cpu->cache >> 1);

	_setFlag(cpu, P_C, cpu->cache & 1);
	_setFlagsNZ(cpu, tmp);
	_checkMode(cpu, tmp & 255);

	return 0;
}


// comparison

u8 mos6500BIT(MOS6500 *cpu)
{
	u16 tmp = cpu->regs.a & mos6500Fetch(cpu);

	_setFlag(cpu, P_Z, tmp & 255 == 0);
	_setFlag(cpu, P_N, cpu->cache & 128);
	_setFlag(cpu, P_V, cpu->cache & 64);

	return 0;
}

u8 mos6500CMP(MOS6500 *cpu)
{
	u16 tmp = cpu->regs.a - mos6500Fetch(cpu);

	_setFlag(cpu, P_C, cpu->regs.a >= cpu->cache);
	_setFlagsNZ(cpu, tmp);

	return 1;
}

u8 mos6500CPX(MOS6500 *cpu)
{
	u16 tmp = cpu->regs.x - mos6500Fetch(cpu);

	_setFlag(cpu, P_C, cpu->regs.x >= cpu->cache);
	_setFlagsNZ(cpu, tmp);

	return 1;
}

u8 mos6500CPY(MOS6500 *cpu)
{
	u16 tmp = cpu->regs.y - mos6500Fetch(cpu);

	_setFlag(cpu, P_C, cpu->regs.y >= cpu->cache);
	_setFlagsNZ(cpu, tmp);

	return 1;
}


// jumping

u8 mos6500JMP(MOS6500 *cpu)
{
	cpu->regs.c = cpu->last_abs_addr;
	return 0;
}

u8 mos6500JSR(MOS6500 *cpu)
{
	mos6500StackWriteAddr(cpu, cpu->regs.c);
	cpu->regs.c = cpu->last_abs_addr;
	return 0;
}


// loading

u8 mos6500LDA(MOS6500 *cpu)
{
	cpu->regs.a = mos6500Fetch(cpu);
	_setFlagsNZ(cpu, cpu->regs.a);
	return 1;
}

u8 mos6500LDX(MOS6500 *cpu)
{
	cpu->regs.x = mos6500Fetch(cpu);
	_setFlagsNZ(cpu, cpu->regs.x);
	return 1;
}

u8 mos6500LDY(MOS6500 *cpu)
{
	cpu->regs.y = mos6500Fetch(cpu);
	_setFlagsNZ(cpu, cpu->regs.y);
	return 1;
}


// storing

u8 mos6500STA(MOS6500 *cpu)
{
	mos6500WriteLast(cpu, cpu->regs.a);
	return 0;
}

u8 mos6500STX(MOS6500 *cpu)
{
	mos6500WriteLast(cpu, cpu->regs.x);
	return 0;
}

u8 mos6500STY(MOS6500 *cpu)
{
	mos6500WriteLast(cpu, cpu->regs.y);
	return 0;
}


// transferring

u8 mos6500TAX(MOS6500 *cpu)
{
	cpu->regs.x = cpu->regs.a;
	_setFlagsNZ(cpu, cpu->regs.x);
	return 0;
}

u8 mos6500TAY(MOS6500 *cpu)
{
	cpu->regs.y = cpu->regs.a;
	_setFlagsNZ(cpu, cpu->regs.y);
	return 0;
}

u8 mos6500TSX(MOS6500 *cpu)
{
	cpu->regs.x = cpu->regs.s;
	_setFlagsNZ(cpu, cpu->regs.x);
	return 0;
}

u8 mos6500TXA(MOS6500 *cpu)
{
	cpu->regs.a = cpu->regs.x;
	_setFlagsNZ(cpu, cpu->regs.a);
	return 0;
}

u8 mos6500TXS(MOS6500 *cpu)
{
	cpu->regs.s = cpu->regs.x;
	return 0;
}

u8 mos6500TYA(MOS6500 *cpu)
{
	cpu->regs.a = cpu->regs.y;
	_setFlagsNZ(cpu, cpu->regs.a);
	return 0;
}


// misc

u8 mos6500NOP(MOS6500 *cpu)
{
	switch (cpu->last_op)
	{
		case 28:
		case 60:
		case 92:
		case 124:
		case 220:
		case 252:
			return 1;
		default:
			return 0;
	}
}
