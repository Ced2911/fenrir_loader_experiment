#pragma once
#include <stdint.h>

typedef struct
{
    uintptr_t cell_addr;
    uintptr_t pattern_addr;
    uintptr_t pal_addr;
    uint16_t texture_w;
    uint16_t texture_h;
    uint16_t noise_palettes_count;
    color_rgb1555_t noise_palettes[];
} noise_cfg_t;

void noise_init(noise_cfg_t *noise_cfg);
void noise_update(noise_cfg_t *noise_cfg);