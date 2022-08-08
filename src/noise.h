#pragma once
#include <stdint.h>

#define NOISE_PALETTES                                    \
    {                                                     \
        COLOR_RGB1555(1, 0x00, 0xC0 >> 3, 0x40 >> 3),     \
            COLOR_RGB1555(1, 0x00, 0xF0 >> 3, 0x00 >> 3), \
            COLOR_RGB1555(1, 0x00, 0x80 >> 3, 0x60 >> 3), \
            COLOR_RGB1555(1, 0x00, 0xF0 >> 3, 0xF0 >> 3), \
                                                          \
            COLOR_RGB1555(1, 0x00, 0xC0 >> 3, 0x40 >> 3), \
            COLOR_RGB1555(1, 0x00, 0xF0 >> 3, 0x00 >> 3), \
            COLOR_RGB1555(1, 0x00, 0x80 >> 3, 0x60 >> 3), \
            COLOR_RGB1555(1, 0x00, 0xF0 >> 3, 0xF0 >> 3), \
                                                          \
            COLOR_RGB1555(1, 0x00, 0xC0 >> 3, 0x40 >> 3), \
            COLOR_RGB1555(1, 0x00, 0xF0 >> 3, 0x00 >> 3), \
            COLOR_RGB1555(1, 0x00, 0x80 >> 3, 0x60 >> 3), \
            COLOR_RGB1555(1, 0x00, 0xF0 >> 3, 0xF0 >> 3), \
                                                          \
            COLOR_RGB1555(1, 0x00, 0xC0 >> 3, 0x40 >> 3), \
            COLOR_RGB1555(1, 0x00, 0xF0 >> 3, 0x00 >> 3), \
            COLOR_RGB1555(1, 0x00, 0x80 >> 3, 0x60 >> 3), \
            COLOR_RGB1555(1, 0x00, 0xF0 >> 3, 0xF0 >> 3), \
    }

typedef struct
{
    uintptr_t cell_addr;
    uintptr_t pattern_addr;
    uintptr_t pal_addr;
    // ui theme 
    uint16_t cell_x;
    uint16_t cell_y;
    uint16_t cell_w;
    uint16_t cell_h;
} noise_cfg_t;

void noise_init(noise_cfg_t *noise_cfg);
void noise_update(noise_cfg_t *noise_cfg);
void noise_destroy(noise_cfg_t *noise_cfg);
