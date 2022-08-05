#include <yaul.h>
#include <stdio.h>
#include <stdlib.h>
#include "fenrir/fenrir.h"
#include "vdp1.config.h"
#include "vdp2.config.h"
#include "font/font.h"
#include "screens/gamelist.h"
#include "../assets/bitmap.h"
#include "../assets/test.h"

#define RESOLUTION_WIDTH (352)
#define RESOLUTION_HEIGHT (224)

sd_dir_t *sd_dir;
status_sector_t *status_sector;
sd_dir_entry_t *sd_dir_entries;

static void copy_bitmap_data(const vdp2_scrn_bitmap_format_t *format)
{
    uint8_t *cpd = (uint8_t *)format->bitmap_pattern;
    vdp_dma_enqueue(cpd, bitmap, bitmapLength / 16);
}

static void copy_palette(const vdp2_scrn_bitmap_format_t *format)
{
    uint16_t *color_palette;
    color_palette = (uint16_t *)format->color_palette;
    vdp_dma_enqueue(color_palette, pal, palLength);
}

static void vdp2_ngb0_init()
{
    const vdp2_scrn_cell_format_t format = {
        .scroll_screen = VDP2_SCRN_NBG0,
        .cc_count = VDP2_SCRN_CCC_PALETTE_256,
        .character_size = 1 * 1,
        .pnd_size = 1,
        .auxiliary_mode = 1,
        .plane_size = 1 * 1,
        .cp_table = NBG0_CELL_ADDR,
        .color_palette = NBG0_COLOR_ADDR,
        .map_bases = {
            .planes = {
                NGB0_PATTERN_ADDR, NGB0_PATTERN_ADDR + 0x02000, NGB0_PATTERN_ADDR + 0x00000, NGB0_PATTERN_ADDR + 0x02000}}};

    vdp_dma_enqueue((void *)NBG0_COLOR_ADDR, pal, palLength);
    vdp_dma_enqueue((void *)NBG0_CELL_ADDR, cells, cellsLength);
    vdp_dma_enqueue((void *)NGB0_PATTERN_ADDR, pattern, patternLength);

    vdp2_scrn_cell_format_set(&format);
    vdp2_scrn_priority_set(VDP2_SCRN_NBG0, 2);
}

static void vdp2_ngb1_init()
{
    const vdp2_scrn_cell_format_t format = {
        .scroll_screen = VDP2_SCRN_NBG1,
        .cc_count = VDP2_SCRN_CCC_PALETTE_256,
        .character_size = 1 * 1,
        .pnd_size = 1,
        .auxiliary_mode = 0,
        .plane_size = 1 * 1,
        .cp_table = NBG1_CELL_ADDR,
        .color_palette = 0,
        .map_bases = {
            .planes = {
                NGB1_PATTERN_ADDR, NGB1_PATTERN_ADDR, NGB1_PATTERN_ADDR, NGB1_PATTERN_ADDR}}};

    vdp2_scrn_cell_format_set(&format);
    vdp2_scrn_priority_set(VDP2_SCRN_NBG1, 3);
    vdp2_cram_offset_set(VDP2_SCRN_NBG1, VDP2_CRAM_ADDR(0x100));
    // vdp2_scrn_scroll_x_set(VDP2_SCRN_NBG1, FIX16(512));
}

void vdp2_init()
{
    vdp2_vram_cycp_t vram_cycp;

    vram_cycp.pt[0].t0 = VDP2_VRAM_CYCP_PNDR_NBG0;
    vram_cycp.pt[0].t1 = VDP2_VRAM_CYCP_PNDR_NBG0;
    vram_cycp.pt[0].t2 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[0].t3 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[0].t4 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[0].t5 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[0].t6 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[0].t7 = VDP2_VRAM_CYCP_NO_ACCESS;

    vram_cycp.pt[1].t0 = VDP2_VRAM_CYCP_CHPNDR_NBG0;
    vram_cycp.pt[1].t1 = VDP2_VRAM_CYCP_CHPNDR_NBG0;
    vram_cycp.pt[1].t2 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[1].t3 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[1].t4 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[1].t5 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[1].t6 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[1].t7 = VDP2_VRAM_CYCP_NO_ACCESS;

    vram_cycp.pt[2].t0 = VDP2_VRAM_CYCP_PNDR_NBG1;
    vram_cycp.pt[2].t1 = VDP2_VRAM_CYCP_PNDR_NBG1;
    vram_cycp.pt[2].t2 = VDP2_VRAM_CYCP_CHPNDR_NBG1;
    vram_cycp.pt[2].t3 = VDP2_VRAM_CYCP_CHPNDR_NBG1;
    vram_cycp.pt[2].t4 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[2].t5 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[2].t6 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[2].t7 = VDP2_VRAM_CYCP_NO_ACCESS;

    vram_cycp.pt[3].t0 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[3].t1 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[3].t2 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[3].t3 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[3].t4 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[3].t5 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[3].t6 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[3].t7 = VDP2_VRAM_CYCP_NO_ACCESS;

    vdp2_vram_cycp_set(&vram_cycp);

    vdp2_ngb0_init();
    vdp2_ngb1_init();

    vdp2_scrn_display_set(VDP2_SCRN_NBG0_DISP | VDP2_SCRN_NBG1_DISP);
}

void *zalloc(size_t l)
{
    void *ptr = malloc(l);
    memset(ptr, 0, l);
    return ptr;
}

int main(void)
{
    vdp1_vram_partitions_t vdp1_vram_partitions;
    vdp1_vram_partitions_get(&vdp1_vram_partitions);
    //  dbgio_init();
    // dbgio_dev_default_init(DBGIO_DEV_VDP2_ASYNC);
    // dbgio_dev_font_load();

    vdp1_init();
    font_to_vram();
    vdp2_init();

    sd_dir = (sd_dir_t *)zalloc(sizeof(sd_dir_t));
    status_sector = (status_sector_t *)zalloc(sizeof(status_sector_t));
    sd_dir_entries = (sd_dir_entry_t *)zalloc(sizeof(sd_dir_entry_t) * 2500);

    screen_t *screen = &gamelist_screen;
    screen->init();

    while (1)
    {
        screen->update();

        vdp1_sync_render();
        vdp1_sync();
        vdp2_sync();
        vdp2_sync_wait();
    }
}

static void _vblank_out_handler(void *work __unused)
{
    smpc_peripheral_intback_issue();
}

void user_init(void)
{

    static const struct vdp1_env env = {
        .bpp = VDP1_ENV_BPP_16,
        .rotation = VDP1_ENV_ROTATION_0,
        .color_mode = VDP1_ENV_COLOR_MODE_RGB_PALETTE,
        .sprite_type = 0,
        .erase_color = COLOR_RGB1555(0, 0, 0, 0),
        .erase_points = {
            {0, 0},
            {RESOLUTION_WIDTH, RESOLUTION_HEIGHT}}};

    smpc_peripheral_init();
    cd_block_init();

    vdp2_tvmd_display_res_set(VDP2_TVMD_INTERLACE_NONE, VDP2_TVMD_HORZ_NORMAL_A,
                              VDP2_TVMD_VERT_240);

    vdp2_scrn_back_color_set(VDP2_VRAM_ADDR(3, 0x01FFFE),
                             COLOR_RGB1555(0, 0, 0, 0));

    vdp2_sprite_priority_set(0, 6);
    vdp1_env_set(&env);

    vdp_sync_vblank_out_set(_vblank_out_handler, NULL);

    cpu_intc_mask_set(0);
    vdp2_tvmd_display_set();
}
