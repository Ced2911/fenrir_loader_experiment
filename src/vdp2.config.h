#pragma once

#define SCREEN_4_BITMAP_SIZE  (512*256/2)
#define SCREEN_8_BITMAP_SIZE  (512*256*1)
#define SCREEN_16_BITMAP_SIZE  (512*256*2)

#define NBG0_BITMAP_ADDR   (VDP2_VRAM_ADDR(0, 0x00000))
#define NBG1_BITMAP_ADDR   (VDP2_VRAM_ADDR(1, 0x00000))
#define NBG2_BITMAP_ADDR   (VDP2_VRAM_ADDR(1, 0x10000))

void vdp2_init();
void vdp2_bg_cell();
void vdp2_fg_cell();

