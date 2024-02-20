#include <stdio.h>
#include <stdlib.h>

#include "../common/util.h"
#include "cpu.h"
#include "internal.h"
#include "isa.h"

MOS_6500 * mos6500_alloc(BUS_6500 *bus, uint16_t start, uint16_t end)
{
	MOS_6500 *cpu = (MOS_6500 *)calloc(1, sizeof(MOS_6500));
	cpu->bus = bus;
	set_flag(cpu, P_U, 1);
	cpu->regs.s = 253;
	cpu->stack_start = &bus->ram[256];
	cpu->frame_index = MOS6500_FRAME_LEN - 1;

	cpu->node = bus6500_add_device(bus, cpu, start, end + 1, DF_OWNS_RAM);
	mos6500_reset(cpu);

	return cpu;
}

void mos6500_free(MOS_6500 *cpu)
{
	if (!cpu) return;

	bus6500_free_device(cpu->bus, cpu);
	free(cpu);
	cpu = NULL;
}

void mos6500_print_disasm(const MOS_6500 *cpu)
{
	if (!cpu) return;

	for (int8_t i = MOS6500_FRAME_LEN - 1; i >= 0; i--)
	{
		uint16_t dummy = cpu->frame[i];
		DISASM_6500 *disasm = disasm6500_op(cpu->bus->ram, &dummy);

		printf("%04X: %s %s\n", disasm->addr, disasm->lhs, disasm->rhs);
		disasm6500_free(disasm);
	}
}

void mos6500_print_regs(const MOS_6500 *cpu)
{
	if (!cpu) return;

	printf("A: %u\tX: %u\tY: %u\n", cpu->regs.a, cpu->regs.x, cpu->regs.y);
	printf("S: $%04X\tPC: $%04X\n", cpu->regs.s, cpu->regs.c);

	printf("FLAGS: ");
	if (check_flag(cpu, P_C)) printf("C"); else printf("x");
	if (check_flag(cpu, P_Z)) printf("Z"); else printf("x");
	if (check_flag(cpu, P_I)) printf("I"); else printf("x");
	if (check_flag(cpu, P_D)) printf("D"); else printf("x");
	if (check_flag(cpu, P_B)) printf("B"); else printf("x");
	if (check_flag(cpu, P_U)) printf("U"); else printf("x");
	if (check_flag(cpu, P_V)) printf("V"); else printf("x");
	if (check_flag(cpu, P_N)) printf("N\n"); else printf("x\n");
}

void mos6500_print_stack(const MOS_6500 *cpu)
{
	hexdump(&cpu->bus->ram[256], 255);
}

void mos6500_reset(MOS_6500 *cpu)
{
	if (!cpu) return;

	cpu->regs.a = cpu->regs.x = cpu->regs.y = 0;

	cpu->regs.p = 0;
	set_flag(cpu, P_U, 1);

	cpu->regs.s = 253;

	cpu->last_abs_addr = 65532;
	cpu->regs.c = mos6500_fetch_addr(cpu);

	cpu->last_rel_addr = 0;
	cpu->last_abs_addr = 0;
	cpu->cache = 0;
	cpu->cycles = 8;
}

void mos6500_map(MOS_6500 *cpu, uint16_t start, uint16_t end)
{
	if (!cpu) return;

	cpu->node->ram_offset = start;
	cpu->node->ram_size = end + 1;
}
