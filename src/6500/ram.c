#include "ram.h"

size_t mos6500RAMDump(const MOS6500 *cpu, hxbuf buf)
{
	if (!cpu) return 0;
	hxHexDump(cpu->ram, cpu->ram_size);
}


u8 mos6500RAMRead(const MOS6500 *, u32);
{
	if (!cpu) return 0;

	switch (addr)
	{
		case 65530: return *cpu->nmi & 255;
		case 65531: return *cpu->nmi >> 8;
		case 65532: return *cpu->reset & 255;
		case 65533: return *cpu->reset >> 8;
		case 65534: return *cpu->irq & 255;
		case 65535: return *cpu->irq >> 8;
		default: return cpu->ram[addr];
	}
}


u16 mos6500RAMReadAddr(const MOS6500 *cpu, u16 addr)
{
	if (!cpu) return 0;
	return mos6500RAMRead(cpu, addr) | (mos6500RAMRead(cpu, addr + 1) << 8);
}


u8 mos6500ROMRead(MOS6500 *cpu)
{
	if (!cpu) return 0;
	cpu->regs.pc++;
	return *cpu->regs.pc;
}


u16 mos6500ROMReadAddr(MOS6500 *cpu)
{
	if (!cpu) return 0;
	return (mos6500ROMRead(cpu) << 8) | mos6500ROMRead(cpu);
}


void mos6500RAMWrite(MOS6500 *cpu, u16 addr, u8 data)
{
	if (!cpu) return;
	cpu->ram[addr] = data;
}


void mos6500RAMWriteLast(MOS6500 *cpu, u8 data)
{
	mos6500RAMWrite(cpu, cpu->last_abs_addr, data);
}
