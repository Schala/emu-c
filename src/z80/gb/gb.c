#include "gb.h"

uint8_t gb_read(const GB *gb, uint16_t addr)
{
	if (addr < 32768)
		return z80bus_read(gb->bus, addr);
	else
		return z80bus_read(gb->bus, addr);
}

void gb_write(GB *gb, uint16_t addr, uint8_t data)
{
	if (addr < 32768)
		z80bus_write(gb->bus, addr, data);
}
