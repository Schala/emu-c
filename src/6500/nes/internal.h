#ifndef _NES_INTERNAL_H
#define _NES_INTERNAL_H

#define MI_VMIRROR 1
#define MI_BATTERY_BACKED_RAM 2
#define MI_TRAINER 4
#define MI_4_SCREEN_VRAM_LAYOUT 8
#define MI_VS_SYS_CART 256

// flags
#define PPU_FRAME_DONE 1
#define PPU_ADDR_LATCH 2

// mask
#define PPU_GRAYSCALE 1
#define PPU_RENDER_BG_L 2
#define PPU_RENDER_BG_R 4
#define PPU_RENDER_BG 8
#define PPU_RENDER_FG 16
#define PPU_ENHANCE_R 32
#define PPU_ENHANCE_G 64
#define PPU_ENHANCE_B 128

// status
#define PPU_OVERFLOW 32
#define PPU_0_HIT 64
#define PPU_VBLANK 128

// control
#define PPU_NAMETABLE_X 1
#define PPU_NAMETABLE_Y 2
#define PPU_INCREMENT 4
#define PPU_PATTERN_FG 8
#define PPU_PATTERN_BG 16
#define PPU_SPRITE_SIZE 32
#define PPU_SLAVE 64 // unused
#define PPU_NMI 128

#endif // _NES_INTERNAL_H
