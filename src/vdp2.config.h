#pragma once

#if 0
// noize
#define NGB1_PATTERN_ADDR (VDP2_VRAM_ADDR(0, 0x08000))
#define NBG1_CELL_ADDR (VDP2_VRAM_ADDR(1, 0x10000))
#define NBG1_COLOR_ADDR (VDP2_CRAM_MODE_0_OFFSET(0, 0, 0))

// bg
#define NGB0_PATTERN_ADDR (VDP2_VRAM_ADDR(0, 0x00000))
#define NBG0_CELL_ADDR (VDP2_VRAM_ADDR(1, 0x00000))
#define NBG0_COLOR_ADDR (VDP2_CRAM_MODE_0_OFFSET(0, 0, 0))

//fg
#define NGB2_PATTERN_ADDR (VDP2_VRAM_ADDR(0, 0x04000))
#define NBG2_CELL_ADDR (VDP2_VRAM_ADDR(1, 0x00000))
#define NBG2_COLOR_ADDR (VDP2_CRAM_MODE_0_OFFSET(0, 0, 0))
#else
// noize
#define NGB1_PATTERN_ADDR (VDP2_VRAM_ADDR(3, 0x00000))
#define NBG1_CELL_ADDR (VDP2_VRAM_ADDR(3, 0x02000))
#define NBG1_COLOR_ADDR (VDP2_CRAM_ADDR(0x300))

// bg
#define NGB0_PATTERN_ADDR (VDP2_VRAM_ADDR(1, 0x00000))
#define NBG0_CELL_ADDR (VDP2_VRAM_ADDR(0, 0x00000))
#define NBG0_COLOR_ADDR (VDP2_CRAM_ADDR(0x100))

// fg
#define NGB2_PATTERN_ADDR (VDP2_VRAM_ADDR(1, 0x04000))
#define NBG2_CELL_ADDR (VDP2_VRAM_ADDR(0, 0x00000))
#define NBG2_COLOR_ADDR (VDP2_CRAM_ADDR(0x200))
#endif

void vdp2_init();