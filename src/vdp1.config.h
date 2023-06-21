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
#define ORDER_BUFFER_DEV_ICON (ORDER_BUFFER_SKIP + 1)
#define ORDER_BUFFER_STARTING_INDEX 5
#define ORDER_COUNT 400
#define ORDER_DRAW_END_INDEX (ORDER_COUNT - 1)

extern vdp1_cmdt_list_t *cmdt_list;
extern uint32_t vdp1_texture_base_addr;
void vdp1_init();
void vdp1_reset();

#define FONT_CACHE_SIZE (64 * 1024)
#define VDP2_CRAM_LUT (0x80)


#define COVER_TEXTURE_ADDR (vdp1_texture_base_addr + (FONT_CACHE_SIZE * 2))
#define ICONS_TEXTURE_ADDR (COVER_TEXTURE_ADDR + (128 * 128 * 2))
