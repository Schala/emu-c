#pragma once

// Disassembly cache
typedef struct _MOS6500Disasm MOS6500Disasm;


// Deallocate disassembly cache
void mos6500DisasmFree(MOS6500Disasm *disasm);

// Retrieve the disassembly line at a given index
const MOS6500Disasm * mos6500DisasmGet(MOS6500Disasm *disasm, size_t index);

// Disassembles a single operation. `next` is NULL.
MOS6500Disasm * mos6500DisasmOp(const hxbuf buf, size_t *offset);

// Prints all disassembly in tree
void mos6500DisasmDump(MOS6500Disasm *disasm);

// Disassembles a range of memory
MOS6500Disasm * mos6500DisasmRange(const hxbuf buf, size_t start, size_t end);
