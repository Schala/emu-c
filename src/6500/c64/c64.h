#ifndef _C64_H
#define _C64_H

#include <SDL2/SDL.h>

#include "../cpu.h"

typedef struct _C64
{
	BUS_6500 *bus;
	MOS_6500 *cpu;
} C64;

#endif // _C64_H
