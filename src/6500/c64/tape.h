#ifndef _C64_TAPE_H
#define _C64_TAPE_H

#include <stdint.h>

typedef enum _C64S_FILETYPE
{
	C64S_FREE = 0,
	C64S_NORMAL,
	C64S_UNCOMPRESSED = 3
} C64S_FILETYPE;

typedef enum _T64_FILETYPE
{
	T64F_SEQ = 129,
	T64F_PRG
} T64_FILETYPE;

typedef struct _T64_DIR_REC
{
	uint8_t c64s_filetype;
	uint8_t filetype;
	uint16_t start_addr;
	uint16_t end_addr;
	uint16_t _reserved6;
	uint32_t offset; // big endian
	uint32_t _reserved10;
	char name[16]; // padded with spaces
} T64_DIR_REC;

typedef struct _T64_HEADER
{
	char magic[32];
	uint16_t version;
	uint16_t max_dir_entries;
	uint16_t used_entries;
	uint16_t _reserved38;
	char name[24]; // padded with spaces
} T64_HEADER;

typedef struct _TAP_HEADER
{
	char magic[12];
	uint8_t version;
	uint8_t _reserved14[3];
	uint32_t data_size;
} TAP_HEADER;

static inline double c64_tap_pulse(uint8_t *p)
{
	// Predominantly PAL, so we go with that format's cycle amount.
	// NTSC is almost the same, so shouldn't have issues.
	return ((double)(*(p++)) * 8) / 985248.0;
}

#endif // _C64_TAPE_H
