#include <stdio.h>
#include <stdlib.h>

#include "disasm.h"
#include "isa.h"

// Metadata for the CPU's various operations
static const struct _OPCODE
{
	const char *sym; // mnemonic for (dis)assembly
	const uint8_t (*mode)(MOS_6500 *);
} OPCODE[] = {
	// 0x
	{ "BRK", &am6500_imp },
	{ "ORA", &am6500_indx },
	{ "JAM", NULL },
	{ "SLO", &am6500_indx },
	{ "NOP", &am6500_zp },
	{ "ORA", &am6500_zp },
	{ "ASL", &am6500_zp },
	{ "SLO", &am6500_zp },
	{ "PHP", &am6500_imp },
	{ "ORA", &am6500_imm },
	{ "ASL", &am6500_imp },
	{ "ANC", &am6500_imm },
	{ "NOP", &am6500_abs },
	{ "ORA", &am6500_abs },
	{ "ASL", &am6500_abs },
	{ "SLO", &am6500_abs },

	// 1x
	{ "BPL", &am6500_rel },
	{ "ORA", &am6500_indy },
	{ "JAM", NULL },
	{ "SLO", &am6500_indy },
	{ "NOP", &am6500_zpx },
	{ "ORA", &am6500_zpx },
	{ "ASL", &am6500_zpx },
	{ "SLO", &am6500_zpx },
	{ "CLC", &am6500_imp },
	{ "ORA", &am6500_absy },
	{ "NOP", &am6500_imp },
	{ "SLO", &am6500_absy },
	{ "NOP", &am6500_absx },
	{ "ORA", &am6500_absx },
	{ "ASL", &am6500_absx },
	{ "SLO", &am6500_absx },

	// 2x
	{ "JSR", &am6500_abs },
	{ "AND", &am6500_indx },
	{ "JAM", NULL },
	{ "RLA", &am6500_indx },
	{ "BIT", &am6500_zp },
	{ "AND", &am6500_zp },
	{ "ROL", &am6500_zp },
	{ "RLA", &am6500_zp },
	{ "PLP", &am6500_imp },
	{ "AND", &am6500_imm },
	{ "ROL", &am6500_imp },
	{ "ANC", &am6500_imm },
	{ "BIT", &am6500_abs },
	{ "AND", &am6500_abs },
	{ "ROL", &am6500_abs },
	{ "RLA", &am6500_abs },

	// 3x
	{ "BMI", &am6500_rel },
	{ "AND", &am6500_indy },
	{ "JAM", NULL },
	{ "RLA", &am6500_indy },
	{ "NOP", &am6500_zpx },
	{ "AND", &am6500_zpx },
	{ "ROL", &am6500_zpx },
	{ "RLA", &am6500_zpx },
	{ "SEC", &am6500_imp },
	{ "AND", &am6500_absy },
	{ "NOP", &am6500_imp },
	{ "RLA", &am6500_absy },
	{ "NOP", &am6500_absx },
	{ "AND", &am6500_absx },
	{ "ROL", &am6500_absx },
	{ "RLA", &am6500_absx },

	// 4x
	{ "RTI", &am6500_imp },
	{ "EOR", &am6500_indx },
	{ "JAM", NULL },
	{ "SRE", &am6500_indx },
	{ "NOP", &am6500_zp },
	{ "EOR", &am6500_zp },
	{ "LSR", &am6500_zp },
	{ "SRE", &am6500_zp },
	{ "PHA", &am6500_imp },
	{ "EOR", &am6500_imm },
	{ "LSR", &am6500_imp },
	{ "ALR", &am6500_abs },
	{ "JMP", &am6500_abs },
	{ "EOR", &am6500_abs },
	{ "LSR", &am6500_abs },
	{ "SRE", &am6500_abs },

	// 5x
	{ "BVC", &am6500_rel },
	{ "EOR", &am6500_indy },
	{ "JAM", NULL },
	{ "SRE", &am6500_indy },
	{ "NOP", &am6500_zpx },
	{ "EOR", &am6500_zpx },
	{ "LSR", &am6500_zpx },
	{ "SRE", &am6500_zpx },
	{ "CLI", &am6500_imp },
	{ "EOR", &am6500_absy },
	{ "NOP", &am6500_imp },
	{ "SRE", &am6500_absy },
	{ "NOP", &am6500_absx },
	{ "EOR", &am6500_absx },
	{ "LSR", &am6500_absx },
	{ "SRE", &am6500_absx },

	// 6x
	{ "RTS", &am6500_imp },
	{ "ADC", &am6500_indx },
	{ "JAM", NULL },
	{ "RRA", &am6500_indx },
	{ "NOP", &am6500_zp },
	{ "ADC", &am6500_zp },
	{ "ROR", &am6500_zp },
	{ "RRA", &am6500_zp },
	{ "PLA", &am6500_imp },
	{ "ADC", &am6500_imm },
	{ "ROR", &am6500_imp },
	{ "ARR", &am6500_imm },
	{ "JMP", &am6500_ind },
	{ "ADC", &am6500_abs },
	{ "ROR", &am6500_abs },
	{ "RRA", &am6500_abs },

	// 7x
	{ "BVS", &am6500_rel },
	{ "ADC", &am6500_indy },
	{ "JAM", NULL },
	{ "RRA", &am6500_indy },
	{ "NOP", &am6500_zpx },
	{ "ADC", &am6500_zpx },
	{ "ROR", &am6500_zpx },
	{ "RRA", &am6500_zpx },
	{ "SEI", &am6500_imp },
	{ "ADC", &am6500_absy },
	{ "NOP", &am6500_imp },
	{ "RRA", &am6500_absy },
	{ "NOP", &am6500_absx },
	{ "ADC", &am6500_absx },
	{ "ROR", &am6500_absx },
	{ "RRA", &am6500_absx },

	// 8x
	{ "NOP", &am6500_imm },
	{ "STA", &am6500_indx },
	{ "NOP", &am6500_imm },
	{ "SAX", &am6500_indx },
	{ "STY", &am6500_zp },
	{ "STA", &am6500_zp },
	{ "STX", &am6500_zp },
	{ "SAX", &am6500_zp },
	{ "DEY", &am6500_imp },
	{ "NOP", &am6500_imm },
	{ "TXA", &am6500_imp },
	{ "ANE", &am6500_imm },
	{ "STY", &am6500_abs },
	{ "STA", &am6500_abs },
	{ "STX", &am6500_abs },
	{ "SAX", &am6500_abs },

	// 9x
	{ "BCC", &am6500_rel },
	{ "STA", &am6500_indy },
	{ "JAM", NULL },
	{ "SHA", &am6500_indy },
	{ "STY", &am6500_zpx },
	{ "STA", &am6500_zpx },
	{ "STX", &am6500_zpy },
	{ "SAX", &am6500_zpy },
	{ "TYA", &am6500_imp },
	{ "STA", &am6500_absy },
	{ "TXS", &am6500_imp },
	{ "TAS", &am6500_absy },
	{ "SHY", &am6500_absx },
	{ "STA", &am6500_absx },
	{ "SHX", &am6500_absy },
	{ "SHA", &am6500_absy },

	// Ax
	{ "LDY", &am6500_imm },
	{ "LDA", &am6500_indx },
	{ "LDX", &am6500_imm },
	{ "LAX", &am6500_indx },
	{ "LDY", &am6500_zp },
	{ "LDA", &am6500_zp },
	{ "LDX", &am6500_zp },
	{ "LAX", &am6500_zp },
	{ "TAY", &am6500_imp },
	{ "LDA", &am6500_imm },
	{ "TAX", &am6500_imp },
	{ "LXA", &am6500_imm },
	{ "LDY", &am6500_abs },
	{ "LDA", &am6500_abs },
	{ "LDX", &am6500_abs },
	{ "LAX", &am6500_abs },

	// Bx
	{ "BCS", &am6500_rel },
	{ "LDA", &am6500_indy },
	{ "JAM", NULL },
	{ "LAX", &am6500_indy },
	{ "LDY", &am6500_zpx },
	{ "LDA", &am6500_zpx },
	{ "LDX", &am6500_zpy },
	{ "LAX", &am6500_zpy },
	{ "CLV", &am6500_imp },
	{ "LDA", &am6500_absy },
	{ "TSX", &am6500_imp },
	{ "LAS", &am6500_absy },
	{ "LDY", &am6500_absx },
	{ "LDA", &am6500_absx },
	{ "LDX", &am6500_absy },
	{ "LAX", &am6500_absy },

	// Cx
	{ "CPY", &am6500_imm },
	{ "CMP", &am6500_indx },
	{ "NOP", &am6500_imm },
	{ "DCP", &am6500_indx },
	{ "CPY", &am6500_zp },
	{ "CMP", &am6500_zp },
	{ "DEC", &am6500_zp },
	{ "DCP", &am6500_zp },
	{ "INY", &am6500_imp },
	{ "CMP", &am6500_imm },
	{ "DEX", &am6500_imp },
	{ "SBX", &am6500_imm },
	{ "CPY", &am6500_abs },
	{ "CMP", &am6500_abs },
	{ "DEC", &am6500_abs },
	{ "DCP", &am6500_abs },

	// Dx
	{ "BNE", &am6500_rel },
	{ "CMP", &am6500_indy },
	{ "JAM", NULL },
	{ "DCP", &am6500_indy },
	{ "NOP", &am6500_zpx },
	{ "CMP", &am6500_zpx },
	{ "DEC", &am6500_zpx },
	{ "DCP", &am6500_zpx },
	{ "CLD", &am6500_imp },
	{ "CMP", &am6500_absy },
	{ "NOP", &am6500_imp },
	{ "DCP", &am6500_absy },
	{ "NOP", &am6500_absx },
	{ "CMP", &am6500_absx },
	{ "DEC", &am6500_absx },
	{ "DCP", &am6500_absx },

	// Ex
	{ "CPX", &am6500_imm },
	{ "SBC", &am6500_indx },
	{ "NOP", &am6500_imm },
	{ "ISC", &am6500_indx },
	{ "CPX", &am6500_zp },
	{ "SBC", &am6500_zp },
	{ "INC", &am6500_zp },
	{ "ISC", &am6500_zp },
	{ "INX", &am6500_imp },
	{ "SBC", &am6500_imm },
	{ "NOP", &am6500_imp },
	{ "USBC", &am6500_imm },
	{ "CPX", &am6500_abs },
	{ "SBC", &am6500_abs },
	{ "INC", &am6500_abs },
	{ "ISC", &am6500_abs },

	// Fx
	{ "BEQ", &am6500_rel },
	{ "SBC", &am6500_indy },
	{ "JAM", NULL },
	{ "ISC", &am6500_indy },
	{ "NOP", &am6500_zpx },
	{ "SBC", &am6500_zpx },
	{ "INC", &am6500_zpx },
	{ "ISC", &am6500_zpx },
	{ "SED", &am6500_imp },
	{ "SBC", &am6500_absy },
	{ "NOP", &am6500_imp },
	{ "ISC", &am6500_absy },
	{ "NOP", &am6500_absx },
	{ "SBC", &am6500_absx },
	{ "INC", &am6500_absx },
	{ "ISC", &am6500_absx }
};

DISASM_6500 * disasm6500_op(uint8_t *start, uint16_t *offset)
{
	DISASM_6500 *disasm = (DISASM_6500 *)calloc(1, sizeof(DISASM_6500));
	const struct _OPCODE *op = &OPCODE[start[*offset++]];

	disasm->addr = *offset;

	sprintf((char *)&disasm->lhs, "%s", op->sym);

	if (op->mode == &am6500_imm)
		sprintf((char *)&disasm->rhs, "#$%02X", start[*offset++]);
	else if (op->mode == &am6500_zp)
		sprintf((char *)&disasm->rhs, "$%02X", start[*offset++]);
	else if (op->mode == &am6500_zpx)
		sprintf((char *)&disasm->rhs, "$%02X, x", start[*offset++]);
	else if (op->mode == &am6500_zpy)
		sprintf((char *)&disasm->rhs, "$%02X, y", start[*offset++]);
	else if (op->mode == &am6500_indx)
		sprintf((char *)&disasm->rhs, "($%02X, x)", start[*offset++]);
	else if (op->mode == &am6500_indy)
		sprintf((char *)&disasm->rhs, "($%02X, y)", start[*offset++]);
	else if (op->mode == &am6500_abs)
		sprintf((char *)&disasm->rhs, "$%04X", start[*offset++] | (start[*offset++] << 8));
	else if (op->mode == &am6500_absx)
		sprintf((char *)&disasm->rhs, "$%04X, x", start[*offset++] | (start[*offset++] << 8));
	else if (op->mode == &am6500_absy)
		sprintf((char *)&disasm->rhs, "$%04X, y", start[*offset++] | (start[*offset++] << 8));
	else if (op->mode == &am6500_ind)
		sprintf((char *)&disasm->rhs, "($%04X)", start[*offset++] | (start[*offset++] << 8));
	else if (op->mode == &am6500_rel)
	{
		uint8_t value = start[*offset++];

		sprintf((char *)&disasm->rhs, "$%02X [$%04X]", value, *offset + (int8_t)value);
	}

	return disasm;
}

DISASM_6500 * disasm6500_range(uint8_t *start, uint16_t offset, uint16_t end)
{
	DISASM_6500 *disasm = disasm6500_op(start, &offset);
	DISASM_6500 *it = disasm;

	while (offset < end)
	{
		it->next = disasm6500_op(start, &offset);
		it = it->next;
	}

	return disasm;
}

void disasm6500_free(DISASM_6500 *disasm)
{
	if (!disasm) return;

	DISASM_6500 *it = NULL;
	DISASM_6500 *next = disasm;

	while (next)
	{
		it = next;
		next = it->next;
		free(it);
	}
}

const DISASM_6500 * disasm6500_get(DISASM_6500 *disasm, uint16_t index)
{
	if (!disasm) return NULL;

	DISASM_6500 *it = disasm;
	while (index--) it = it->next;
	return it;
}

void disasm6500_print(DISASM_6500 *disasm)
{
	if (!disasm) return;

	DISASM_6500 *it = disasm;

	while (it)
	{
		printf("%04X: %s %s\n", it->addr, it->lhs, it->rhs);
		it = it->next;
	};
}
