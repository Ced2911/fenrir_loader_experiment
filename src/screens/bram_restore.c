#include <stdlib.h>
#include "browser.h"
#include "screens.h"
#include "fenrir/fenrir.h"
#include "vdp1.config.h"
#include "vdp2.config.h"
#include "theme.h"

extern fenrir_config_t *fenrir_config;
extern status_sector_t *status_sector;
extern sd_dir_entry_t *sd_dir_entries;

static void browser_input_callback(browser_t *browser);
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

static void browser_input_callback(browser_t *browser)
{
    // if button A, browse dir or launch game
    if (browser->digital.held.button.a)
    {
    }
    // btn b, go to parent directory
    else if (browser->digital.held.button.b)
    {
    }
    else if (browser->digital.held.button.start)
    {
        screens_select(screen_options);
    }
}

static void __vbk()
{
    // smpc - input
    smpc_peripheral_intback_issue();
}
static void __update()
{
    // render browser
    browser_update(&browser);
}

static void __init()
{
    /*****************************************************
     * Alloc ressources
     ****************************************************/
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
    browser.count = fenrir_config->hdr.count;

    /*****************************************************
     * themes
     ****************************************************/
    // browser theme
    memcpy(&browser.browser_ui_config, &ui_config->screens.gamelist.browser, sizeof(ui_config_browser_t));

    // init browser
    browser_init(&browser);
    browser_update(&browser);

    // setup vblank
    vdp_sync_vblank_out_set(__vbk, NULL);

    vdp2_scrn_display_set(VDP2_SCRN_DISPTP_NBG0 | VDP2_SCRN_DISPTP_NBG2);
}

static void __destroy()
{
    // remove slave usage
    cpu_dual_slave_clear();

    // remove vblank
    vdp_sync_vblank_out_clear();

    free((void *)browser.texture_buffer);
    browser_destroy(&browser);
}

screen_t bram_restore_screen = {
    .type = screen_restore_bram,
    .init = __init,
    .update = __update,
    .destroy = __destroy,
};
