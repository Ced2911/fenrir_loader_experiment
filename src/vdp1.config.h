#pragma once

#include <yaul.h>

#define ORDER_SYSTEM_CLIP_COORDS_INDEX 0
#define ORDER_CLEAR_LOCAL_COORDS_INDEX 1
#define ORDER_BUFFER_SKIP 2
#define ORDER_BUFFER_STARTING_INDEX 3
#define ORDER_COUNT 400
#define ORDER_DRAW_END_INDEX (ORDER_COUNT - 1)

extern vdp1_cmdt_list_t *cmdt_list;
void vdp1_init();

#define FONT_CACHE_SIZE (32*1024)