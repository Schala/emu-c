#ifndef _NES_H
#define _NES_H

#include <SDL2/SDL.h>

#include "../cpu.h"

typedef struct _NES_ROM NES_ROM;
typedef struct _PPU_2C02 PPU_2C02;

// The NES system itself
typedef struct _NES
{
	size_t cycles; // global cycle count
	BUS_6500 *bus;
	MOS_6500 *cpu;
	PPU_2C02 *ppu;
	NES_ROM *rom;
} NES;

// Allocate the NES system
NES * nes_alloc(SDL_Renderer *);

// Allocates the NES CPU, mapped appropriately in RAM
void nes_cpu_alloc(NES *);

// Deallocate the NES system
void nes_free(NES *);

// Read byte from RAM address
uint8_t nes_read(const NES *, uint16_t);

// Write byte to RAM address
void nes_write(NES *, uint16_t, uint8_t);

// Runs NES clocks
void nes_clock(NES *);

// Resets the NES
void nes_reset(NES *);

// Prints PRG memory
/*void nes_print_prg(const NES *);

// Prints CHR memory
void nes_print_chr(const NES *);*/

#endif // _NES_H
