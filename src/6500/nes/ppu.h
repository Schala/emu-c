#ifndef _NES_PPU_H
#define _NES_PPU_H

#include "nes.h"

// 2C02 pixel processing unit
struct _PPU_2C02
{
	uint8_t flags;
	int16_t x; // screen row
	int16_t y; // screen column
	uint16_t last_addr;
	SDL_Renderer *renderer;
	uint8_t status;
	uint8_t mask;
	uint8_t ctrl;
	uint8_t data;
	uint8_t addr;
	BUS_6500 *bus; // PPU's own bus
	DEV_6500 *cpu_node; // device node in CPU bus device tree
	DEV_6500 *ppu_node; // device node in its own bus
};

// Initialise a new PPU on the NES
void ppu2c02_alloc(NES *, SDL_Renderer *);

// Advance the clock, which is relentless
void ppu2c02_clock(PPU_2C02 *);

// Deallocate PPU
void ppu2c02_free(NES *);

// Test noise
//void ppu2c02_noise_test(NES *);

// PPU read from CPU
uint8_t ppu2c02_read_cpu(PPU_2C02 *, uint16_t);

// PPU write to CPU
void ppu2c02_write_cpu(PPU_2C02 *, uint16_t, uint8_t);

// Read byte from PPU RAM address
uint8_t ppu2c02_read(const BUS_6500 *, uint16_t);

// Write byte to PPU RAM address
void ppu2c02_write(BUS_6500 *, uint16_t, uint8_t);

void ppu2c02_reset(PPU_2C02 *);

#endif // _NES_PPU_H
