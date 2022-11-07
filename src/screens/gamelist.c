#include <stdlib.h>
#include "browser.h"
#include "screen.h"
#include "fenrir/fenrir.h"
#include "vdp1.config.h"
#include "vdp2.config.h"
#include "theme.h"
#include "noise.h"

#define COVER_TEXTURE_ADDR (browser.texture_base + (FONT_CACHE_SIZE * 2))

#define _CPU_IN_SPINLOCK(x)         \
    do                              \
    {                               \
        cpu_sync_spinlock(0);       \
        x;                          \
        cpu_sync_spinlock_clear(0); \
    } while (0);

extern sd_dir_t *sd_dir;
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
} gamelist_ctx_t;

static gamelist_ctx_t gamelist_ctx = {
    .last_selected_item = -1};

static void browser_input_callback(browser_t *browser);
static void browser_change_dir(browser_t *browser, int16_t id);
static void browser_get_item(browser_t *browser, uint32_t item, char *dst, int max_len);

static browser_t browser = {
    .count = 0,
    .file_per_page = 12,
    .input_handler = browser_input_callback,
    .get_item = browser_get_item};

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
            fenrir_launch_game(sd_dir_entries[browser->selected].id, fenrir_boot_direct);
        }
    }
    // btn b, go to parent directory
    else if (browser->digital.held.button.b)
    {
        browser_change_dir(browser, PARENT_DIRECTORY);
    }
}

static int cover_selected __uncached = -1;

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

    // if entry changed, load the cover
    if (theme_get_ui_config()->screens.gamelist.cover.enabled && browser.selected != gamelist_ctx.last_selected_item)
    {
        gamelist_ctx.last_selected_item = browser.selected;
        cover_selected = sd_dir_entries[browser.selected].id;
        // cpu_dual_slave_notify();
    }
}

static void gamelist_init()
{
    gamelist_ctx.last_selected_item = -1;

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
    fenrir_refresh_entries(sd_dir, sd_dir_entries);

    browser.count = sd_dir->hdr.count;

    /*****************************************************
     * themes
     ****************************************************/
    // browser theme
    memcpy(&browser.browser_ui_config, &theme_get_ui_config()->screens.gamelist.browser, sizeof(ui_config_browser_t));
    gamelist_theme_apply(&browser);

    // init browser
    browser_init(&browser);
    browser_update(&browser);

    // setup vblank
    vdp_sync_vblank_out_set(gamelist_vbk, NULL);

    // setup slave
    cpu_dual_slave_set(_slave_entry);
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
}

screen_t gamelist_screen = {
    .init = gamelist_init,
    .update = gamelist_update,
    .destroy = gamelist_destroy,
};
