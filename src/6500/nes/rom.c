#include <stdio.h>
#include <stdlib.h>

#include "internal.h"
#include "mappers.h"
#include "ppu.h"
#include "rom.h"

static const NES_MAPPER PRG_MAPPERS[] =
{
	&nes_prg000
};

static const NES_MAPPER CHR_MAPPERS[] =
{
	&nes_chr000
};

NES_ROM * nes_rom_alloc(NES *nes, const char *filepath)
{
	FILE *f = fopen(filepath, "rb");
	NES_ROM_HEADER header;

	fread(&header, sizeof(NES_ROM_HEADER), 1, f);

	if (strncmp(header.magic, "NES\x1A", 4) != 0 || !filepath)
		fclose(f);

	// Mapper must be identified first so that we can initialise everything correctly
	NES_ROM *rom = (NES_ROM *)malloc(sizeof(NES_ROM));
	if (!rom) return NULL;

	rom->mapper_id = ((header.mapper_info & 240) << 4) | ((header.mapper_info & 7680) >> 9);
	rom->prg_map = PRG_MAPPERS[rom->mapper_id];
	rom->chr_map = CHR_MAPPERS[rom->mapper_id];

	// ignore trainer, if present
	if (header.mapper_info & MI_TRAINER) fseek(f, 512, SEEK_CUR);

	fread(&nes->bus->ram[32768], 16384, header.prg_pages, f);
	fread(nes->ppu->bus->ram, 8192, header.chr_pages > 1 ? header.chr_pages : 1, f);

	fclose(f);

	rom->header = header;

	mos6500_map(nes->cpu, 32768, 65535);
	nes->rom->ppu_node->ram_offset = 0;
	nes->rom->ppu_node->ram_size = 8192;

	// and add the ROM's PRG and CHR to their respective bus
	nes->rom->cpu_node = bus6500_add_device(nes->bus, nes->rom, 32768, 65535, DF_OWNS_RAM);
	nes->rom->ppu_node = bus6500_add_device(nes->ppu->bus, nes->rom, 0, 8191, DF_OWNS_RAM);
}

void nes_rom_free(NES *nes)
{
	if (!nes) return;
	if (!nes->rom) return;

	bus6500_free_device(nes->bus, nes->rom);
	bus6500_free_device(nes->ppu->bus, nes->rom);
	free(nes->rom);
	nes->rom = NULL;
}

void nes_print_rom_info(const NES_ROM *rom)
{
	if (!rom) return;

	const NES_ROM_HEADER *header = &rom->header;

	printf("Magic: %c%c%c\\x%2X\n", header->magic[0], header->magic[1], header->magic[2], header->magic[3]);
	printf("PRG pages: %u\n", header->prg_pages);
	printf("CHR pages: %u\n", header->chr_pages);
	printf("Mapper:\n");
	if (header->mapper_info & MI_VMIRROR) printf("\t- vertical mirroring\n");
	if (header->mapper_info & MI_BATTERY_BACKED_RAM) printf("\t- battery backed RAM\n");
	if (header->mapper_info & MI_TRAINER) printf("\t- trainer\n");
	if (header->mapper_info & MI_4_SCREEN_VRAM_LAYOUT) printf("\t- 4 screen VRAM layout\n");
	if (header->mapper_info & MI_VS_SYS_CART) printf("\t- VS system cartridge\n");
	printf("RAM pages: %u\n", header->ram_pages);
	printf("Format: %s\n", header->is_pal ? "PAL" : "NTSC");
	printf("Mapper ID: %X\n", rom->mapper_id);
}

uint8_t nes_rom_read_cpu(NES *nes, uint16_t addr)
{
	if (!nes) return 0;

	uint32_t mapped_addr = 0;
	if (nes->rom->prg_map(nes->rom, addr, &mapped_addr))
		return bus6500_read(nes->bus, mapped_addr);
	return 0;
}

void nes_rom_write_cpu(NES *nes, uint16_t addr, uint8_t data)
{
	if (!nes) return;

	uint32_t mapped_addr = 0;
	if (nes->rom->prg_map(nes->rom, addr, &mapped_addr))
		bus6500_write(nes->bus, mapped_addr, data);
}

uint8_t nes_rom_read_ppu(NES *nes, uint16_t addr)
{
	if (!nes) return 0;

	uint32_t mapped_addr = 0;
	if (nes->rom->chr_map(nes->rom, addr, &mapped_addr))
		return bus6500_read(nes->ppu->bus, mapped_addr);
	return 0;
}

void nes_rom_write_ppu(NES *nes, uint16_t addr, uint8_t data)
{
	if (!nes) return;

	uint32_t mapped_addr = 0;
	if (nes->rom->chr_map(nes->rom, addr, &mapped_addr))
		bus6500_write(nes->ppu->bus, mapped_addr, data);
}
