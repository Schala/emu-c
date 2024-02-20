#pragma once

// Dump RAM to buffer
size_t mos6500RAMDump(const MOS6500 *cpu, hxbuf buf);

// Read byte from RAM address
u8 mos6500RAMRead(const MOS6500 *cpu, u32 addr);

// Read address from RAM address
u16 mos6500RAMReadAddr(const MOS6500 *cpu, u16 addr);

// Read byte from ROM
u8 mos6500ROMRead(MOS6500 *cpu);

// Read address from ROM
u16 mos6500ROMReadAddr(MOS6500 *cpu);

// Write byte to RAM address
void mos6500RAMWrite(MOS6500 *cpu, u16 addr, u8 data);

// Write byte to last used address
void mos6500RAMWriteLast(MOS6500 *cpu, u8 data);
