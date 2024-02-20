#ifndef _6500_ISA_H
#define _6500_ISA_H

#include "cpu.h"

// Moved from cpu.h/.c to reduce bloat regarding the internal opcodes table

// CPU clock operation (execute one instruction)
void mos6500_clock(MOS_6500 *);

// Fetch and cache a byte from the cached absolute address
uint8_t mos6500_fetch(MOS_6500 *);

// Read address from RAM
static inline uint16_t mos6500_fetch_addr(MOS_6500 *cpu)
{
	return mos6500_fetch(cpu) | (mos6500_fetch(cpu) << 8);
}


// None of these should be inlined, as we need them to be addressable.
// All except interrupts return the number of additional cycles possibly needed.


// address modes

// Absolute address mode
uint8_t am6500_abs(MOS_6500 *);

// Absolute address mode with X register offset
uint8_t am6500_absx(MOS_6500 *);

// Absolute address mode with Y register offset
uint8_t am6500_absy(MOS_6500 *);

// Immediate address mode
uint8_t am6500_imm(MOS_6500 *);

// Implied address mode
uint8_t am6500_imp(MOS_6500 *);

// Indirect address mode
uint8_t am6500_ind(MOS_6500 *);

// Indirect address mode of zero-page with X register offset
uint8_t am6500_indx(MOS_6500 *);

// Indirect address mode of zero-page with Y register offset
uint8_t am6500_indy(MOS_6500 *);

// Relative address mode (branching)
uint8_t am6500_rel(MOS_6500 *);

// Zero-page address mode
uint8_t am6500_zp(MOS_6500 *);

// Zero-page address mode with X register offset
uint8_t am6500_zpx(MOS_6500 *);

// Zero-page address mode with Y register offset
uint8_t am6500_zpy(MOS_6500 *);


// branching

// Branching if carry bit clear
uint8_t op6500_bcc(MOS_6500 *);

// Branching if carry bit set
uint8_t op6500_bcs(MOS_6500 *);

// Branching if equal
uint8_t op6500_beq(MOS_6500 *);

// Branching if negative
uint8_t op6500_bmi(MOS_6500 *);

// Branching if not equal
uint8_t op6500_bne(MOS_6500 *);

// Branching if positive
uint8_t op6500_bpl(MOS_6500 *);

// Branching if overflow bit clear
uint8_t op6500_bvc(MOS_6500 *);

// Branching if overflow bit set
uint8_t op6500_bvs(MOS_6500 *);


// status bit manipulation

// Clear carry bit
uint8_t op6500_clc(MOS_6500 *);

// Clear decimal mode bit
uint8_t op6500_cld(MOS_6500 *);

// Clear disable interrupts bit
uint8_t op6500_cli(MOS_6500 *);

// Clear overflow bit
uint8_t op6500_clv(MOS_6500 *);

// Set carry bit
uint8_t op6500_sec(MOS_6500 *);

// Set decimal mode bit
uint8_t op6500_sed(MOS_6500 *);

// Set disable interrupts bit
uint8_t op6500_sei(MOS_6500 *);


// interrupts

// Program-sourced interrupt
uint8_t op6500_brk(MOS_6500 *);

// Interrupt request
void op6500_irq(MOS_6500 *);

// Non-maskable interrupt
void op6500_nmi(MOS_6500 *);

// Return from interrupt
uint8_t op6500_rti(MOS_6500 *);

// Return from subroutine
uint8_t op6500_rts(MOS_6500 *);


// pushing/popping

// Push accumulator to stack
uint8_t op6500_pha(MOS_6500 *);

// Push status to stack
uint8_t op6500_php(MOS_6500 *);

// Pop accumulator from stack
uint8_t op6500_pla(MOS_6500 *);

// Pop status from stack
uint8_t op6500_plp(MOS_6500 *);


// arithmetic

// Add with carry
uint8_t op6500_adc(MOS_6500 *);

// Decrement value at location
uint8_t op6500_dec(MOS_6500 *);

// Decrement X register
uint8_t op6500_dex(MOS_6500 *);

// Decrement Y register
uint8_t op6500_dey(MOS_6500 *);

// Increment value at location
uint8_t op6500_inc(MOS_6500 *);

// Increment X register
uint8_t op6500_inx(MOS_6500 *);

// Increment Y register
uint8_t op6500_iny(MOS_6500 *);

// Subtract with carry
uint8_t op6500_sbc(MOS_6500 *);


// bitwise

// Bitwise AND
uint8_t op6500_and(MOS_6500 *);

// Arithmetical shift left
uint8_t op6500_asl(MOS_6500 *);

// Logical shift right
uint8_t op6500_lsr(MOS_6500 *);

// Exclusive OR
uint8_t op6500_eor(MOS_6500 *);

// Bitwise OR
uint8_t op6500_ora(MOS_6500 *);

// Rotate left
uint8_t op6500_rol(MOS_6500 *);

// Rotate right
uint8_t op6500_ror(MOS_6500 *);


// comparison

// bit test
uint8_t op6500_bit(MOS_6500 *);

// Compare accumulator
uint8_t op6500_cmp(MOS_6500 *);

// Compare X register
uint8_t op6500_cpx(MOS_6500 *);

// Compare Y register
uint8_t op6500_cpy(MOS_6500 *);


// jumping

// jump to location
uint8_t op6500_jmp(MOS_6500 *);

// jump to subroutine
uint8_t op6500_jsr(MOS_6500 *);


// loading

// load accumulator
uint8_t op6500_lda(MOS_6500 *);

// load X register
uint8_t op6500_ldx(MOS_6500 *);

// load Y register
uint8_t op6500_ldy(MOS_6500 *);


// storing

// store accumulator at address
uint8_t op6500_sta(MOS_6500 *);

// store X register at address
uint8_t op6500_stx(MOS_6500 *);

// store Y register at address
uint8_t op6500_sty(MOS_6500 *);


// transferring

// transfer accumulator to X register
uint8_t op6500_tax(MOS_6500 *);

// transfer accumulator to Y register
uint8_t op6500_tay(MOS_6500 *);

// transfer stack pointer to X register
uint8_t op6500_tsx(MOS_6500 *);

// transfer X register to accumulator
uint8_t op6500_txa(MOS_6500 *);

// transfer X register to stack pointer
uint8_t op6500_txs(MOS_6500 *);

// transfer Y register to accumulator
uint8_t op6500_tya(MOS_6500 *);


// no operation
uint8_t op6500_nop(MOS_6500 *);

#endif // _6500_ISA_H
