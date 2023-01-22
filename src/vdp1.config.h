#pragma once
#include <yaul.h>
#ifdef FENRIR_480i
#define SCREEN_WIDTH (320 * 2)
#define SCREEN_HEIGHT (240 * 2)
#else
#define SCREEN_WIDTH (320)
#define SCREEN_HEIGHT (240)
#endif

#define ORDER_SYSTEM_CLIP_COORDS_INDEX 0
#define ORDER_CLEAR_LOCAL_COORDS_INDEX 1
#define ORDER_BUFFER_SKIP 2
#define ORDER_BUFFER_STARTING_INDEX 3
#define ORDER_COUNT 400
#define ORDER_DRAW_END_INDEX (ORDER_COUNT - 1)

extern vdp1_cmdt_list_t *cmdt_list;
void vdp1_init();
void vdp1_reset();

#define FONT_CACHE_SIZE (32 * 1024)
#define VDP2_CRAM_LUT (0x80)