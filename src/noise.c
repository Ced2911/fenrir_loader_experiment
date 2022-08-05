#include <yaul.h>
#include "noise.h"

#define CELL_SIZE_W (8)
#define CELL_SIZE_H (8)

static unsigned long long int __rand(void)
{
    static long long int x = 0; // not so random ^^
    x = x * 6364136223846793005 + 1442695040888963407;
    return ((unsigned long long int)x & 0xffffffffffffffffLL);
}

void noise_init(noise_cfg_t *noise_cfg)
{
    // create a noise texture
    int w = noise_cfg->texture_w;
    int h = noise_cfg->texture_h;
    uint8_t *noise_texture = (uint8_t *)(noise_cfg->cell_addr + CELL_SIZE_W * CELL_SIZE_H);
    uint16_t *pnd = (uint16_t *)noise_cfg->pattern_addr;
    color_rgb1555_t *pal = (color_rgb1555_t *)noise_cfg->pal_addr;

    // empty cell
    memset((void *)noise_cfg->cell_addr, 0, CELL_SIZE_W * CELL_SIZE_H);

    // empty pattern
    for (int i = 0; i < 0x800; i++)
    {
        pnd[i] = VDP2_SCRN_PND_CONFIG_0(0, noise_cfg->cell_addr, 0, 0, 0);
    }

    // noise cell
    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            noise_texture[(x * w) + y] = 1 | ((__rand() >> 16) & 0xF);
        }
    }

    // add noise
    for (int i = 0; i < 8 /*(64 * 64)*/; i++)
    {
        pnd[i] = VDP2_SCRN_PND_CONFIG_0(0, noise_cfg->cell_addr + (1 + i) * (CELL_SIZE_W * CELL_SIZE_H), 0, 0, 0);
    }

    vdp_dma_enqueue((void *)noise_cfg->pal_addr, noise_cfg->noise_palettes, noise_cfg->noise_palettes_count * sizeof(color_rgb1555_t));
}

void noise_update(noise_cfg_t *noise_cfg)
{
    // change noise palette to create noise animation
    color_rgb1555_t *pal = (color_rgb1555_t *)noise_cfg->pal_addr;
    static int i = 0;
    i++;
    for (int x = 0; x < noise_cfg->noise_palettes_count; x++)
    {
        uint8_t idx = (__rand() >> 16) & 0xf;
        pal[x] = noise_cfg->noise_palettes[idx];
    }
}
