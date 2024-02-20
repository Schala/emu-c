#ifndef _6500_INTERNAL_H
#define _6500_INTERNAL_H

#include "cpu.h"

#define P_C 1
#define P_Z 2
#define P_I 4
#define P_D 8
#define P_B 16
#define P_U 32
#define P_V 64
#define P_N 128

static inline uint8_t check_flag(const MOS_6500 *cpu, uint8_t flag)
{
	return cpu->regs.p & flag;
}

static inline void set_flag(MOS_6500 *cpu, uint8_t flag, int cond)
{
	if (cond)
		cpu->regs.p |= flag;
	else
		cpu->regs.p &= ~flag;
}

#endif // _6500_INTERNAL_H
