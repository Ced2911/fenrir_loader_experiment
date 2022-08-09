#include <stdlib.h>
#include "browser.h"
#include "screen.h"
#include "fenrir/fenrir.h"
#include "vdp1.config.h"
#include "vdp2.config.h"
#include "ui.config.h"
#include "noise.h"

#define _CPU_IN_SPINLOCK(x)         \
    {                               \
        cpu_sync_spinlock(0);       \
        x;                          \
        cpu_sync_spinlock_clear(0); \
    }

extern sd_dir_t *sd_dir;
extern status_sector_t *status_sector;
extern sd_dir_entry_t *sd_dir_entries;

// @todo theme ?
typedef struct
{
    fix16_t bg_x;
    fix16_t bg_y;
    fix16_t bg_v_x;
    fix16_t bg_v_y;
    int last_selected_item;
    uintptr_t game_cover;
} gamelist_ctx_t;

static gamelist_ctx_t gamelist_ctx = {
    .bg_x = FIX16(512),
    .bg_y = FIX16(0),
    .bg_v_x = FIX16(0.7),
    .bg_v_y = FIX16(-0.7)};

static void browser_input_callback(browser_t *browser);
static void browser_change_dir(browser_t *browser, int16_t id);
static void browser_get_item(browser_t *browser, uint32_t item, char *dst, int max_len);

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

// 00c040
// 00f000
// 008060
// 00f0f0

noise_cfg_t noise_cfg = {
    .cell_addr = NBG1_CELL_ADDR,
    .pattern_addr = NGB1_PATTERN_ADDR,
    .pal_addr = VDP2_CRAM_ADDR(0x100),
    .cell_w = 128 / 8,
    .cell_h = 96 / 8,
    .cell_x = 200 / 8,
    .cell_y = 100 / 8};

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
    gamelist_ctx.last_selected_item = -1;
}

static void browser_input_callback(browser_t *browser)
{
    // if button A, browse dir or launch game
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
    // btn b, go to parent directory
    else if (browser->digital.held.button.b)
    {
        browser_change_dir(browser, PARENT_DIRECTORY);
    }
}

static void _slave_entry(void)
{
    int item = browser.selected;

    _CPU_IN_SPINLOCK({ gamelist_ctx.last_selected_item = browser.selected; })

    uintptr_t addr = browser.texture_base + FONT_CACHE_SIZE;

    fenrir_get_cover(item, addr /*gamelist_ctx.game_cover*/);
    // vdp_dma_enqueue(RBG0_BITMAP_ADDR, gamelist_ctx.game_cover, FENRIR_COVER_SIZE);
    // scu_dma_transfer(0, RBG0_BITMAP_ADDR, gamelist_ctx.game_cover, FENRIR_COVER_SIZE);

    // cpu_dual_master_notify();
}

static void gamelist_vbk()
{
    // smpc - input
    smpc_peripheral_intback_issue();
}

static void gamelist_update()
{
    // animate bg
    vdp2_scrn_scroll_x_set(VDP2_SCRN_NBG0, gamelist_ctx.bg_x);
    vdp2_scrn_scroll_y_set(VDP2_SCRN_NBG0, gamelist_ctx.bg_y);

    gamelist_ctx.bg_x += gamelist_ctx.bg_v_x;
    gamelist_ctx.bg_y += gamelist_ctx.bg_v_y;

    noise_update(&noise_cfg);

    // render browser
    browser_update(&browser);

    // if entry changed, load the cover
    if (browser.selected != gamelist_ctx.last_selected_item)
    {
        cpu_dual_slave_notify();
    }
}

static void gamelist_init()
{
    gamelist_ctx.last_selected_item = -1;

    // init ressources
    gamelist_ctx.game_cover = (uintptr_t)malloc(FENRIR_COVER_SIZE);
    browser.texture_buffer = (uintptr_t)malloc(FONT_CACHE_SIZE);
    noise_init(&noise_cfg);

    // setup browser textures
    vdp1_vram_partitions_t vdp1_vram_partitions;
    vdp1_vram_partitions_get(&vdp1_vram_partitions);
    browser.texture_base = (uintptr_t)vdp1_vram_partitions.texture_base;
    browser.pal_base = (uintptr_t)vdp1_vram_partitions.clut_base;
    browser.gouraud_base = (uintptr_t)vdp1_vram_partitions.gouraud_base;

    // Read gamelist
    fenrir_read_status_sector(status_sector);
    fenrir_refresh_entries(sd_dir, sd_dir_entries);

    browser.count = sd_dir->hdr.count;

    // init browser
    browser_init(&browser);
    browser_update(&browser);

    // setup vblank
    vdp_sync_vblank_out_set(gamelist_vbk, NULL);

    // setup slave
    cpu_dual_slave_set(_slave_entry);

    // add preview area
    vdp1_cmdt_t *cmdt = &cmdt_list->cmdts[ORDER_BUFFER_SKIP];
    // build and enqueue the polygon
    const vdp1_cmdt_draw_mode_t draw_mode = {
        .raw = 0x0000,
        .bits.cc_mode = 0,                                // todo: enable gouraud
        .bits.color_mode = CMDT_PMOD_CM_RGB_32768_COLORS, // 16color 4bit
        .bits.trans_pixel_disable = false,
        .bits.pre_clipping_disable = true,
        .bits.end_code_disable = true};

    const vdp1_cmdt_color_bank_t color_bank = {
        .type_0.data.dc = 0};

    cmdt->cmd_xa = 80;
    cmdt->cmd_ya = 120;

    vdp1_cmdt_normal_sprite_set(cmdt);
    vdp1_cmdt_param_color_mode1_set(cmdt, 0);
    vdp1_cmdt_param_gouraud_base_set(cmdt, 0);
    vdp1_cmdt_param_draw_mode_set(cmdt, draw_mode);
    vdp1_cmdt_param_size_set(cmdt, 128, 96);

    vdp1_cmdt_param_char_base_set(cmdt, browser.texture_base + FONT_CACHE_SIZE);
}

static void gamelist_destroy()
{
    // remove slave usage
    cpu_dual_slave_clear();

    // remove vblank
    vdp_sync_vblank_out_clear();

    // free ressources
    free((void *)gamelist_ctx.game_cover);
    free((void *)browser.texture_buffer);
    noise_destroy(&noise_cfg);
}

screen_t gamelist_screen = {
    .init = gamelist_init,
    .update = gamelist_update,
    .destroy = gamelist_destroy,
};
