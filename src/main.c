#include <yaul.h>
#include <stdio.h>
#include <stdlib.h>
#include "fenrir/fenrir.h"
#include "vdp1.config.h"
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
    vdp_dma_enqueue(cpd, bitmap, bitmapLength);
}

static void copy_palette(const vdp2_scrn_bitmap_format_t *format)
{
    uint16_t *color_palette;
    color_palette = (uint16_t *)format->color_palette;
    vdp_dma_enqueue(color_palette, pal, palLength);
}

void vdp2_init()
{
    vdp2_scrn_bitmap_format_t format;
    memset(&format, 0x00, sizeof(format));

    format.scroll_screen = VDP2_SCRN_NBG0;
    format.cc_count = VDP2_SCRN_CCC_PALETTE_256;
    format.bitmap_pattern = (uint32_t)VDP2_VRAM_ADDR(0, 0x00000);
    format.color_palette = (uint32_t)VDP2_CRAM_MODE_0_OFFSET(0, 0, 0);
    format.bitmap_size.width = 1024;
    format.bitmap_size.height = 512;
    format.rp_mode = 0;
    format.sf_type = VDP2_SCRN_SF_TYPE_NONE;
    format.sf_code = VDP2_SCRN_SF_CODE_A;
    format.sf_mode = 0;

    vdp2_vram_cycp_t vram_cycp;

    vram_cycp.pt[0].t0 = VDP2_VRAM_CYCP_PNDR_NBG0;
    vram_cycp.pt[0].t1 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[0].t2 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[0].t3 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[0].t4 = VDP2_VRAM_CYCP_CHPNDR_NBG0;
    vram_cycp.pt[0].t5 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[0].t6 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[0].t7 = VDP2_VRAM_CYCP_NO_ACCESS;

    vram_cycp.pt[1].t0 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[1].t1 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[1].t2 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[1].t3 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[1].t4 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[1].t5 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[1].t6 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[1].t7 = VDP2_VRAM_CYCP_NO_ACCESS;

    vram_cycp.pt[2].t0 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[2].t1 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[2].t2 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[2].t3 = VDP2_VRAM_CYCP_NO_ACCESS;
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

    copy_bitmap_data(&format);
    copy_palette(&format);

    vdp2_scrn_bitmap_format_set(&format);
    vdp2_scrn_priority_set(VDP2_SCRN_NBG0, 2);
    vdp2_scrn_display_set(VDP2_SCRN_NBG0_DISP);
    vdp2_scrn_scroll_x_set(VDP2_SCRN_NBG0, FIX16(512));
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
    smpc_peripheral_init();
    cd_block_init();

    static const struct vdp1_env env = {
        .bpp = VDP1_ENV_BPP_16,
        .rotation = VDP1_ENV_ROTATION_0,
        .color_mode = VDP1_ENV_COLOR_MODE_RGB_PALETTE,
        .sprite_type = 0,
        .erase_color = COLOR_RGB1555(0, 0, 0, 0),
        .erase_points = {
            {0, 0},
            {RESOLUTION_WIDTH, RESOLUTION_HEIGHT}}};

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
