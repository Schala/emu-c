#include <stdio.h>
#include <stdlib.h>

#include "../common/util.h"
#include "bus.h"

BUS_6500 * bus6500_alloc(uint16_t ram_size)
{
	BUS_6500 *bus = (BUS_6500 *)calloc(1, sizeof(BUS_6500));
	bus->ram = (uint8_t *)calloc(1, ram_size);
	bus->ram_size = ram_size;

	return bus;
}


DEV_6500 * bus6500_add_device(BUS_6500 *bus, void *dev, uint16_t ram_offset, uint16_t ram_size, uint8_t flags)
{
	if (!bus) return NULL;
	if (!dev) return NULL;

	if (!bus->dev_list)
		bus->dev_list = (DEV_6500 *)calloc(1, sizeof(DEV_6500));

	DEV_6500 *it = bus->dev_list;

	while (it) it = it->next;

	it = (DEV_6500 *)calloc(0, sizeof(DEV_6500));
	it->flags = flags;
	it->ram_offset = ram_offset;
	it->ram_size = ram_size;
	it->data = dev;

	return it;
}

void * bus6500_device(BUS_6500 *bus, size_t index)
{
	if (!bus) return NULL;

	DEV_6500 *it = bus->dev_list;
	while (index--) it = it->next;
	return it->data;
}

DISASM_6500 * bus6500_disasm(BUS_6500 *bus, uint16_t start, uint16_t end)
{
	if (!bus) return NULL;
	return disasm6500_range(bus->ram, start, end);
}

void bus6500_free(BUS_6500 *bus)
{
	if (!bus) return;

	DEV_6500 *it = NULL;
	DEV_6500 *next = bus->dev_list;

	while (next)
	{
		it = next;
		next = it->next;
		free(it);
	}

	free(bus);
	bus = NULL;
}

void bus6500_free_device(BUS_6500 *bus, void *dev)
{
	if (!bus) return;
	if (!dev) return;

	DEV_6500 *it = bus->dev_list;
	DEV_6500 *prev = NULL;

	while (dev != it->data)
	{
		if (!it->next) break;

		prev = it;
		it = it->next;
	}

	// only free if we have a match
	if (dev == it->data)
	{
		if (it->flags & DF_OWNS_RAM)
			memset(&bus->ram[it->ram_offset], it->ram_size, 0);

		if (prev)
		{
			DEV_6500 *next = it->next;
			free(it);
			prev->next = next;
		}
		else
			free(it);
	}
}

void bus6500_print_ram(const BUS_6500 *bus)
{
	if (!bus) return;

	hexdump(bus->ram, bus->ram_size);
}

int bus6500_ram_dump(const BUS_6500 *bus, size_t iter)
{
	if (!bus) return -1;

	char fmt[BUFSIZ];
	memset((char *)&fmt, 0, BUFSIZ);
	sprintf((char *)&fmt, "6500.%u.dmp", iter);

	FILE *dump = fopen((char *)&fmt, "wb");
	fwrite(&bus->ram, 1, bus->ram_size, dump);

	return fclose(dump);
}
