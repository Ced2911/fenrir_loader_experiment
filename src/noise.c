#include <yaul.h>
#include "noise.h"

#define CELL_SIZE_W (8)
#define CELL_SIZE_H (8)

static color_rgb1555_t work_palettes[NOISE_PALETTE_SIZE];

static unsigned long long int __rand(void)
{
    static long long int x = 0; // not so random ^^
    x = x * 6364136223846793005 + 1442695040888963407;
    return ((unsigned long long int)x & 0xffffffffffffffffLL);
}

void noise_destroy(noise_cfg_t *noise_cfg)
{
}

void noise_init(noise_cfg_t *noise_cfg)
{
    // create a noise texture
    int w = 256;
    int h = 256;

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
    for (size_t n = 0, y = noise_cfg->cell_y; y < (noise_cfg->cell_y + noise_cfg->cell_h); y++)
    {
        for (int x = noise_cfg->cell_x; x < (noise_cfg->cell_x + noise_cfg->cell_w); x++, n++)
        {
            pnd[x + y * 64] = VDP2_SCRN_PND_CONFIG_0(0, noise_cfg->cell_addr + (1 + (n % (50))) * (CELL_SIZE_W * CELL_SIZE_H), 0, 0, 0);
        }
    }

    vdp_dma_enqueue((void *)noise_cfg->pal_addr, (void *)noise_cfg->noise_palette, noise_cfg->noise_palettes_sz);
}

void noise_update(noise_cfg_t *noise_cfg)
{
    // change noise palette to create noise animation
    static int i = 0;
    i++;
    for (size_t x = 0; x < noise_cfg->noise_palettes_sz / sizeof(color_rgb1555_t); x++)
    {
        uint8_t idx = (__rand() >> 16) & 0xf;
        work_palettes[x] = ((color_rgb1555_t *)noise_cfg->noise_palette)[idx];
    }
    vdp_dma_enqueue((void *)noise_cfg->pal_addr, (void *)work_palettes, noise_cfg->noise_palettes_sz);
}
