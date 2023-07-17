#include <stdlib.h>
#include "browser.h"
#include "screens.h"
#include "fenrir/fenrir.h"
#include "vdp1.config.h"
#include "vdp2.config.h"
#include "theme.h"

#define _CPU_IN_SPINLOCK(x)         \
    do                              \
    {                               \
        cpu_sync_spinlock(0);       \
        x;                          \
        cpu_sync_spinlock_clear(0); \
    } while (0);

enum
{
    DEVICE_ICON_STATE_SD = 0,
    DEVICE_ICON_STATE_NO_SD,
    DEVICE_ICON_STATE_WIFI,
    DEVICE_ICON_STATE_NO_WIFI,
};

extern fenrir_config_t *fenrir_config;
extern status_sector_t *status_sector;
extern sd_dir_entry_t *sd_dir_entries;

extern void gamelist_theme_update(browser_t *browser);
extern void gamelist_theme_apply(browser_t *browser);
extern void gamelist_theme_destroy(browser_t *browser);

// @todo theme ?
typedef struct
{
    int last_selected_item;
    uintptr_t game_cover;
    int gamelist_source;
} gamelist_ctx_t;

static gamelist_ctx_t gamelist_ctx = {
    .last_selected_item = -1};

static void browser_input_callback(browser_t *browser);
static void browser_change_dir(browser_t *browser, int16_t id);
static void browser_get_item(browser_t *browser, uint32_t item, char *dst, int max_len);

static int cover_selected __uncached = -1;

static browser_t browser = {
    .count = 0,
    .file_per_page = 16,
    .input_handler = browser_input_callback,
    .get_item = browser_get_item};

static void browser_get_item(browser_t *browser, uint32_t item, char *dst, int max_len)
{
    if (item < browser->count)
    {
        // snprintf(dst, max_len, "item: %x %s", sd_dir_entries[item].flag, sd_dir_entries[item].filename);
        strncpy(dst, sd_dir_entries[item].filename, max_len);
        dst[max_len - 1] = 0;
    }
}

static void browser_change_dir(browser_t *browser, int16_t id)
{
    fenrir_select_direntry(id);
    fenrir_refresh_entries(fenrir_config, sd_dir_entries);

    browser->count = fenrir_config->hdr.count;
    browser->page = 0;
    browser->selected = 0;
    browser->old_page = -1;
    gamelist_ctx.last_selected_item = -1;
}

static void browser_change_source(browser_t *browser, int source)
{

    fenrir_set_gamelist_source(source);
    fenrir_select_direntry(DIR_ENTRY_ROOT);
    fenrir_refresh_entries(fenrir_config, sd_dir_entries);

    fenrir_read_configuration(fenrir_config);

    browser->count = fenrir_config->hdr.count;
    browser->page = 0;
    browser->selected = 0;
    browser->old_page = -1;

    gamelist_ctx.last_selected_item = -1;
    gamelist_ctx.gamelist_source = source;
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
            fenrir_launch_game(sd_dir_entries[browser->selected].id, fenrir_boot_direct);
        }
    }
    // btn b, go to parent directory
    else if (browser->digital.held.button.b)
    {
        browser_change_dir(browser, PARENT_DIRECTORY);
    }
    else if (browser->digital.held.button.start)
    {
        screens_select(screen_options);
    }
    // btn l / r switch between filesystem
    else if (browser->digital.held.button.l || browser->digital.held.button.r)
    {
        switch (gamelist_ctx.gamelist_source)
        {
        case fenrir_gamelist_source_sd:
            browser_change_source(browser, fenrir_gamelist_source_smb);
            break;
        case fenrir_gamelist_source_smb:
        default:
            browser_change_source(browser, fenrir_gamelist_source_sd);
            break;
        }
    }
}

static void _slave_entry(void)
{
    const uintptr_t addr = COVER_TEXTURE_ADDR;
    int selected = 0;

    _CPU_IN_SPINLOCK({ selected = cover_selected; })

    fenrir_get_cover(selected, (uint8_t *)addr);
}

static void gamelist_vbk()
{
    // smpc - input
    smpc_peripheral_intback_issue();
}
static void gamelist_update()
{
    // render browser
    browser_update(&browser);

    gamelist_theme_update(&browser);

    int state = DEVICE_ICON_STATE_SD;
    switch (gamelist_ctx.gamelist_source)
    {
    case fenrir_gamelist_source_sd:
        state = browser.count > 0 ? DEVICE_ICON_STATE_SD : DEVICE_ICON_STATE_NO_SD;
        break;
    case fenrir_gamelist_source_http:
        state = browser.count > 0 ? DEVICE_ICON_STATE_WIFI : DEVICE_ICON_STATE_NO_WIFI;
        break;
    }

    // if entry changed, load the cover
    if (ui_config->screens.gamelist.cover.enabled && browser.selected != gamelist_ctx.last_selected_item)
    {
        gamelist_ctx.last_selected_item = browser.selected;
        cover_selected = sd_dir_entries[browser.selected].id;
        cpu_dual_slave_notify();
    }

    // draw sd/wifi bar..
    {
        const int tex_w = 16;
        const int tex_h = 16;

        vdp1_cmdt_t *cmdt = &cmdt_list->cmdts[ORDER_BUFFER_DEV_ICON];
        vdp1_cmdt_char_base_set(cmdt, ICONS_TEXTURE_ADDR + state * (tex_w * tex_h * 2));
    }
}

static void gamelist_init()
{
    vdp2_scrn_display_set(0);
    gamelist_ctx.last_selected_item = -1;
    gamelist_ctx.gamelist_source = fenrir_gamelist_source_sd;

    /*****************************************************
     * Alloc ressources
     ****************************************************/
    gamelist_ctx.game_cover = (uintptr_t)malloc(FENRIR_COVER_SIZE);
    browser.texture_buffer = (uintptr_t)malloc(FONT_CACHE_SIZE);

    /*****************************************************
     * Setup browser textures
     ****************************************************/
    vdp1_vram_partitions_t vdp1_vram_partitions;
    vdp1_vram_partitions_get(&vdp1_vram_partitions);
    browser.texture_base = (uintptr_t)vdp1_vram_partitions.texture_base;
    browser.pal_base = (uintptr_t)vdp1_vram_partitions.clut_base;
    browser.gouraud_base = (uintptr_t)vdp1_vram_partitions.gouraud_base;

    /*****************************************************
     * Read gamelist
     ****************************************************/
    fenrir_read_status_sector(status_sector);
    fenrir_refresh_entries(fenrir_config, sd_dir_entries);

    browser.count = fenrir_config->hdr.count;

    /*****************************************************
     * themes
     ****************************************************/
    // browser theme
    memcpy(&browser.browser_ui_config, &ui_config->screens.gamelist.browser, sizeof(ui_config_browser_t));
    gamelist_theme_apply(&browser);

    // init browser
    browser_init(&browser);
    browser_update(&browser);

    // setup vblank
    vdp_sync_vblank_out_set(gamelist_vbk, NULL);

    // setup slave
    cpu_dual_slave_set(_slave_entry);

    for (int i = 0; i < 8; i++)
        vdp2_sprite_priority_set(i, 6);

    vdp2_scrn_priority_set(VDP2_SCRN_NBG1, 2);
    vdp2_scrn_priority_set(VDP2_SCRN_NBG2, 3);
    vdp2_scrn_display_set(VDP2_SCRN_DISP_NBG1 | VDP2_SCRN_DISPTP_NBG2);
}

static void gamelist_destroy()
{
    // remove slave usage
    cpu_dual_slave_clear();

    // remove vblank
    vdp_sync_vblank_out_clear();

    // delete theme resc
    gamelist_theme_destroy(&browser);

    // free ressources
    free((void *)gamelist_ctx.game_cover);
    free((void *)browser.texture_buffer);

    memset(COVER_TEXTURE_ADDR, 0, FENRIR_COVER_SIZE);

    vdp1_reset();

    browser_destroy(&browser);
}

screen_t gamelist_screen = {
    .type = screen_gamelist,
    .init = gamelist_init,
    .update = gamelist_update,
    .destroy = gamelist_destroy,
};
