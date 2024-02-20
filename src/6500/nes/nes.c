#include <stdlib.h>

#include "../../common/util.h"
#include "../isa.h"
#include "ppu.h"
#include "rom.h"
#include "nes.h"

NES * nes_alloc(SDL_Renderer *renderer)
{
	NES *nes = (NES *)calloc(1, sizeof(NES));
	nes->bus = bus6500_alloc(65535);
	nes_cpu_alloc(nes);
	ppu2c02_alloc(nes, renderer);

	return nes;
}

void nes_clock(NES *nes)
{
	if (!nes) return;

	ppu2c02_clock(nes->ppu);

	// CPU runs 3 times slower than PPU
	if (nes->cycles++ % 3 == 0)
		mos6500_clock(nes->cpu);
}


void nes_cpu_alloc(NES *nes)
{
	if (!nes) return;

	nes-> cpu = mos6500_alloc(nes->bus, 0, 0);
}

void nes_free(NES *nes)
{
	if (!nes) return;

	if (nes->rom) nes_rom_free(nes); // free ROM before anything else
	if (nes->cpu) mos6500_free(nes->cpu);
	if (nes->ppu) ppu2c02_free(nes);
	if (nes->bus) bus6500_free(nes->bus);

	free(nes);
	nes = NULL;
}

/*void nes_print_chr(const NES *nes)
{
	hexdump(nes->rom->chr, nes->map->chr_size * nes->rom->header.chr_pages);
}

void nes_print_prg(const NES *nes)
{
	hexdump(nes->rom->prg, nes->map->prg_size * nes->rom->header.prg_pages);
}*/

uint8_t nes_read(const NES *nes, uint16_t addr)
{
	if (addr <= 8191)
		return bus6500_read(nes->bus, addr & 2047);
	else if (addr >= 8192 && addr <= 16383)
		return bus6500_read(nes->bus, addr & 8199);
	else
		bus6500_read(nes->bus, addr);
}

void nes_reset(NES *nes)
{
	if (!nes) return;

	mos6500_reset(nes->cpu);
	nes->cycles = 0;
}

void nes_write(NES *nes, uint16_t addr, uint8_t data)
{
	if (addr <= 8191)
		bus6500_write(nes->bus, addr & 2047, data);
	else if (addr >= 8192 && addr <= 16383)
		bus6500_write(nes->bus, addr & 8199, data);
	else
		bus6500_write(nes->bus, addr, data);
}
