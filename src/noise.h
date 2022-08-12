#pragma once
#include <stdint.h>
#define NOISE_PALETTE_SIZE (16)

typedef struct
{
    uintptr_t cell_addr;
    uintptr_t pattern_addr;
    uintptr_t pal_addr;
    uintptr_t noise_palette;
    size_t noise_palettes_sz;
    // ui theme 
    uint16_t cell_x;
    uint16_t cell_y;
    uint16_t cell_w;
    uint16_t cell_h;
} noise_cfg_t;

void noise_init(noise_cfg_t *noise_cfg);
void noise_update(noise_cfg_t *noise_cfg);
void noise_destroy(noise_cfg_t *noise_cfg);
