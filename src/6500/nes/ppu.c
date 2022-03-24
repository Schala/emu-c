#include "internal.h"
#include "nes.h"
#include "ppu.h"
#include "rom.h"

static const SDL_Color PALETTE[] =
{
	{ 84, 84, 84, SDL_ALPHA_OPAQUE },
	{ 0, 30, 116, SDL_ALPHA_OPAQUE },
	{ 8, 16, 144, SDL_ALPHA_OPAQUE },
	{ 48, 0, 136, SDL_ALPHA_OPAQUE },
	{ 68, 0, 100, SDL_ALPHA_OPAQUE },
	{ 92, 0, 48, SDL_ALPHA_OPAQUE },
	{ 84, 4, 0, SDL_ALPHA_OPAQUE },
	{ 60, 24, 0, SDL_ALPHA_OPAQUE },
	{ 32, 42, 0, SDL_ALPHA_OPAQUE },
	{ 8, 58, 0, SDL_ALPHA_OPAQUE },
	{ 0, 64, 0, SDL_ALPHA_OPAQUE },
	{ 0, 60, 0, SDL_ALPHA_OPAQUE },
	{ 0, 50, 60, SDL_ALPHA_OPAQUE },
	{ 0, 0, 0, SDL_ALPHA_OPAQUE },
	{ 0, 0, 0, SDL_ALPHA_OPAQUE },
	{ 0, 0, 0, SDL_ALPHA_OPAQUE },

	{ 152, 150, 152, SDL_ALPHA_OPAQUE },
	{ 8, 76, 196, SDL_ALPHA_OPAQUE },
	{ 48, 50, 236, SDL_ALPHA_OPAQUE },
	{ 92, 30, 228, SDL_ALPHA_OPAQUE },
	{ 136, 20, 176, SDL_ALPHA_OPAQUE },
	{ 160, 20, 100, SDL_ALPHA_OPAQUE },
	{ 152, 34, 32, SDL_ALPHA_OPAQUE },
	{ 120, 60, 0, SDL_ALPHA_OPAQUE },
	{ 84, 90, 0, SDL_ALPHA_OPAQUE },
	{ 40, 114, 0, SDL_ALPHA_OPAQUE },
	{ 8, 124, 0, SDL_ALPHA_OPAQUE },
	{ 0, 118, 40, SDL_ALPHA_OPAQUE },
	{ 0, 102, 120, SDL_ALPHA_OPAQUE },
	{ 0, 0, 0, SDL_ALPHA_OPAQUE },
	{ 0, 0, 0, SDL_ALPHA_OPAQUE },
	{ 0, 0, 0, SDL_ALPHA_OPAQUE },

	{ 236, 238, 236, SDL_ALPHA_OPAQUE },
	{ 76, 154, 236, SDL_ALPHA_OPAQUE },
	{ 120, 124, 236, SDL_ALPHA_OPAQUE },
	{ 176, 98, 236, SDL_ALPHA_OPAQUE },
	{ 228, 84, 236, SDL_ALPHA_OPAQUE },
	{ 236, 88, 180, SDL_ALPHA_OPAQUE },
	{ 236, 106, 100, SDL_ALPHA_OPAQUE },
	{ 212, 136, 32, SDL_ALPHA_OPAQUE },
	{ 160, 170, 0, SDL_ALPHA_OPAQUE },
	{ 116, 196, 0, SDL_ALPHA_OPAQUE },
	{ 76, 208, 32, SDL_ALPHA_OPAQUE },
	{ 56, 204, 108, SDL_ALPHA_OPAQUE },
	{ 56, 180, 204, SDL_ALPHA_OPAQUE },
	{ 60, 60, 60, SDL_ALPHA_OPAQUE },
	{ 0, 0, 0, SDL_ALPHA_OPAQUE },
	{ 0, 0, 0, SDL_ALPHA_OPAQUE },

	{ 236, 238, 236, SDL_ALPHA_OPAQUE },
	{ 168, 204, 236, SDL_ALPHA_OPAQUE },
	{ 188, 188, 236, SDL_ALPHA_OPAQUE },
	{ 212, 178, 236, SDL_ALPHA_OPAQUE },
	{ 236, 174, 236, SDL_ALPHA_OPAQUE },
	{ 236, 174, 212, SDL_ALPHA_OPAQUE },
	{ 236, 180, 176, SDL_ALPHA_OPAQUE },
	{ 228, 196, 144, SDL_ALPHA_OPAQUE },
	{ 204, 210, 120, SDL_ALPHA_OPAQUE },
	{ 180, 222, 120, SDL_ALPHA_OPAQUE },
	{ 168, 226, 144, SDL_ALPHA_OPAQUE },
	{ 152, 226, 180, SDL_ALPHA_OPAQUE },
	{ 160, 214, 228, SDL_ALPHA_OPAQUE },
	{ 160, 162, 160, SDL_ALPHA_OPAQUE },
	{ 0, 0, 0, SDL_ALPHA_OPAQUE },
	{ 0, 0, 0, SDL_ALPHA_OPAQUE },
};

static inline SDL_Color read_palette_color(PPU_2C02 *ppu, uint8_t palette, uint8_t pixel)
{
	// Palettes are 4 bytes, so we need to multiply ID by 4
	return PALETTE[ppu2c02_read(ppu->bus, 16128 + (palette << 2) + pixel) & 63];
}

void ppu2c02_alloc(NES *nes, SDL_Renderer *renderer)
{
	if (!nes) return;

	nes->ppu = (PPU_2C02 *)calloc(1, sizeof(PPU_2C02));
	nes->ppu->bus = bus6500_alloc(16384);
	nes->ppu->cpu_node = bus6500_add_device(nes->bus, nes->ppu, 8192, 8199, DF_OWNS_RAM);
	nes->ppu->ppu_node = bus6500_add_device(nes->ppu->bus, nes->ppu, 8192, 16383, DF_OWNS_RAM);

	// registers
	nes->ppu->ctrl = &nes->bus->ram[8192];
	nes->ppu->mask = &nes->bus->ram[8193];
	nes->ppu->status = &nes->bus->ram[8194];
	nes->ppu->addr = &nes->bus->ram[8198];
	nes->ppu->data = &nes->bus->ram[8199];

	nes->ppu->renderer = renderer;

	// Give the renderer a solid fill colour instead of copying what's underneath
	SDL_SetRenderDrawColor(nes->ppu->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(nes->ppu->renderer);
}

void ppu2c02_clock(PPU_2C02 *ppu)
{
	if (!ppu) return;

	if (++ppu->y >= 341)
	{
		ppu->y = 0;

		if (++ppu->x >= 261)
		{
			ppu->x = -1;
			ppu->flags |= PPU_FRAME_DONE;
		}
	}
}

void ppu2c02_free(NES *nes)
{
	if (!nes) return;

	bus6500_free_device(nes->bus, nes->ppu);
	bus6500_free(nes->ppu->bus); // no need to free PPU from its own exclusive bus

	free(nes->ppu);
	nes->ppu = NULL;
}

/*void ppu2c02_noise_test(NES *nes)
{
	if (!nes) return;

	SDL_Color c = nes->ppu->palette[(rand() % 2) ? 63 : 48];
	SDL_SetRenderDrawColor(nes->ppu->renderer, c.r, c.g, c.b, c.a);
	SDL_RenderDrawPoint(nes->ppu->renderer, nes->ppu->cycle - 1, nes->ppu->scanline);
}*/

uint8_t ppu2c02_read(const BUS_6500 *bus, uint16_t addr)
{
	if (addr >= 0 && addr <= 8191)
		// MSB + remaining bits
		return bus6500_read(bus, ((addr & 4095) >> 12) * 64 + (addr & 4095));
	else if (addr > 16128 && addr <= 16383)
	{
		addr %= 31;
		if (addr == 16) addr = 0;
		if (addr == 20) addr = 4;
		if (addr == 24) addr = 8;
		if (addr == 28) addr = 12;
		return bus6500_read(bus, addr);
	}
	return 0;
}

void ppu2c02_write(BUS_6500 *bus, uint16_t addr, uint8_t data)
{
	if (addr >= 0 && addr <= 8191)
		// MSB + remaining bits
		bus6500_write(bus, ((addr & 4095) >> 12) * 64 + (addr & 4095), data);
	else if (addr > 16128 && addr <= 16383)
	{
		addr %= 31;
		if (addr == 16) addr = 0;
		if (addr == 20) addr = 4;
		if (addr == 24) addr = 8;
		if (addr == 28) addr = 12;
		bus6500_write(bus, addr, data);
	}
}

uint8_t ppu2c02_read_cpu(PPU_2C02 *ppu, uint16_t addr)
{
	uint8_t data = 0;

	switch (addr % 8)
	{
		case 0: break; // control
		case 1: break; // mask
		case 2: break; // status
			data = (*ppu->status & 224) | (*ppu->data & 31);
			*ppu->status &= ~PPU_VBLANK;
			ppu->flags &= ~PPU_ADDR_LATCH;
			break;
		case 3: break; // OAM address
		case 4: break; // OAM data
		case 5: break; // scroll
		case 6: break; // PPU address
		case 7: break; // PPU data
			data = *ppu->data;
			*ppu->data = ppu2c02_read(ppu->bus, ppu->last_addr);
			if (ppu->last_addr > 16128) data = *ppu->data;
			ppu->last_addr++;
			break;
		default: ;
	}

	return data;
}

void ppu2c02_write_cpu(PPU_2C02 *ppu, uint16_t addr, uint8_t data)
{
	switch (addr % 8)
	{
		case 0: // control
			*ppu->ctrl = data;
			break;
		case 1: // mask
			*ppu->mask = data;
			break;
		case 2: break; // status
		case 3: break; // OAM address
		case 4: break; // OAM data
		case 5: break; // scroll
		case 6: // PPU address
			if (ppu->flags & PPU_ADDR_LATCH)
			{
				ppu->last_addr = (ppu->last_addr & 255) | (data << 8);
				ppu->flags &= ~PPU_ADDR_LATCH;
			}
			else
			{
				ppu->last_addr = HI16(ppu->last_addr) | data;
				ppu->flags |= PPU_ADDR_LATCH;
			}
			*ppu->addr = data;
			break;
		case 7: // PPU data
			ppu2c02_write(ppu->bus, ppu->last_addr++, data);
			break;
		default: ;
	}
}

void ppu2c02_reset(PPU_2C02 *ppu)
{
	ppu->flags = 0;
	*ppu->status = 0;
	*ppu->mask = 0;
	*ppu->ctrl = 0;
	*ppu->data = 0;
	*ppu->addr = 0;
	ppu->last_addr = 0;
	ppu->x = 0;
	ppu->y = 0;
}
