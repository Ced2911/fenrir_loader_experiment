
#include "yaul.h"
#include <stdlib.h>

#include "theme.h"

theme_cfg_t theme_cfg;

//

static ui_config_t theme = {
    // browser
    .screens.gamelist.browser.line_height = 10,
    .screens.gamelist.browser.x = 20,
    .screens.gamelist.browser.y = 18,
    .screens.gamelist.browser.w = 172,
    .screens.gamelist.browser.h = 200,

    .screens.gamelist.browser.item_colors.colors = {
        RGB1555_INITIALIZER(1, 17, 17, 17),
        RGB1555_INITIALIZER(1, 1, 1, 1),
    },
    .screens.gamelist.browser.item_colors.gouraud = {{.raw = 51794}, {.raw = 51794}, {.raw = 41224}, {.raw = 41224}},

    .screens.gamelist.browser.item_focused_colors.colors = {
        RGB1555_INITIALIZER(1, 15, 15, 15),
        RGB1555_INITIALIZER(1, 3, 3, 3),
    },
    .screens.gamelist.browser.item_focused_colors.gouraud = {{.raw = 51794}, {.raw = 51794}, {.raw = 41224}, {.raw = 41224}},

    // cover
    .screens.gamelist.cover.enabled = 1,
    .screens.gamelist.cover.x = 200,
    .screens.gamelist.cover.y = 24,
    .screens.gamelist.cover.w = 128,
    .screens.gamelist.cover.h = 96,
};

ui_config_t const *ui_config = (ui_config_t *)&theme;

void theme_update()
{
    vdp2_scrn_scroll_x_update(VDP2_SCRN_NBG1, theme_cfg.gamelist_background.x);
    vdp2_scrn_scroll_y_update(VDP2_SCRN_NBG1, theme_cfg.gamelist_background.y);
}

void theme_init_bgm()
{
}

/*****************************************************
 * theme loader
 ****************************************************/
uint8_t *theme_bin;

static void theme_load_from_disc()
{

    cdfs_filelist_t cdfs_filelist;
    cdfs_filelist_entry_t cdfs_filelist_root;
    cdfs_filelist_entry_t *cdfs_filelist_entries;

    cdfs_filelist_entries = cdfs_entries_alloc(50);
    cdfs_filelist_default_init(&cdfs_filelist, cdfs_filelist_entries, 50);
    cdfs_filelist_root_read(&cdfs_filelist);

    fad_t theme_starting_fad = 0;
    uint16_t theme_sector_count = 0;

    for (int i = 0; i < cdfs_filelist.entries_count; i++)
    {
        if (strcmp(cdfs_filelist.entries[i].name, "THEME.BIN") == 0)
        {
            theme_starting_fad = cdfs_filelist.entries[i].starting_fad;
            theme_sector_count = cdfs_filelist.entries[i].sector_count;
            break;
        }
    }

    if (theme_starting_fad && theme_sector_count)
    {
        theme_bin = (uint8_t *)malloc(theme_sector_count * CDFS_SECTOR_SIZE);
        cd_block_sectors_read(theme_starting_fad, theme_bin, theme_sector_count * CDFS_SECTOR_SIZE);
    }
    else
    {
        theme_bin = (uint8_t *)malloc(sizeof(theme_bin_t));
        memset(theme_bin, 0, sizeof(theme_bin_t));
    }

    cdfs_entries_free(cdfs_filelist_entries);
}

void theme_ui_load()
{
    uint32_t config_sz = 0;

    theme_load_from_disc();

    theme_cfg_t *user_theme_cfg = (theme_cfg_t *)theme_get_ressource(theme_bin, THEME_ID_CONFIG_V0, &config_sz);


    // theme_cfg->gamelist_cover.area.
    if (user_theme_cfg)
    {
        memcpy(&theme_cfg, user_theme_cfg, sizeof(theme_cfg_t));

        theme.screens.gamelist.browser.x = user_theme_cfg->gamelist_browser.area.x;
        theme.screens.gamelist.browser.y = user_theme_cfg->gamelist_browser.area.y;
        theme.screens.gamelist.browser.w = user_theme_cfg->gamelist_browser.area.w;
        theme.screens.gamelist.browser.h = user_theme_cfg->gamelist_browser.area.h;
        theme.screens.gamelist.browser.line_height = user_theme_cfg->gamelist_browser.line_height;

        theme.screens.gamelist.cover.x = user_theme_cfg->gamelist_cover.area.x;
        theme.screens.gamelist.cover.y = user_theme_cfg->gamelist_cover.area.y;
        theme.screens.gamelist.cover.w = user_theme_cfg->gamelist_cover.area.w;
        theme.screens.gamelist.cover.h = user_theme_cfg->gamelist_cover.area.h;
        theme.screens.gamelist.cover.enabled = user_theme_cfg->gamelist_cover.enabled;

        theme.screens.gamelist.browser.item_colors.colors[0] = user_theme_cfg->gamelist_browser.item.color;
        theme.screens.gamelist.browser.item_colors.colors[1] = user_theme_cfg->gamelist_browser.item.shadow;
        theme.screens.gamelist.browser.item_colors.gouraud[0] = user_theme_cfg->gamelist_browser.item.gradients[0];
        theme.screens.gamelist.browser.item_colors.gouraud[1] = user_theme_cfg->gamelist_browser.item.gradients[1];
        theme.screens.gamelist.browser.item_colors.gouraud[2] = user_theme_cfg->gamelist_browser.item.gradients[2];
        theme.screens.gamelist.browser.item_colors.gouraud[3] = user_theme_cfg->gamelist_browser.item.gradients[3];

        theme.screens.gamelist.browser.item_focused_colors.colors[0] = user_theme_cfg->gamelist_browser.focused.color;
        theme.screens.gamelist.browser.item_focused_colors.colors[1] = user_theme_cfg->gamelist_browser.focused.shadow;
        theme.screens.gamelist.browser.item_focused_colors.gouraud[0] = user_theme_cfg->gamelist_browser.focused.gradients[0];
        theme.screens.gamelist.browser.item_focused_colors.gouraud[1] = user_theme_cfg->gamelist_browser.focused.gradients[1];
        theme.screens.gamelist.browser.item_focused_colors.gouraud[2] = user_theme_cfg->gamelist_browser.focused.gradients[2];
        theme.screens.gamelist.browser.item_focused_colors.gouraud[3] = user_theme_cfg->gamelist_browser.focused.gradients[3];


        theme.screens.gamelist.browser.device_icon.x = user_theme_cfg->gamelist_device_icon.x;
        theme.screens.gamelist.browser.device_icon.y = user_theme_cfg->gamelist_device_icon.y;
    }
}
