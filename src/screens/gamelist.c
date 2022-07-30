#include <stdlib.h>
#include "browser.h"
#include "screen.h"
#include "fenrir/fenrir.h"

// @todo theme ?
typedef struct
{
    fix16_t bg_x;
    fix16_t bg_y;
    fix16_t bg_v_x;
    fix16_t bg_v_y;
} gamelist_ctx_t;

extern sd_dir_t *sd_dir;
extern status_sector_t *status_sector;
extern sd_dir_entry_t *sd_dir_entries;

gamelist_ctx_t gamelist_ctx;

static void browser_get_item(browser_t *browser, uint32_t item, char *dst, int max_len)
{
    if (item < browser->count)
    {
        // sprintf(dst, "item: %x", sd_dir_entries[item].flag);
        strncpy(dst, sd_dir_entries[item].filename, max_len);
        dst[max_len - 1] = 0;
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

static browser_t browser = {
    .count = 0,
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

static void gamelist_init()
{
    // setup browser textures
    vdp1_vram_partitions_t vdp1_vram_partitions;
    vdp1_vram_partitions_get(&vdp1_vram_partitions);
    browser.texture_base = (uintptr_t)vdp1_vram_partitions.texture_base;
    browser.pal_base = (uintptr_t)vdp1_vram_partitions.clut_base;
    browser.gouraud_base = (uintptr_t)vdp1_vram_partitions.gouraud_base;

    // setup vdp2
    gamelist_ctx.bg_x = FIX16(512);
    gamelist_ctx.bg_y = FIX16(0);
    gamelist_ctx.bg_v_x = FIX16(0.7);
    gamelist_ctx.bg_v_y = FIX16(-0.7);

    // Read gamelist
    fenrir_read_status_sector(status_sector);
    fenrir_refresh_entries(sd_dir, sd_dir_entries);

    browser.count = sd_dir->hdr.count;
    browser.texture_buffer = (uintptr_t)malloc(browser.file_per_page * (512 * 16));

    browser_init(&browser);
    browser_update(&browser);
}

static void gamelist_update()
{
    // animate bg
    vdp2_scrn_scroll_x_set(VDP2_SCRN_NBG0, gamelist_ctx.bg_x);
    vdp2_scrn_scroll_y_set(VDP2_SCRN_NBG0, gamelist_ctx.bg_y);

    gamelist_ctx.bg_x += gamelist_ctx.bg_v_x;
    gamelist_ctx.bg_y += gamelist_ctx.bg_v_y;

    // render browser
    browser_update(&browser);
}

screen_t gamelist_screen = {
    .init = gamelist_init,
    .update = gamelist_update,
};
