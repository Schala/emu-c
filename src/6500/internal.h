#ifndef _6500_INTERNAL_H
#define _6500_INTERNAL_H

#include "cpu.h"

#define P_C 1
#define P_Z 2
#define P_I 3
#define P_D 8
#define P_B 16
#define P_U 32
#define P_V 64
#define P_N 128

u8 mos6500CheckFlag(MOS6500State state, u8 flag)
{
	return cpu->regs.p & flag;
}

void mos6500SetFlag(MOS6500State state, u8 flag, int cond)
{
	if (cond)
		cpu->regs.p |= flag;
	else
		cpu->regs.p &= ~flag;
}

#endif // _6500_INTERNAL_H
