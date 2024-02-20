#include <stdio.h>
#include <stdlib.h>

#include "rom.h"

GB_ROM * gb_rom_alloc(GB *gb, const char *filepath)
{
	FILE *f = fopen(filepath, "rb");
	if (!f) return NULL;

	GB_ROM *rom = (GB_ROM *)malloc(sizeof(GB_ROM));
	if (!rom) return NULL;

	fseek(f, 0, SEEK_END);
	rom->size = ftell(f);
	rewind(f);

	rom->data = (uint8_t *)calloc(1, rom->size);
	fread(rom->data, rom->size, 1, f);
	fclose(f);

	return rom;
}

const char * gb_rom_licensee_str(uint16_t code)
{
	switch (code)
	{
		case 1: return "Nintendo R&D1";
		case 8: return "Capcom";
		case 19: return "Electronic Arts";
		case 24: return "Hudson Soft";
		case 25: return "b-ai";
		case 32: return "kss";
		case 34: return "pow";
		case 36: return "PCM Complete";
		case 37: return "san-x";
		case 40: return "Kemco Japan";
		case 41: return "seta";
		case 48: return "Viacom";
		case 49: return "Nintendo";
		case 50: return "Bandai";
		case 51: return "Ocean/Acclaim";
		case 52: return "Konami";
		case 53: return "Hector";
		case 55: return "Taito";
		case 56: return "Hudson";
		case 57: return "Banpresto";
		case 65: return "Ubi Soft";
		case 66: return "Atlus";
		case 68: return "Malibu";
		case 70: return "angel";
		case 71: return "Bullet-Proof";
		case 73: return "irem";
		case 80: return "Absolute";
		case 81: return "Acclaim";
		case 82: return "Activision";
		case 83: return "American sammy";
		case 84: return "Konami";
		case 85: return "Hi tech entertainment";
		case 86: return "LJN";
		case 87: return "Matchbox";
		case 88: return "Mattel";
		case 89: return "Milton Bradley";
		case 96: return "Titus";
		case 97: return "Virgin";
		case 100: return "LucasArts";
		case 103: return "Ocean";
		case 105: return "Electronic Arts";
		case 112: return "Infogrames";
		case 113: return "Interplay";
		case 114: return "Broderbund";
		case 115: return "sculptured";
		case 117: return "sci";
		case 120: return "THQ";
		case 121: return "Accolade";
		case 128: return "misawa";
		case 131: return "lozc";
		case 134: return "Tokuma Shoten Intermedia";
		case 135: return "Tsukada Original";
		case 145: return "Chunsoft";
		case 146: return "Video system";
		case 147: return "Ocean/Acclaim";
		case 149: return "Varie";
		case 150: return "Yonezawa/s'pal";
		case 151: return "Kaneko";
		case 153: return "Pack in soft";
		case 164: return "Konami";
		default: return "None";
	}
}

void gb_rom_print_info(const GB_ROM *rom)
{
	if (!rom) return;

	const GB_ROM_HEADER *header = &rom->header;

	printf("Title: %.*s\n", 11, header->title);
	printf("Manufacturer: %.*s\n", 4, header->manufacturer);
	//printf("Licensee: %s\n", gb_rom_licensee_str(header->
	printf("Destination: %s\n", header->dest ? "Non-Japanese" : "Japanese");
	printf("Version: %u\n", header->version);
	printf("Header checksum: %04X\n", header->header_checksum);
	printf("Global checksum: %04X\n", header->global_checksum);

	printf("Attributes:\n");
	if (header->cgb_attr & 128) printf("\t- GameBoy Classic/Colour hybrid\n");
	if (header->cgb_attr & 192) printf("\t- GameBoy Colour exclusive\n");
	if (header->sgb_attr == 0) printf("\t- No Super GameBoy support\n");
	if (header->sgb_attr == 3) printf("\t- Super GameBoy support\n");
}
