#include "internal.h"
#include "isa.h"

// Metadata for the CPU's various operations
static const struct _OPCODE
{
	uint8_t cycles;
	const uint8_t (*mode)(MOS_6500 *);
	const uint8_t (*op)(MOS_6500 *);
} OPCODE[] = {
	// 0x
	{ 7, &am6500_imp, &op6500_brk },
	{ 6, &am6500_indx, &op6500_ora },
	{ 0, &am6500_imp, &op6500_nop },
	{ 8, &am6500_indx, &op6500_nop },
	{ 2, &am6500_zp, &op6500_nop },
	{ 3, &am6500_zp, &op6500_ora },
	{ 5, &am6500_zp, &op6500_asl },
	{ 5, &am6500_zp, &op6500_nop },
	{ 3, &am6500_imp, &op6500_php },
	{ 2, &am6500_imm, &op6500_ora },
	{ 2, &am6500_imp, &op6500_asl },
	{ 2, &am6500_imm, &op6500_nop },
	{ 4, &am6500_abs, &op6500_nop },
	{ 4, &am6500_abs, &op6500_ora },
	{ 6, &am6500_abs, &op6500_asl },
	{ 6, &am6500_abs, &op6500_nop },

	// 1x
	{ 2, &am6500_rel, &op6500_bpl },
	{ 5, &am6500_indy, &op6500_ora },
	{ 0, &am6500_imp, &op6500_nop },
	{ 8, &am6500_indy, &op6500_nop },
	{ 4, &am6500_zpx, &op6500_nop },
	{ 4, &am6500_zpx, &op6500_ora },
	{ 6, &am6500_zpx, &op6500_asl },
	{ 6, &am6500_zpx, &op6500_nop },
	{ 2, &am6500_imp, &op6500_clc },
	{ 4, &am6500_absy, &op6500_ora },
	{ 2, &am6500_imp, &op6500_nop },
	{ 7, &am6500_absy, &op6500_nop },
	{ 4, &am6500_absx, &op6500_nop },
	{ 4, &am6500_absx, &op6500_ora },
	{ 7, &am6500_absx, &op6500_asl },
	{ 7, &am6500_absx, &op6500_nop },

	// 2x
	{ 6, &am6500_abs, &op6500_jsr },
	{ 6, &am6500_indx, &op6500_and },
	{ 0, &am6500_imp, &op6500_nop },
	{ 8, &am6500_indx, &op6500_nop },
	{ 3, &am6500_zp, &op6500_bit },
	{ 3, &am6500_zp, &op6500_and },
	{ 5, &am6500_zp, &op6500_rol },
	{ 5, &am6500_zp, &op6500_nop },
	{ 4, &am6500_imp, &op6500_plp },
	{ 2, &am6500_imm, &op6500_and },
	{ 2, &am6500_imp, &op6500_rol },
	{ 2, &am6500_imm, &op6500_nop },
	{ 4, &am6500_abs, &op6500_bit },
	{ 4, &am6500_abs, &op6500_and },
	{ 6, &am6500_abs, &op6500_rol },
	{ 6, &am6500_abs, &op6500_nop },

	// 3x
	{ 2, &am6500_rel, &op6500_bmi },
	{ 5, &am6500_indy, &op6500_and },
	{ 0, &am6500_imp, &op6500_nop },
	{ 8, &am6500_indy, &op6500_nop },
	{ 4, &am6500_zpx, &op6500_nop },
	{ 4, &am6500_zpx, &op6500_and },
	{ 6, &am6500_zpx, &op6500_rol },
	{ 6, &am6500_zpx, &op6500_nop },
	{ 2, &am6500_imp, &op6500_sec },
	{ 4, &am6500_absy, &op6500_and },
	{ 2, &am6500_imp, &op6500_nop },
	{ 7, &am6500_absy, &op6500_nop },
	{ 4, &am6500_absx, &op6500_nop },
	{ 4, &am6500_absx, &op6500_and },
	{ 7, &am6500_absx, &op6500_rol },
	{ 7, &am6500_absx, &op6500_nop },

	// 4x
	{ 6, &am6500_imp, &op6500_rti },
	{ 6, &am6500_indx, &op6500_eor },
	{ 0, &am6500_imp, &op6500_nop },
	{ 8, &am6500_indx, &op6500_nop },
	{ 3, &am6500_zp, &op6500_nop },
	{ 3, &am6500_zp, &op6500_eor },
	{ 5, &am6500_zp, &op6500_lsr },
	{ 5, &am6500_zp, &op6500_nop },
	{ 3, &am6500_imp, &op6500_pha },
	{ 2, &am6500_imm, &op6500_eor },
	{ 2, &am6500_imp, &op6500_lsr },
	{ 2, &am6500_abs, &op6500_nop },
	{ 3, &am6500_abs, &op6500_jmp },
	{ 4, &am6500_abs, &op6500_eor },
	{ 6, &am6500_abs, &op6500_lsr },
	{ 6, &am6500_abs, &op6500_nop },

	// 5x
	{ 2, &am6500_rel, &op6500_bvc },
	{ 5, &am6500_indy, &op6500_eor },
	{ 0, &am6500_imp, &op6500_nop },
	{ 8, &am6500_indy, &op6500_nop },
	{ 4, &am6500_zpx, &op6500_nop },
	{ 4, &am6500_zpx, &op6500_eor },
	{ 6, &am6500_zpx, &op6500_lsr },
	{ 6, &am6500_zpx, &op6500_nop },
	{ 2, &am6500_imp, &op6500_cli },
	{ 4, &am6500_absy, &op6500_eor },
	{ 2, &am6500_imp, &op6500_nop },
	{ 7, &am6500_absy, &op6500_nop },
	{ 4, &am6500_absx, &op6500_nop },
	{ 4, &am6500_absx, &op6500_eor },
	{ 7, &am6500_absx, &op6500_lsr },
	{ 7, &am6500_absx, &op6500_nop },

	// 6x
	{ 6, &am6500_imp, &op6500_rts },
	{ 6, &am6500_indx, &op6500_adc },
	{ 0, &am6500_imp, &op6500_nop },
	{ 8, &am6500_indx, &op6500_nop },
	{ 3, &am6500_zp, &op6500_nop },
	{ 3, &am6500_zp, &op6500_adc },
	{ 5, &am6500_zp, &op6500_ror },
	{ 5, &am6500_zp, &op6500_nop },
	{ 4, &am6500_imp, &op6500_pla },
	{ 2, &am6500_imm, &op6500_adc },
	{ 2, &am6500_imp, &op6500_ror },
	{ 2, &am6500_imm, &op6500_nop },
	{ 5, &am6500_ind, &op6500_jmp },
	{ 4, &am6500_abs, &op6500_adc },
	{ 6, &am6500_abs, &op6500_ror },
	{ 6, &am6500_abs, &op6500_nop },

	// 7x
	{ 2, &am6500_rel, &op6500_bvs },
	{ 5, &am6500_indy, &op6500_adc },
	{ 0, &am6500_imp, &op6500_nop },
	{ 8, &am6500_indy, &op6500_nop },
	{ 4, &am6500_zpx, &op6500_nop },
	{ 4, &am6500_zpx, &op6500_adc },
	{ 6, &am6500_zpx, &op6500_ror },
	{ 6, &am6500_zpx, &op6500_nop },
	{ 2, &am6500_imp, &op6500_sei },
	{ 4, &am6500_absy, &op6500_adc },
	{ 2, &am6500_imp, &op6500_nop },
	{ 7, &am6500_absy, &op6500_nop },
	{ 4, &am6500_absx, &op6500_nop },
	{ 4, &am6500_absx, &op6500_adc },
	{ 7, &am6500_absx, &op6500_ror },
	{ 7, &am6500_absx, &op6500_nop },

	// 8x
	{ 2, &am6500_imm, &op6500_nop },
	{ 6, &am6500_indx, &op6500_sta },
	{ 2, &am6500_imm, &op6500_nop },
	{ 6, &am6500_indx, &op6500_nop },
	{ 3, &am6500_zp, &op6500_sty },
	{ 3, &am6500_zp, &op6500_sta },
	{ 3, &am6500_zp, &op6500_stx },
	{ 3, &am6500_zp, &op6500_nop },
	{ 2, &am6500_imp, &op6500_dey },
	{ 2, &am6500_imm, &op6500_nop },
	{ 2, &am6500_imp, &op6500_txa },
	{ 2, &am6500_imm, &op6500_nop },
	{ 4, &am6500_abs, &op6500_sty },
	{ 4, &am6500_abs, &op6500_sta },
	{ 4, &am6500_abs, &op6500_stx },
	{ 4, &am6500_abs, &op6500_nop },

	// 9x
	{ 2, &am6500_rel, &op6500_bcc },
	{ 6, &am6500_indy, &op6500_sta },
	{ 0, &am6500_imp, &op6500_nop },
	{ 6, &am6500_indy, &op6500_nop },
	{ 4, &am6500_zpx, &op6500_sty },
	{ 4, &am6500_zpx, &op6500_sta },
	{ 4, &am6500_zpy, &op6500_stx },
	{ 4, &am6500_zpy, &op6500_nop },
	{ 2, &am6500_imp, &op6500_tya },
	{ 5, &am6500_absy, &op6500_sta },
	{ 2, &am6500_imp, &op6500_txs },
	{ 5, &am6500_absy, &op6500_nop },
	{ 5, &am6500_absx, &op6500_nop },
	{ 5, &am6500_absx, &op6500_sta },
	{ 6, &am6500_absy, &op6500_nop },
	{ 5, &am6500_absy, &op6500_nop },

	// Ax
	{ 2, &am6500_imm, &op6500_ldy },
	{ 6, &am6500_indx, &op6500_lda },
	{ 2, &am6500_imm, &op6500_ldx },
	{ 6, &am6500_indx, &op6500_nop },
	{ 3, &am6500_zp, &op6500_ldy },
	{ 3, &am6500_zp, &op6500_lda },
	{ 3, &am6500_zp, &op6500_ldx },
	{ 3, &am6500_zp, &op6500_nop },
	{ 2, &am6500_imp, &op6500_tay },
	{ 2, &am6500_imm, &op6500_lda },
	{ 2, &am6500_imp, &op6500_tax },
	{ 2, &am6500_imm, &op6500_lxa },
	{ 4, &am6500_abs, &op6500_ldy },
	{ 4, &am6500_abs, &op6500_lda },
	{ 4, &am6500_abs, &op6500_ldx },
	{ 4, &am6500_abs, &op6500_nop },

	// Bx
	{ 2, &am6500_rel, &op6500_bcs },
	{ 5, &am6500_indy, &op6500_lda },
	{ 0, &am6500_imp, &op6500_nop },
	{ 5, &am6500_indy, &op6500_nop },
	{ 4, &am6500_zpx, &op6500_ldy },
	{ 4, &am6500_zpx, &op6500_lda },
	{ 4, &am6500_zpy, &op6500_ldx },
	{ 4, &am6500_zpy, &op6500_nop },
	{ 2, &am6500_imp, &op6500_clv },
	{ 4, &am6500_absy, &op6500_lda },
	{ 2, &am6500_imp, &op6500_tsx },
	{ 4, &am6500_absy, &op6500_nop },
	{ 4, &am6500_absx, &op6500_ldy },
	{ 4, &am6500_absx, &op6500_lda },
	{ 4, &am6500_absy, &op6500_ldx },
	{ 4, &am6500_absy, &op6500_nop },

	// Cx
	{ 2, &am6500_imm, &op6500_cpy },
	{ 6, &am6500_indx, &op6500_cmp },
	{ 2, &am6500_imm, &op6500_nop },
	{ 8, &am6500_indx, &op6500_nop },
	{ 3, &am6500_zp, &op6500_cpy },
	{ 3, &am6500_zp, &op6500_cmp },
	{ 5, &am6500_zp, &op6500_dec },
	{ 5, &am6500_zp, &op6500_nop },
	{ 2, &am6500_imp, &op6500_iny },
	{ 2, &am6500_imm, &op6500_cmp },
	{ 2, &am6500_imp, &op6500_dex },
	{ 2, &am6500_imm, &op6500_nop },
	{ 4, &am6500_abs, &op6500_cpy },
	{ 4, &am6500_abs, &op6500_cmp },
	{ 6, &am6500_abs, &op6500_dec },
	{ 6, &am6500_abs, &op6500_nop },

	// Dx
	{ 2, &am6500_rel, &op6500_bne },
	{ 5, &am6500_indy, &op6500_cmp },
	{ 0, &am6500_imp, &op6500_nop },
	{ 8, &am6500_indy, &op6500_nop },
	{ 4, &am6500_zpx, &op6500_nop },
	{ 4, &am6500_zpx, &op6500_cmp },
	{ 6, &am6500_zpx, &op6500_dec },
	{ 6, &am6500_zpx, &op6500_nop },
	{ 2, &am6500_imp, &op6500_cld },
	{ 4, &am6500_absy, &op6500_cmp },
	{ 2, &am6500_imp, &op6500_nop },
	{ 7, &am6500_absy, &op6500_nop },
	{ 4, &am6500_absx, &op6500_nop },
	{ 4, &am6500_absx, &op6500_cmp },
	{ 7, &am6500_absx, &op6500_dec },
	{ 7, &am6500_absx, &op6500_nop },

	// Ex
	{ 2, &am6500_imm, &op6500_cpx },
	{ 6, &am6500_indx, &op6500_sbc },
	{ 2, &am6500_imm, &op6500_nop },
	{ 8, &am6500_indx, &op6500_nop },
	{ 3, &am6500_zp, &op6500_cpx },
	{ 3, &am6500_zp, &op6500_sbc },
	{ 5, &am6500_zp, &op6500_inc },
	{ 5, &am6500_zp, &op6500_nop },
	{ 2, &am6500_imp, &op6500_inx },
	{ 2, &am6500_imm, &op6500_sbc },
	{ 2, &am6500_imp, &op6500_nop },
	{ 2, &am6500_imm, &op6500_nop },
	{ 4, &am6500_abs, &op6500_cpx },
	{ 4, &am6500_abs, &op6500_sbc },
	{ 6, &am6500_abs, &op6500_inc },
	{ 6, &am6500_abs, &op6500_nop },

	// Fx
	{ 2, &am6500_rel, &op6500_beq },
	{ 5, &am6500_indy, &op6500_sbc },
	{ 0, &am6500_imp, &op6500_nop },
	{ 4, &am6500_indy, &op6500_nop },
	{ 4, &am6500_zpx, &op6500_nop },
	{ 4, &am6500_zpx, &op6500_sbc },
	{ 6, &am6500_zpx, &op6500_inc },
	{ 6, &am6500_zpx, &op6500_nop },
	{ 2, &am6500_imp, &op6500_sed },
	{ 4, &am6500_absy, &op6500_sbc },
	{ 2, &am6500_imp, &op6500_nop },
	{ 7, &am6500_absy, &op6500_nop },
	{ 4, &am6500_absx, &op6500_nop },
	{ 4, &am6500_absx, &op6500_sbc },
	{ 7, &am6500_absx, &op6500_inc },
	{ 7, &am6500_absx, &op6500_nop }
};

void mos6500_clock(MOS_6500 *cpu)
{
	if (!cpu) return;

	if (cpu->cycles == 0)
	{
		set_flag(cpu, P_U, 1); // always set unused flag

		cpu->frame[cpu->frame_index] = cpu->regs.c;
		cpu->frame_index = cpu->frame_index-- % MOS6500_FRAME_LEN;

		// get and increment the counter
		cpu->last_op = mos6500_read_rom(cpu);

		// set cycles, see if any additional cycles are needed
		cpu->cycles = OPCODE[cpu->last_op].cycles;
		uint8_t extra1 = OPCODE[cpu->last_op].mode(cpu);
		uint8_t extra2 = OPCODE[cpu->last_op].op(cpu);
		cpu->cycles += extra1 & extra2;

		set_flag(cpu, P_U, 1); // always set unused flag
	}

	cpu->cycles--;
}

uint8_t mos6500_fetch(MOS_6500 *cpu)
{
	if (!cpu) return 0;

	if (OPCODE[cpu->last_op].mode != &am6500_imp)
		cpu->cache = bus6500_read(cpu->bus, bus6500_read_addr(cpu->bus, cpu->last_abs_addr));

	return cpu->cache;
}

// Common functionality for branch instructions
static inline void branch(MOS_6500 *cpu)
{
	cpu->cycles++;
	cpu->last_abs_addr = cpu->regs.c + cpu->last_rel_addr;

	// need an additional cycle if different page
	if (HI16(cpu->last_abs_addr) != HI16(cpu->regs.c))
		cpu->cycles++;

	// jump to address
	cpu->regs.c = cpu->last_abs_addr;
}

// Assign to accumulator, or write to bus, depending on the address mode
static inline void check_mode(MOS_6500 *cpu, uint16_t value)
{
	if (OPCODE[cpu->last_op].mode == &am6500_imp)
		cpu->regs.a = value;
	else
		mos6500_write_last(cpu, value);
}

// Set negative and/or zero bits of state flags register, given a value
static inline void flags_nz(MOS_6500 *cpu, uint16_t value)
{
	set_flag(cpu, P_Z, value & 255 == 0);
	set_flag(cpu, P_N, value & 128);
}

// Set carry, negative, and/or zero bits of state flags register, given a 16-bit value
static inline void flags_cnz(MOS_6500 *cpu, uint16_t value)
{
	set_flag(cpu, P_C, value > 255);
	flags_nz(cpu, value);
}

// Common functionality for interrupt operations
static inline void interrupt(MOS_6500 *cpu, uint16_t new_abs_addr, uint8_t new_cycles)
{
	// write the counter's current value to stack
	mos6500_stack_write_addr(cpu, cpu->regs.c);

	// set and write flags register to stack too
	set_flag(cpu, P_B, 0);
	set_flag(cpu, P_U, 1);
	set_flag(cpu, P_I, 1);
	mos6500_stack_write(cpu, cpu->regs.p);

	// get a new counter
	cpu->last_abs_addr = new_abs_addr;
	cpu->regs.c = mos6500_fetch_addr(cpu);

	cpu->cycles = new_cycles;
}

// address modes

uint8_t am6500_abs(MOS_6500 *cpu)
{
	cpu->last_abs_addr = mos6500_read_rom_addr(cpu);
	return 0;
}

uint8_t am6500_absx(MOS_6500 *cpu)
{
	uint16_t addr = mos6500_read_rom_addr(cpu) + cpu->regs.x;
	cpu->last_abs_addr = addr;

	if (HI16(cpu->last_abs_addr) != HI16(addr))
		return 1;
	else
		return 0;
}

uint8_t am6500_absy(MOS_6500 *cpu)
{
	uint16_t addr = mos6500_read_rom_addr(cpu) + cpu->regs.y;
	cpu->last_abs_addr = addr;

	return HI16(cpu->last_abs_addr) != HI16(addr) ? 1 : 0;
}

uint8_t am6500_imm(MOS_6500 *cpu)
{
	cpu->last_abs_addr = ++cpu->regs.c;
	return 0;
}

uint8_t am6500_imp(MOS_6500 *cpu)
{
	cpu->cache = cpu->regs.a;
	return 0;
}

uint8_t am6500_ind(MOS_6500 *cpu)
{
	uint16_t ptr = mos6500_read_rom_addr(cpu);

	if (ptr & 255)
		// emulate page boundary hardware bug
		cpu->last_abs_addr = bus6500_read_addr(cpu->bus, ptr);
	else
		// normal behavior
		cpu->last_abs_addr = bus6500_read_addr(cpu->bus, ptr + 1);

	return 0;
}

uint8_t am6500_indx(MOS_6500 *cpu)
{
	cpu->last_abs_addr = bus6500_read_addr(cpu->bus, (mos6500_read_rom(cpu) + cpu->regs.x) & 255);
	return 0;
}

uint8_t am6500_indy(MOS_6500 *cpu)
{
	uint16_t t = mos6500_read_rom(cpu);
	uint16_t lo = bus6500_read(cpu->bus, t & 255);
	uint16_t hi = bus6500_read(cpu->bus, (t + 1) & 255);

	cpu->last_abs_addr = (hi << 8) | lo + cpu->regs.y;

	return HI16(cpu->last_abs_addr) != hi << 8 ? 1 : 0;
}

uint8_t am6500_rel(MOS_6500 *cpu)
{
	cpu->last_rel_addr = mos6500_read_rom(cpu);

	// check for signed bit
	if (cpu->last_rel_addr & 128)
		cpu->last_rel_addr |= 65280;

	return 0;
}

uint8_t am6500_zp(MOS_6500 *cpu)
{
	cpu->last_abs_addr = mos6500_read_rom(cpu);
	return 0;
}

uint8_t am6500_zpx(MOS_6500 *cpu)
{
	cpu->last_rel_addr = (mos6500_read_rom(cpu) + cpu->regs.x) & 255;
	return 0;
}

uint8_t am6500_zpy(MOS_6500 *cpu)
{
	cpu->last_rel_addr = (mos6500_read_rom(cpu) + cpu->regs.y) & 255;
	return 0;
}


// branching

uint8_t op6500_bcc(MOS_6500 *cpu)
{
	if (!check_flag(cpu, P_C))
		branch(cpu);
	return 0;
}

uint8_t op6500_bcs(MOS_6500 *cpu)
{
	if (check_flag(cpu, P_C))
		branch(cpu);
	return 0;
}

uint8_t op6500_beq(MOS_6500 *cpu)
{
	if (check_flag(cpu, P_Z))
		branch(cpu);
	return 0;
}

uint8_t op6500_bmi(MOS_6500 *cpu)
{
	if (check_flag(cpu, P_N))
		branch(cpu);
	return 0;
}

uint8_t op6500_bne(MOS_6500 *cpu)
{
	if (!check_flag(cpu, P_Z))
		branch(cpu);
	return 0;
}

uint8_t op6500_bpl(MOS_6500 *cpu)
{
	if (!check_flag(cpu, P_N))
		branch(cpu);
	return 0;
}

uint8_t op6500_bvc(MOS_6500 *cpu)
{
	if (!check_flag(cpu, P_V))
		branch(cpu);
	return 0;
}

uint8_t op6500_bvs(MOS_6500 *cpu)
{
	if (check_flag(cpu, P_V))
		branch(cpu);
	return 0;
}


// state bit manipulation

uint8_t op6500_clc(MOS_6500 *cpu)
{
	set_flag(cpu, P_C, 0);
	return 0;
}

uint8_t op6500_cld(MOS_6500 *cpu)
{
	set_flag(cpu, P_D, 0);
	return 0;
}

uint8_t op6500_cli(MOS_6500 *cpu)
{
	set_flag(cpu, P_I, 0);
	return 0;
}

uint8_t op6500_clv(MOS_6500 *cpu)
{
	set_flag(cpu, P_V, 0);
	return 0;
}

uint8_t op6500_sec(MOS_6500 *cpu)
{
	set_flag(cpu, P_C, 1);
	return 0;
}

uint8_t op6500_sed(MOS_6500 *cpu)
{
	set_flag(cpu, P_D, 1);
	return 0;
}

uint8_t op6500_sei(MOS_6500 *cpu)
{
	set_flag(cpu, P_I, 1);
	return 0;
}


// interrupts

uint8_t op6500_brk(MOS_6500 *cpu)
{
	// differs slightly from interrupt

	cpu->regs.c++;

	set_flag(cpu, P_I, 1);
	mos6500_stack_write_addr(cpu, cpu->regs.c);

	set_flag(cpu, P_B, 1);
	mos6500_stack_write(cpu, cpu->regs.p);
	set_flag(cpu, P_B, 0);

	cpu->regs.c = bus6500_read_addr(cpu->bus, 65534);

	return 0;
}

void op6500_irq(MOS_6500 *cpu)
{
	if (!check_flag(cpu, P_I))
		interrupt(cpu, 65534, 7);
}

void op6500_nmi(MOS_6500 *cpu)
{
	interrupt(cpu, 65530, 8);
}

uint8_t op6500_rti(MOS_6500 *cpu)
{
	// restore flags
	cpu->regs.p = mos6500_stack_read(cpu);
	set_flag(cpu, P_B, 0);
	set_flag(cpu, P_U, 0);

	// restore counter
	cpu->regs.c = mos6500_stack_read_addr(cpu);

	return 0;
}

uint8_t op6500_rts(MOS_6500 *cpu)
{
	cpu->regs.c = mos6500_stack_read_addr(cpu);
	return 0;
}


// pushing/popping

uint8_t op6500_pha(MOS_6500 *cpu)
{
	mos6500_stack_write(cpu, cpu->regs.a);
	return 0;
}

uint8_t op6500_php(MOS_6500 *cpu)
{
	set_flag(cpu, P_B, 1);
	set_flag(cpu, P_U, 1);
	mos6500_stack_write(cpu, cpu->regs.p);
	set_flag(cpu, P_B, 0);
	set_flag(cpu, P_U, 0);
	return 0;
}

uint8_t op6500_pla(MOS_6500 *cpu)
{
	cpu->regs.a = mos6500_stack_read(cpu);
	flags_nz(cpu, cpu->regs.a);

	return 0;
}

uint8_t op6500_plp(MOS_6500 *cpu)
{
	mos6500_stack_read(cpu);
	set_flag(cpu, P_U, 1);
	return 0;
}


// arith

uint8_t op6500_adc(MOS_6500 *cpu)
{
	uint16_t tmp = cpu->regs.a + mos6500_fetch(cpu) + check_flag(cpu, P_C);

	flags_cnz(cpu, tmp);
	set_flag(cpu, P_V, ~((cpu->regs.a ^ cpu->cache) & (cpu->regs.a ^ tmp) & 128));

	cpu->regs.a = tmp & 255;

	return 1;
}

uint8_t op6500_dec(MOS_6500 *cpu)
{
	uint8_t tmp = mos6500_fetch(cpu) - 1;
	mos6500_write_last(cpu, tmp);
	flags_nz(cpu, cpu->regs.x);

	return 0;
}

uint8_t op6500_dex(MOS_6500 *cpu)
{
	flags_nz(cpu, --cpu->regs.x);
	return 0;
}

uint8_t op6500_dey(MOS_6500 *cpu)
{
	flags_nz(cpu, --cpu->regs.y);
	return 0;
}

uint8_t op6500_inc(MOS_6500 *cpu)
{
	uint8_t tmp = mos6500_fetch(cpu) + 1;
	mos6500_write_last(cpu, tmp);
	flags_nz(cpu, tmp);

	return 0;
}

uint8_t op6500_inx(MOS_6500 *cpu)
{
	flags_nz(cpu, ++cpu->regs.x);
	return 0;
}

uint8_t op6500_iny(MOS_6500 *cpu)
{
	flags_nz(cpu, ++cpu->regs.y);
	return 0;
}

uint8_t op6500_sbc(MOS_6500 *cpu)
{
	uint16_t value = mos6500_fetch(cpu) ^ 255; // invert the value
	uint16_t tmp = cpu->regs.a + value + check_flag(cpu, P_C);

	flags_cnz(cpu, tmp);

	set_flag(cpu, P_V, (tmp ^ cpu->regs.a) & ((tmp ^ value) & 128));
	cpu->regs.a = tmp & 255;

	return 1;
}


// bitwise

uint8_t op6500_and(MOS_6500 *cpu)
{
	cpu->regs.a &= mos6500_fetch(cpu);
	flags_nz(cpu, cpu->regs.a);

	return 1;
}

uint8_t op6500_asl(MOS_6500 *cpu)
{
	uint16_t tmp = mos6500_fetch(cpu) << 1;

	flags_cnz(cpu, tmp);
	check_mode(cpu, tmp & 255);

	return 0;
}

uint8_t op6500_eor(MOS_6500 *cpu)
{
	cpu->regs.a ^= mos6500_fetch(cpu);
	flags_nz(cpu, cpu->regs.a);

	return 1;
}

uint8_t op6500_lsr(MOS_6500 *cpu)
{
	set_flag(cpu, P_C, mos6500_fetch(cpu));

	uint16_t tmp = cpu->cache >> 1;

	flags_nz(cpu, tmp);
	check_mode(cpu, tmp & 255);

	return 0;
}

uint8_t op6500_ora(MOS_6500 *cpu)
{
	cpu->regs.a |= mos6500_fetch(cpu);
	flags_nz(cpu, cpu->regs.a);

	return 1;
}

uint8_t op6500_rol(MOS_6500 *cpu)
{
	uint16_t tmp = (mos6500_fetch(cpu) << 1) | check_flag(cpu, P_C);

	flags_cnz(cpu, tmp);
	check_mode(cpu, tmp & 255);

	return 0;
}

uint8_t op6500_ror(MOS_6500 *cpu)
{
	mos6500_fetch(cpu);
	uint16_t tmp = (cpu->cache << 7) | (cpu->cache >> 1);

	set_flag(cpu, P_C, cpu->cache & 1);
	flags_nz(cpu, tmp);
	check_mode(cpu, tmp & 255);

	return 0;
}


// comparison

uint8_t op6500_bit(MOS_6500 *cpu)
{
	uint16_t tmp = cpu->regs.a & mos6500_fetch(cpu);

	set_flag(cpu, P_Z, tmp & 255 == 0);
	set_flag(cpu, P_N, cpu->cache & 128);
	set_flag(cpu, P_V, cpu->cache & 64);

	return 0;
}

uint8_t op6500_cmp(MOS_6500 *cpu)
{
	uint16_t tmp = cpu->regs.a - mos6500_fetch(cpu);

	set_flag(cpu, P_C, cpu->regs.a >= cpu->cache);
	flags_nz(cpu, tmp);

	return 1;
}

uint8_t op6500_cpx(MOS_6500 *cpu)
{
	uint16_t tmp = cpu->regs.x - mos6500_fetch(cpu);

	set_flag(cpu, P_C, cpu->regs.x >= cpu->cache);
	flags_nz(cpu, tmp);

	return 1;
}

uint8_t op6500_cpy(MOS_6500 *cpu)
{
	uint16_t tmp = cpu->regs.y - mos6500_fetch(cpu);

	set_flag(cpu, P_C, cpu->regs.y >= cpu->cache);
	flags_nz(cpu, tmp);

	return 1;
}


// jumping

uint8_t op6500_jmp(MOS_6500 *cpu)
{
	cpu->regs.c = cpu->last_abs_addr;
	return 0;
}

uint8_t op6500_jsr(MOS_6500 *cpu)
{
	mos6500_stack_write_addr(cpu, cpu->regs.c);
	cpu->regs.c = cpu->last_abs_addr;
	return 0;
}


// loading

uint8_t op6500_lda(MOS_6500 *cpu)
{
	cpu->regs.a = mos6500_fetch(cpu);
	flags_nz(cpu, cpu->regs.a);
	return 1;
}

uint8_t op6500_ldx(MOS_6500 *cpu)
{
	cpu->regs.x = mos6500_fetch(cpu);
	flags_nz(cpu, cpu->regs.x);
	return 1;
}

uint8_t op6500_ldy(MOS_6500 *cpu)
{
	cpu->regs.y = mos6500_fetch(cpu);
	flags_nz(cpu, cpu->regs.y);
	return 1;
}


// storing

uint8_t op6500_sta(MOS_6500 *cpu)
{
	mos6500_write_last(cpu, cpu->regs.a);
	return 0;
}

uint8_t op6500_stx(MOS_6500 *cpu)
{
	mos6500_write_last(cpu, cpu->regs.x);
	return 0;
}

uint8_t op6500_sty(MOS_6500 *cpu)
{
	mos6500_write_last(cpu, cpu->regs.y);
	return 0;
}


// transferring

uint8_t op6500_tax(MOS_6500 *cpu)
{
	cpu->regs.x = cpu->regs.a;
	flags_nz(cpu, cpu->regs.x);
	return 0;
}

uint8_t op6500_tay(MOS_6500 *cpu)
{
	cpu->regs.y = cpu->regs.a;
	flags_nz(cpu, cpu->regs.y);
	return 0;
}

uint8_t op6500_tsx(MOS_6500 *cpu)
{
	cpu->regs.x = cpu->regs.s;
	flags_nz(cpu, cpu->regs.x);
	return 0;
}

uint8_t op6500_txa(MOS_6500 *cpu)
{
	cpu->regs.a = cpu->regs.x;
	flags_nz(cpu, cpu->regs.a);
	return 0;
}

uint8_t op6500_txs(MOS_6500 *cpu)
{
	cpu->regs.s = cpu->regs.x;
	return 0;
}

uint8_t op6500_tya(MOS_6500 *cpu)
{
	cpu->regs.a = cpu->regs.y;
	flags_nz(cpu, cpu->regs.a);
	return 0;
}


// misc

uint8_t op6500_nop(MOS_6500 *cpu)
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
