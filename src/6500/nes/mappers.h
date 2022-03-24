#ifndef _NES_MAPPERS_H
#define _NES_MAPPERS_H

#include "rom.h"

// Map addresses to RAM
// Return 1 if valid, 0 otherwise

// NROM
int nes_prg000(NES_ROM *, uint16_t, uint32_t *);
int nes_chr000(NES_ROM *, uint16_t, uint32_t *);

#endif // _NES_MAPPERS_H
