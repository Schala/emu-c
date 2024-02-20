#include <stdio.h>
#include <stdlib.h>

#include "disasm.h"
#include "isa.h"


#define DISASM_STR_LEN 128


struct _MOS6500Disasm
{
	char lhs[DISASM_STR_LEN];
	char rhs[DISASM_STR_LEN];
	u16 addr;
	struct _MOS6500Disasm *next;
};


// Metadata for the CPU's various operations
static const struct _Opcode
{
	const char *sym; // mnemonic for (dis)assembly
	const u8 (*mode)(MOS6500 *);
} _opcode[] = {
	// 0x
	{ "BRK", &mos6500Implied },
	{ "ORA", &mos6500IndirectX },
	{ "NOP", &mos6500Implied },
	{ "NOP", &mos6500IndirectX },
	{ "NOP", &mos6500ZeroPage },
	{ "ORA", &mos6500ZeroPage },
	{ "ASL", &mos6500ZeroPage },
	{ "NOP", &mos6500ZeroPage },
	{ "PHP", &mos6500Implied },
	{ "ORA", &mos6500Immediate },
	{ "ASL", &mos6500Implied },
	{ "NOP", &mos6500Immediate },
	{ "NOP", &mos6500Absolute },
	{ "ORA", &mos6500Absolute },
	{ "ASL", &mos6500Absolute },
	{ "NOP", &mos6500Absolute },

	// 1x
	{ "BPL", &mos6500Relative },
	{ "ORA", &mos6500IndirectY },
	{ "NOP", &mos6500Implied },
	{ "NOP", &mos6500IndirectY },
	{ "NOP", &mos6500ZeroPageX },
	{ "ORA", &mos6500ZeroPageX },
	{ "ASL", &mos6500ZeroPageX },
	{ "NOP", &mos6500ZeroPageX },
	{ "CLC", &mos6500Implied },
	{ "ORA", &mos6500AbsoluteY },
	{ "NOP", &mos6500Implied },
	{ "NOP", &mos6500AbsoluteY },
	{ "NOP", &mos6500AbsoluteX },
	{ "ORA", &mos6500AbsoluteX },
	{ "ASL", &mos6500AbsoluteX },
	{ "NOP", &mos6500AbsoluteX },

	// 2x
	{ "JSR", &mos6500Absolute },
	{ "AND", &mos6500IndirectX },
	{ "NOP", &mos6500Implied },
	{ "NOP", &mos6500IndirectX },
	{ "BIT", &mos6500ZeroPage },
	{ "AND", &mos6500ZeroPage },
	{ "ROL", &mos6500ZeroPage },
	{ "NOP", &mos6500ZeroPage },
	{ "PLP", &mos6500Implied },
	{ "AND", &mos6500Immediate },
	{ "ROL", &mos6500Implied },
	{ "NOP", &mos6500Immediate },
	{ "BIT", &mos6500Absolute },
	{ "AND", &mos6500Absolute },
	{ "ROL", &mos6500Absolute },
	{ "NOP", &mos6500Absolute },

	// 3x
	{ "BMI", &mos6500Relative },
	{ "AND", &mos6500IndirectY },
	{ "NOP", &mos6500Implied },
	{ "NOP", &mos6500IndirectY },
	{ "NOP", &mos6500ZeroPageX },
	{ "AND", &mos6500ZeroPageX },
	{ "ROL", &mos6500ZeroPageX },
	{ "NOP", &mos6500ZeroPageX },
	{ "SEC", &mos6500Implied },
	{ "AND", &mos6500AbsoluteY },
	{ "NOP", &mos6500Implied },
	{ "NOP", &mos6500AbsoluteY },
	{ "NOP", &mos6500AbsoluteX },
	{ "AND", &mos6500AbsoluteX },
	{ "ROL", &mos6500AbsoluteX },
	{ "NOP", &mos6500AbsoluteX },

	// 4x
	{ "RTI", &mos6500Implied },
	{ "EOR", &mos6500IndirectX },
	{ "NOP", &mos6500Implied },
	{ "NOP", &mos6500IndirectX },
	{ "NOP", &mos6500ZeroPage },
	{ "EOR", &mos6500ZeroPage },
	{ "LSR", &mos6500ZeroPage },
	{ "NOP", &mos6500ZeroPage },
	{ "PHA", &mos6500Implied },
	{ "EOR", &mos6500Immediate },
	{ "LSR", &mos6500Implied },
	{ "NOP", &mos6500Absolute },
	{ "JMP", &mos6500Absolute },
	{ "EOR", &mos6500Absolute },
	{ "LSR", &mos6500Absolute },
	{ "NOP", &mos6500Absolute },

	// 5x
	{ "BVC", &mos6500Relative },
	{ "EOR", &mos6500IndirectY },
	{ "NOP", &mos6500Implied },
	{ "NOP", &mos6500IndirectY },
	{ "NOP", &mos6500ZeroPageX },
	{ "EOR", &mos6500ZeroPageX },
	{ "LSR", &mos6500ZeroPageX },
	{ "NOP", &mos6500ZeroPageX },
	{ "CLI", &mos6500Implied },
	{ "EOR", &mos6500AbsoluteY },
	{ "NOP", &mos6500Implied },
	{ "NOP", &mos6500AbsoluteY },
	{ "NOP", &mos6500AbsoluteX },
	{ "EOR", &mos6500AbsoluteX },
	{ "LSR", &mos6500AbsoluteX },
	{ "NOP", &mos6500AbsoluteX },

	// 6x
	{ "RTS", &mos6500Implied },
	{ "ADC", &mos6500IndirectX },
	{ "NOP", &mos6500Implied },
	{ "NOP", &mos6500IndirectX },
	{ "NOP", &mos6500ZeroPage },
	{ "ADC", &mos6500ZeroPage },
	{ "ROR", &mos6500ZeroPage },
	{ "NOP", &mos6500ZeroPage },
	{ "PLA", &mos6500Implied },
	{ "ADC", &mos6500Immediate },
	{ "ROR", &mos6500Implied },
	{ "ARR", &mos6500Immediate },
	{ "JMP", &mos6500Indirect },
	{ "ADC", &mos6500Absolute },
	{ "ROR", &mos6500Absolute },
	{ "NOP", &mos6500Absolute },

	// 7x
	{ "BVS", &mos6500Relative },
	{ "ADC", &mos6500IndirectY },
	{ "NOP", &mos6500Implied },
	{ "NOP", &mos6500IndirectY },
	{ "NOP", &mos6500ZeroPageX },
	{ "ADC", &mos6500ZeroPageX },
	{ "ROR", &mos6500ZeroPageX },
	{ "NOP", &mos6500ZeroPageX },
	{ "SEI", &mos6500Implied },
	{ "ADC", &mos6500AbsoluteY },
	{ "NOP", &mos6500Implied },
	{ "NOP", &mos6500AbsoluteY },
	{ "NOP", &mos6500AbsoluteX },
	{ "ADC", &mos6500AbsoluteX },
	{ "ROR", &mos6500AbsoluteX },
	{ "NOP", &mos6500AbsoluteX },

	// 8x
	{ "NOP", &mos6500Immediate },
	{ "STA", &mos6500IndirectX },
	{ "NOP", &mos6500Immediate },
	{ "NOP", &mos6500IndirectX },
	{ "STY", &mos6500ZeroPage },
	{ "STA", &mos6500ZeroPage },
	{ "STX", &mos6500ZeroPage },
	{ "NOP", &mos6500ZeroPage },
	{ "DEY", &mos6500Implied },
	{ "NOP", &mos6500Immediate },
	{ "TXA", &mos6500Implied },
	{ "NOP", &mos6500Immediate },
	{ "STY", &mos6500Absolute },
	{ "STA", &mos6500Absolute },
	{ "STX", &mos6500Absolute },
	{ "NOP", &mos6500Absolute },

	// 9x
	{ "BCC", &mos6500Relative },
	{ "STA", &mos6500IndirectY },
	{ "NOP", &mos6500Implied },
	{ "NOP", &mos6500IndirectY },
	{ "STY", &mos6500ZeroPageX },
	{ "STA", &mos6500ZeroPageX },
	{ "STX", &mos6500ZeroPagey },
	{ "NOP", &mos6500ZeroPagey },
	{ "TYA", &mos6500Implied },
	{ "STA", &mos6500AbsoluteY },
	{ "TXS", &mos6500Implied },
	{ "TAS", &mos6500AbsoluteY },
	{ "NOP", &mos6500AbsoluteX },
	{ "STA", &mos6500AbsoluteX },
	{ "NOP", &mos6500AbsoluteY },
	{ "NOP", &mos6500AbsoluteY },

	// Ax
	{ "LDY", &mos6500Immediate },
	{ "LDA", &mos6500IndirectX },
	{ "LDX", &mos6500Immediate },
	{ "NOP", &mos6500IndirectX },
	{ "LDY", &mos6500ZeroPage },
	{ "LDA", &mos6500ZeroPage },
	{ "LDX", &mos6500ZeroPage },
	{ "NOP", &mos6500ZeroPage },
	{ "TAY", &mos6500Implied },
	{ "LDA", &mos6500Immediate },
	{ "TAX", &mos6500Implied },
	{ "NOP", &mos6500Immediate },
	{ "LDY", &mos6500Absolute },
	{ "LDA", &mos6500Absolute },
	{ "LDX", &mos6500Absolute },
	{ "NOP", &mos6500Absolute },

	// Bx
	{ "BCS", &mos6500Relative },
	{ "LDA", &mos6500IndirectY },
	{ "NOP", &mos6500Implied },
	{ "NOP", &mos6500IndirectY },
	{ "LDY", &mos6500ZeroPageX },
	{ "LDA", &mos6500ZeroPageX },
	{ "LDX", &mos6500ZeroPagey },
	{ "NOP", &mos6500ZeroPagey },
	{ "CLV", &mos6500Implied },
	{ "LDA", &mos6500AbsoluteY },
	{ "TSX", &mos6500Implied },
	{ "NOP", &mos6500AbsoluteY },
	{ "LDY", &mos6500AbsoluteX },
	{ "LDA", &mos6500AbsoluteX },
	{ "LDX", &mos6500AbsoluteY },
	{ "NOP", &mos6500AbsoluteY },

	// Cx
	{ "CPY", &mos6500Immediate },
	{ "CMP", &mos6500IndirectX },
	{ "NOP", &mos6500Immediate },
	{ "NOP", &mos6500IndirectX },
	{ "CPY", &mos6500ZeroPage },
	{ "CMP", &mos6500ZeroPage },
	{ "DEC", &mos6500ZeroPage },
	{ "NOP", &mos6500ZeroPage },
	{ "INY", &mos6500Implied },
	{ "CMP", &mos6500Immediate },
	{ "DEX", &mos6500Implied },
	{ "NOP", &mos6500Immediate },
	{ "CPY", &mos6500Absolute },
	{ "CMP", &mos6500Absolute },
	{ "DEC", &mos6500Absolute },
	{ "NOP", &mos6500Absolute },

	// Dx
	{ "BNE", &mos6500Relative },
	{ "CMP", &mos6500IndirectY },
	{ "NOP", &mos6500Implied },
	{ "NOP", &mos6500IndirectY },
	{ "NOP", &mos6500ZeroPageX },
	{ "CMP", &mos6500ZeroPageX },
	{ "DEC", &mos6500ZeroPageX },
	{ "NOP", &mos6500ZeroPageX },
	{ "CLD", &mos6500Implied },
	{ "CMP", &mos6500AbsoluteY },
	{ "NOP", &mos6500Implied },
	{ "NOP", &mos6500AbsoluteY },
	{ "NOP", &mos6500AbsoluteX },
	{ "CMP", &mos6500AbsoluteX },
	{ "DEC", &mos6500AbsoluteX },
	{ "NOP", &mos6500AbsoluteX },

	// Ex
	{ "CPX", &mos6500Immediate },
	{ "SBC", &mos6500IndirectX },
	{ "NOP", &mos6500Immediate },
	{ "NOP", &mos6500IndirectX },
	{ "CPX", &mos6500ZeroPage },
	{ "SBC", &mos6500ZeroPage },
	{ "INC", &mos6500ZeroPage },
	{ "NOP", &mos6500ZeroPage },
	{ "INX", &mos6500Implied },
	{ "SBC", &mos6500Immediate },
	{ "NOP", &mos6500Implied },
	{ "NOP", &mos6500Immediate },
	{ "CPX", &mos6500Absolute },
	{ "SBC", &mos6500Absolute },
	{ "INC", &mos6500Absolute },
	{ "NOP", &mos6500Absolute },

	// Fx
	{ "BEQ", &mos6500Relative },
	{ "SBC", &mos6500IndirectY },
	{ "NOP", &mos6500Implied },
	{ "NOP", &mos6500IndirectY },
	{ "NOP", &mos6500ZeroPageX },
	{ "SBC", &mos6500ZeroPageX },
	{ "INC", &mos6500ZeroPageX },
	{ "NOP", &mos6500ZeroPageX },
	{ "SED", &mos6500Implied },
	{ "SBC", &mos6500AbsoluteY },
	{ "NOP", &mos6500Implied },
	{ "NOP", &mos6500AbsoluteY },
	{ "NOP", &mos6500AbsoluteX },
	{ "SBC", &mos6500AbsoluteX },
	{ "INC", &mos6500AbsoluteX },
	{ "NOP", &mos6500AbsoluteX }
};

MOS6500Disasm * mos6500DisasmOp(const hxbuf buf, size_t *offset)
{
	MOS6500Disasm *disasm = (MOS6500Disasm *)hxAlloc(sizeof(MOS6500Disasm);
	const struct _Opcode *op = &OPCODE[buf[*offset++]];

	disasm->addr = *offset;

	sprintf(HXSTR(&disasm->lhs), "%s", op->sym);

	if (op->mode == &mos6500Immediate)
		sprintf(HXSTR(&disasm->rhs), "#$%02X", buf[*offset++]);
	else if (op->mode == &mos6500ZeroPage)
		sprintf(HXSTR(&disasm->rhs), "$%02X", buf[*offset++]);
	else if (op->mode == &mos6500ZeroPageX)
		sprintf(HXSTR(&disasm->rhs), "$%02X, X", buf[*offset++]);
	else if (op->mode == &mos6500ZeroPagey)
		sprintf(HXSTR(&disasm->rhs), "$%02X, Y", buf[*offset++]);
	else if (op->mode == &mos6500IndirectX)
		sprintf(HXSTR(&disasm->rhs), "($%02X, X)", buf[*offset++]);
	else if (op->mode == &mos6500IndirectY)
		sprintf(HXSTR(&disasm->rhs), "($%02X, Y)", buf[*offset++]);
	else if (op->mode == &mos6500Absolute)
		sprintf(HXSTR(&disasm->rhs), "$%04X", buf[*offset++] | (buf[*offset++] << 8));
	else if (op->mode == &mos6500AbsoluteX)
		sprintf(HXSTR(&disasm->rhs), "$%04X, X", buf[*offset++] | (buf[*offset++] << 8));
	else if (op->mode == &mos6500AbsoluteY)
		sprintf(HXSTR(&disasm->rhs), "$%04X, Y", buf[*offset++] | (buf[*offset++] << 8));
	else if (op->mode == &mos6500Indirect)
		sprintf(HXSTR(&disasm->rhs), "($%04X)", buf[*offset++] | (buf[*offset++] << 8));
	else if (op->mode == &mos6500Relative)
	{
		u8 value = buf[*offset++];

		sprintf(HXSTR(&disasm->rhs), "$%02X [$%04X]", value, *offset + (i8)value);
	}

	return disasm;
}

MOS6500Disasm * disasm6500_range(u8 *buf, size_t offset, size_t end)
{
	MOS6500Disasm *disasm = disasm6500_op(buf, &offset);
	MOS6500Disasm *it = disasm;

	while (offset < end)
	{
		it->next = disasm6500_op(buf, &offset);
		it = it->next;
	}

	return disasm;
}

void disasm6500_free(MOS6500Disasm *disasm)
{
	if (!disasm) return;

	MOS6500Disasm *it = NULL;
	MOS6500Disasm *next = disasm;

	while (next)
	{
		it = next;
		next = it->next;
		free(it);
	}
}

const MOS6500Disasm * disasm6500_get(MOS6500Disasm *disasm, size_t index)
{
	if (!disasm) return NULL;

	MOS6500Disasm *it = disasm;
	while (index--) it = it->next;
	return it;
}

void disasm6500_print(MOS6500Disasm *disasm)
{
	if (!disasm) return;

	MOS6500Disasm *it = disasm;

	while (it)
	{
		printf("%04X: %s %s\n", it->addr, it->lhs, it->rhs);
		it = it->next;
	};
}
