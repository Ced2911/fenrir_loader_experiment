#include <yaul.h>
#include <stdio.h>
#include <stdlib.h>
#include "fenrir/fenrir.h"
#include "vdp1.config.h"
#include "font/font.h"
#include "screens/browser.h"
#include "../assets/bitmap.h"
#include "../assets/test.h"

#define RESOLUTION_WIDTH (352)
#define RESOLUTION_HEIGHT (224)

sd_dir_t *sd_dir;
status_sector_t *status_sector;
sd_dir_entry_t *sd_dir_entries;

static void browser_get_item(browser_t *browser, uint32_t item, char *dst, int max_len)
{
    if (item < sd_dir->hdr.count)
    {
        // sprintf(dst, "item: %x", sd_dir_entries[item].flag);
        strncpy(dst, sd_dir_entries[item].filename, max_len);
    }
}

static void browser_change_dir(browser_t *browser, int16_t id)
{
    fenrir_select_direntry(id);
    fenrir_refresh_entries(sd_dir, sd_dir_entries);
    browser->count = sd_dir->hdr.count;
    browser->page = 0;
    browser->selected = 0;
    browser->old_page = -1;
}

static void browser_input_callback(browser_t *browser)
{
    if (browser->digital.held.button.a)
    {
        if (sd_dir_entries[browser->selected].flag & SD_DIR_FLAG_DIRECTORY)
        {
            browser_change_dir(browser, sd_dir_entries[browser->selected].id);
        }
        else
        {
            fenrir_launch_game(sd_dir_entries[browser->selected].id, fenrir_boot_cd_player);
        }
    }
    else if (browser->digital.held.button.b)
    {
        browser_change_dir(browser, PARENT_DIRECTORY);
    }
}

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
    //  dbgio_init();
    // dbgio_dev_default_init(DBGIO_DEV_VDP2_ASYNC);
    // dbgio_dev_font_load();

    vdp1_init();
    font_to_vram();
    vdp2_init();

    sd_dir = (sd_dir_t *)zalloc(sizeof(sd_dir_t));
    status_sector = (status_sector_t *)zalloc(sizeof(status_sector_t));
    sd_dir_entries = (sd_dir_entry_t *)zalloc(sizeof(sd_dir_entry_t) * 2500);

    fenrir_read_status_sector(status_sector);
    fenrir_refresh_entries(sd_dir, sd_dir_entries);

    browser_t browser = {
        .count = sd_dir->hdr.count,
        .file_per_page = 12,
        .input_handler = browser_input_callback,
        .get_item = browser_get_item,
        .browser_ui_config = {
            .font_color = 0,
            .font_focus_color = 1,
            .line_height = 16,
            .font_height = 9,
            .browser_w = 120,
            .x_offset = 20,
            .y_offset = 24,
            .bar = {
                .visible = 1,
                .x = 12,
                .h = 12,
                .w = 5,
            },
        }};

    vdp1_vram_partitions_t vdp1_vram_partitions;
    vdp1_vram_partitions_get(&vdp1_vram_partitions);
    browser.texture_base = (uintptr_t)vdp1_vram_partitions.texture_base;
    browser.pal_base = (uintptr_t)vdp1_vram_partitions.clut_base;
    browser.gouraud_base = (uintptr_t)vdp1_vram_partitions.gouraud_base;

    // set palette color
    uint32_t pal_addr = (0x25C00000 | (uint32_t)browser.pal_base);
    color_rgb1555_t *color_0 = (color_rgb1555_t *)pal_addr;
    color_rgb1555_t *color_1 = (color_rgb1555_t *)(pal_addr + 16);
    for (int i = 0; i < 32; i++)
    {
        color_0[i] = COLOR_RGB1555(1, 0x0F, i, 0x1F);
        color_1[i] = COLOR_RGB1555(1, 0x0F, 0x0F, i);
    }

    // set gauraud color
    uint32_t g_addr = (0x25C00000 | (uint32_t)browser.gouraud_base);
    uint16_t *gouraud = (uint16_t *)g_addr;
    memset(gouraud, 0xff, 4 * sizeof(uint16_t));

    // animate bg
    fix16_t bg_x = FIX16(512);
    fix16_t bg_y = FIX16(0);
    fix16_t bg_v_x = FIX16(0.7);
    fix16_t bg_v_y = FIX16(-0.7);

    browser_init(&browser);

    while (1)
    {
        browser_update(&browser);

        vdp1_sync_render();
        vdp1_sync();
        vdp2_sync();
        vdp2_sync_wait();

        vdp2_scrn_scroll_x_set(VDP2_SCRN_NBG0, bg_x);
        vdp2_scrn_scroll_y_set(VDP2_SCRN_NBG0, bg_y);

        bg_x += bg_v_x;
        bg_y += bg_v_y;
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
