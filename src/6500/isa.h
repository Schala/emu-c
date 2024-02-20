#ifndef _6500_ISA_H
#define _6500_ISA_H

#include "cpu.h"

// Moved from cpu.h/.c to reduce bloat regarding the internal opcodes table

// CPU clock operation (execute one instruction)
void mos6500Clock(MOS6500 *cpucpu);

// Fetch and cache a byte from the cached absolute address
u8 mos6500Fetch(MOS6500 *cpucpu);

// Read address from RAM
static inline uint16_t mos6500FetchAddr(MOS6500 *cpucpu)
{
	return mos6500Fetch(cpu) | (mos6500Fetch(cpu) << 8);
}


// None of these should be inlined, as we need them to be addressable.
// All except interrupts return the number of additional cycles possibly needed.


// address modes

// Absolute address mode
u8 mos6500Absolute(MOS6500 *cpu);

// Absolute address mode with X register offset
u8 mos6500AbsoluteX(MOS6500 *cpu);

// Absolute address mode with Y register offset
u8 mos6500AbsoluteY(MOS6500 *cpu);

// Immediate address mode
u8 mos6500Immediate(MOS6500 *cpu);

// Implied address mode
u8 mos6500Implied(MOS6500 *cpu);

// Indirect address mode
u8 mos6500Indirect(MOS6500 *cpu);

// Indirect address mode of zero-page with X register offset
u8 mos6500IndirectX(MOS6500 *cpu);

// Indirect address mode of zero-page with Y register offset
u8 mos6500IndirectY(MOS6500 *cpu);

// Relative address mode (branching)
u8 mos6500Relative(MOS6500 *cpu);

// Zero-page address mode
u8 mos6500ZeroPage(MOS6500 *cpu);

// Zero-page address mode with X register offset
u8 mos6500ZeroPageX(MOS6500 *cpu);

// Zero-page address mode with Y register offset
u8 mos6500ZeroPageY(MOS6500 *cpu);


// branching

// Branching if carry bit clear
u8 mos6500BCC(MOS6500 *cpu);

// Branching if carry bit set
u8 mos6500BCS(MOS6500 *cpu);

// Branching if equal
u8 mos6500BEQ(MOS6500 *cpu);

// Branching if negative
u8 mos6500BMI(MOS6500 *cpu);

// Branching if not equal
u8 mos6500BNE(MOS6500 *cpu);

// Branching if positive
u8 mos6500BPL(MOS6500 *cpu);

// Branching if overflow bit clear
u8 mos6500BVC(MOS6500 *cpu);

// Branching if overflow bit set
u8 mos6500BVS(MOS6500 *cpu);


// status bit manipulation

// Clear carry bit
u8 mos6500CLC(MOS6500 *cpu);

// Clear decimal mode bit
u8 mos6500CLD(MOS6500 *cpu);

// Clear disable interrupts bit
u8 mos6500CLI(MOS6500 *cpu);

// Clear overflow bit
u8 mos6500CLV(MOS6500 *cpu);

// Set carry bit
u8 mos6500SEC(MOS6500 *cpu);

// Set decimal mode bit
u8 mos6500SED(MOS6500 *cpu);

// Set disable interrupts bit
u8 mos6500SEI(MOS6500 *cpu);


// interrupts

// Program-sourced interrupt
u8 mos6500BRK(MOS6500 *cpu);

// Interrupt request
void mos6500IRQ(MOS6500 *cpu);

// Non-maskable interrupt
void mos6500NMI(MOS6500 *cpu);

// Return from interrupt
u8 mos6500RTI(MOS6500 *cpu);

// Return from subroutine
u8 mos6500RTS(MOS6500 *cpu);


// pushing/popping

// Push accumulator to stack
u8 mos6500PHA(MOS6500 *cpu);

// Push status to stack
u8 mos6500PHP(MOS6500 *cpu);

// Pop accumulator from stack
u8 mos6500PLA(MOS6500 *cpu);

// Pop status from stack
u8 mos6500PLP(MOS6500 *cpu);


// arithmetic

// Add with carry
u8 mos6500ADC(MOS6500 *cpu);

// Decrement value at location
u8 mos6500DEC(MOS6500 *cpu);

// Decrement X register
u8 mos6500DEX(MOS6500 *cpu);

// Decrement Y register
u8 mos6500DEY(MOS6500 *cpu);

// Increment value at location
u8 mos6500INC(MOS6500 *cpu);

// Increment X register
u8 mos6500INX(MOS6500 *cpu);

// Increment Y register
u8 mos6500INY(MOS6500 *cpu);

// Subtract with carry
u8 mos6500SBC(MOS6500 *cpu);


// bitwise

// Bitwise AND
u8 mos6500AND(MOS6500 *cpu);

// Arithmetical shift left
u8 mos6500ASL(MOS6500 *cpu);

// Logical shift right
u8 mos6500LSR(MOS6500 *cpu);

// Exclusive OR
u8 mos6500EOR(MOS6500 *cpu);

// Bitwise OR
u8 mos6500ORA(MOS6500 *cpu);

// Rotate left
u8 mos6500ROL(MOS6500 *cpu);

// Rotate right
u8 mos6500ROR(MOS6500 *cpu);


// comparison

// bit test
u8 mos6500BIT(MOS6500 *cpu);

// Compare accumulator
u8 mos6500CMP(MOS6500 *cpu);

// Compare X register
u8 mos6500CPX(MOS6500 *cpu);

// Compare Y register
u8 mos6500CPY(MOS6500 *cpu);


// jumping

// jump to location
u8 mos6500JMP(MOS6500 *cpu);

// jump to subroutine
u8 mos6500JSR(MOS6500 *cpu);


// loading

// load accumulator
u8 mos6500LDA(MOS6500 *cpu);

// load X register
u8 mos6500LDX(MOS6500 *cpu);

// load Y register
u8 mos6500LDY(MOS6500 *cpu);


// storing

// store accumulator at address
u8 mos6500STA(MOS6500 *cpu);

// store X register at address
u8 mos6500STX(MOS6500 *cpu);

// store Y register at address
u8 mos6500STY(MOS6500 *cpu);


// transferring

// transfer accumulator to X register
u8 mos6500TAX(MOS6500 *cpu);

// transfer accumulator to Y register
u8 mos6500TAY(MOS6500 *cpu);

// transfer stack pointer to X register
u8 mos6500TSX(MOS6500 *cpu);

// transfer X register to accumulator
u8 mos6500TXA(MOS6500 *cpu);

// transfer X register to stack pointer
u8 mos6500TXS(MOS6500 *cpu);

// transfer Y register to accumulator
u8 mos6500TYA(MOS6500 *cpu);


// no operation
u8 mos6500NOP(MOS6500 *cpu);

#endif // _6500_ISA_H
