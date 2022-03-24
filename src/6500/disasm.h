#ifndef _6500_DISASM_H
#define _6500_DISASM_H

#include <stdint.h>

// Disassembly line length
#define DISASM_STR_LEN 16

// Disassembly cache
typedef struct _DISASM_6500
{
	char lhs[DISASM_STR_LEN];
	char rhs[DISASM_STR_LEN];
	uint16_t addr;
	struct _DISASM_6500 *next;
} DISASM_6500;

// Deallocate disassembly cache
void disasm6500_free(DISASM_6500 *);

// Retrieve the disassembly line at a given index
const DISASM_6500 * disasm6500_get(DISASM_6500 *, uint16_t);

// Disassembles a single operation. `next` is NULL.
DISASM_6500 * disasm6500_op(uint8_t *, uint16_t *);

// Prints all disassembly in tree
void disasm6500_print(DISASM_6500 *);

// Disassembles a range of memory
DISASM_6500 * disasm6500_range(uint8_t *, uint16_t, uint16_t);

#endif // _6500_DISASM_H
