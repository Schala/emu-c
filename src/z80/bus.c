#include <stdio.h>
#include <stdlib.h>

#include "../util.h"
#include "bus.h"

Z80_BUS * z80bus_alloc(uint16_t ram_size)
{
	Z80_BUS *bus = (Z80_BUS *)calloc(1, sizeof(Z80_BUS));
	bus->ram = (uint8_t *)calloc(1, ram_size);
	bus->ram_size = ram_size;

	return bus;
}


Z80_DEV * z80bus_add_device(Z80_BUS *bus, void *dev, uint16_t ram_offset, uint16_t ram_size, uint8_t flags)
{
	if (!bus) return NULL;
	if (!dev) return NULL;

	if (!bus->dev_list)
		bus->dev_list = (Z80_DEV *)calloc(1, sizeof(Z80_DEV));

	Z80_DEV *it = bus->dev_list;

	while (it) it = it->next;

	it = (Z80_DEV *)calloc(0, sizeof(Z80_DEV));
	it->flags = flags;
	it->ram_offset = ram_offset;
	it->ram_size = ram_size;
	it->data = dev;

	return it;
}

void * z80bus_device(Z80_BUS *bus, size_t index)
{
	if (!bus) return NULL;

	Z80_DEV *it = bus->dev_list;
	while (index--) it = it->next;
	return it->data;
}

void z80bus_free(Z80_BUS *bus)
{
	if (!bus) return;

	Z80_DEV *it = NULL;
	Z80_DEV *next = bus->dev_list;

	while (next)
	{
		it = next;
		next = it->next;
		free(it);
	}

	free(bus);
	bus = NULL;
}

void z80bus_free_device(Z80_BUS *bus, void *dev)
{
	if (!bus) return;
	if (!dev) return;

	Z80_DEV *it = bus->dev_list;
	Z80_DEV *prev = NULL;

	while (dev != it->data)
	{
		if (!it->next) break;

		prev = it;
		it = it->next;
	}

	// only free if we have a match
	if (dev == it->data)
	{
		if (prev)
		{
			Z80_DEV *next = it->next;
			free(it);
			prev->next = next;
		}
		else
			free(it);
	}
}

void z80bus_print_ram(const Z80_BUS *bus)
{
	if (!bus) return;

	hexdump(bus->ram, bus->ram_size);
}

int z80bus_ram_dump(const Z80_BUS *bus, size_t iter)
{
	if (!bus) return -1;

	char fmt[BUFSIZ];
	memset((char *)&fmt, 0, BUFSIZ);
	sprintf((char *)&fmt, "z80.%u.dmp", iter);

	FILE *dump = fopen((char *)&fmt, "wb");
	fwrite(&bus->ram, 1, bus->ram_size, dump);

	return fclose(dump);
}
