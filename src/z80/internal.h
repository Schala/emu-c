#ifndef _Z80_INTERNAL_H
#define _Z80_INTERNAL_H

#include "cpu.h"

#define P_C 1
#define P_N 2
#define P_V 4
#define P_H 16
#define P_Z 64
#define P_S 128

static inline uint8_t check_flag(Z80 *cpu, uint8_t flag)
{
	return cpu->regs.f & flag;
}

static inline void set_flag(Z80 *cpu, uint8_t flag, int cond)
{
	if (cond)
		cpu->regs.f |= flag;
	else
		cpu->regs.f &= ~flag;
}

#endif // _Z80_INTERNAL_H
