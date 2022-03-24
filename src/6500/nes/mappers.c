#include "mappers.h"

int nes_prg000(NES_ROM *rom, uint16_t addr, uint32_t *mapped_addr)
{
	if (addr >= 32768 && addr <= 65535)
	{
		// Mirror the ROM read based on the ROM file offset, pending 16kb or 32kb
		*mapped_addr = addr & rom->header.prg_pages > 1 ? 32767 : 16383;
		return 1;
	}

	return 0;
}

int nes_chr000(NES_ROM *rom, uint16_t addr, uint32_t *mapped_addr)
{
	if (addr >= 0 && addr <= 8191)
	{
		// Mirror the ROM read based on the ROM file offset
		*mapped_addr = addr & 8191;
		return 1;
	}

	return 0;
}
