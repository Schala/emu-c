#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "isa.h"

MOS6500 * mos6500Alloc()
{
	MOS6500 *cpu = (MOS6500 *)hxalloc(sizeof(MOS6500));
	cpu->frame_index = MOS6500_FRAME_LEN - 1;

	return cpu;
}


void mos6500Free(MOS6500 *cpu)
{
	hxfree(cpu);
}


void mos6500DisasmDump(const MOS6500 *cpu)
{
	if (!cpu) return;

	for (size_t i = MOS6500_FRAME_LEN - 1; i >= 0; i--)
	{
		u16 dummy = cpu->frame[i];
		MOS6500Disasm *disasm = disasm6500_op(cpu->bus->ram, &dummy);

		printf("%04X: %s %s\n", disasm->addr, disasm->lhs, disasm->rhs);
		disasm6500_free(disasm);
	}
}


hxstr mos6500RegsDump(const MOS6500 *cpu)
{
	if (!cpu) return NULL;

	printf("A: %u\tX: %u\tY: %u\n", cpu->regs.a, cpu->regs.x, cpu->regs.y);
	printf("SP: $%02X\tPC: $%04X\n", cpu->regs.sp, cpu->regs.pc);

	printf("FLAGS: ");
	if (mos6500CheckFlag(cpu, P_C)) printf("C"); else printf("x");
	if (mos6500CheckFlag(cpu, P_Z)) printf("Z"); else printf("x");
	if (mos6500CheckFlag(cpu, P_I)) printf("I"); else printf("x");
	if (mos6500CheckFlag(cpu, P_D)) printf("D"); else printf("x");
	if (mos6500CheckFlag(cpu, P_B)) printf("B"); else printf("x");
	if (mos6500CheckFlag(cpu, P_U)) printf("U"); else printf("x");
	if (mos6500CheckFlag(cpu, P_V)) printf("V"); else printf("x");
	if (mos6500CheckFlag(cpu, P_N)) printf("N\n"); else printf("x\n");
}


hxstr mos6500StackDump(const MOS6500 *cpu)
{
	if (!cpu) return NULL;
	hxHexDump(cpu->stack, 255);
}


void mos6500Reset(MOS6500 *cpu)
{
	if (!cpu) return;

	cpu->regs.a = cpu->regs.x = cpu->regs.y = 0;

	cpu->regs.p = 0;
	set_flag(cpu, P_U, 1);

	cpu->regs.sp = 255;
	cpu->abs = cpu->reset;
	cpu->regs.pc = mos6500FetchAddr(cpu);

	cpu->rel = 0;
	cpu->abs = 0;
	cpu->cache = 0;
	cpu->cycles = 8;
}


void mos6500Branch(MOS6500 *cpu)
{
	if (!cpu) return;

	cpu->cycles++;
	cpu->last_abs_addr = cpu->regs.c + cpu->last_rel_addr;

	// need an additional cycle if different page
	if (_HI16(cpu->last_abs_addr) != _HI16(cpu->regs.c))
		cpu->cycles++;

	// jump to address
	cpu->regs.c = cpu->last_abs_addr;
}


u8 mos6500StackRead(MOS6500 *cpu)
{
	if (!cpu) return 0;
	return cpu->stack[++cpu->regs.sp];
}


u16 mos6500StackReadAddr(MOS6500 *cpu)
{
	if (!cpu) return 0;
	return mos6500StackRead(cpu) | (mos6500StackRead(cpu) << 8);
}


void mos6500StackWrite(MOS6500 *cpu, u8 data)
{
	if (!cpu) return;
	cpu->stack[cpu->regs.sp--] = data;
}


void mos6500StackWriteAddr(MOS6500 *cpu, u16 addr)
{
	if (!cpu) return;
	mos6500StackWrite(cpu, (addr >> 8) & 255);
	mos6500StackWrite(cpu, addr & 255);
}
