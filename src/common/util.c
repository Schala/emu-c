#include <ctype.h>
#include <stdio.h>

#include "util.h"

void hexdump(const uint8_t *data, size_t size)
{
	if (!data || size == 0) return;

	for (int y = 0; y < size >> 4; y++)
	{
		// offset
		printf("%04X: ", y << 4);

		// hex
		for (int x = 0; x < 16; x++)
			printf("%02X ", data[(y << 4) + x]);

		printf("\t");

		// ASCII
		for (int x = 0; x < 16; x++)
		{
			char c = data[(y << 4) + x];
			printf("%c", isgraph(c) ? c : '.');
		}

		printf("\n");
	}
}
