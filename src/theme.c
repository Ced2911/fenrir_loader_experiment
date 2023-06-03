#include "yaul.h"
#include "theme.h"

static ui_config_t *ui_config;
theme_cfg_t theme_cfg;

typedef struct
{
    ui_config_background_t *background;
    ui_config_background_t *foreground;
} current_theme_config_t;

static current_theme_config_t current_theme_config = {
    .background = NULL,
    .foreground = NULL};

#define SET_CURRENT_SCREEN(x)                            \
    {                                                    \
        current_theme_config.foreground = &x.foreground; \
        current_theme_config.background = &x.background; \
        theme_background_config_set(&x.foreground);      \
        theme_background_config_set(&x.background);      \
    }

/*****************************************************
 * theme
 ****************************************************/
void theme_init_vdp()
{
    // MEMORY_WRITE(16, VDP2(RAMCTL), 0x1301);
}

void theme_set_pattern(theme_scr_t scr, uint32_t pattern_offset)
{
#if 0
    vdp2_scrn_normal_map_t map;
    ui_config_t *ui_config = THEME_UI;

    switch (scr)
    {
    case THEME_SCR_NBG0:
        vdp2_set_plane_addr(&map,
                            pattern_offset + ui_config->vdp2.nbg0.pattern_addr,
                            0);
        vdp2_scrn_cell_format_set(&format_nbg0, &map);
        break;
    case THEME_SCR_NBG2:
        vdp2_set_plane_addr(&map,
                            pattern_offset + ui_config->vdp2.nbg2.pattern_addr,
                            0);
        vdp2_scrn_cell_format_set(&format_nbg2, &map);
        break;
    default:
        break;
    }
#endif
}

void theme_background_config_set(ui_config_background_t *b)
{
    // theme_set_pattern(b->screen, b->pattern_offset);

    // dbgio_printf("unknow %d-%d\n", b->screen, b->pattern_offset);
}

#define VDP2_REG_SCXIN1 (*(volatile uint16_t *)0x25F80080)
#define VDP2_REG_SCXDN1 (*(volatile uint16_t *)0x25F80082)
#define VDP2_REG_SCYIN1 (*(volatile uint16_t *)0x25F80084)
#define VDP2_REG_SCYDN1 (*(volatile uint16_t *)0x25F80086)

void theme_set_background(screens_type_t scr)
{
#if 0
    ui_config_t *ui_config = THEME_UI;

    vdp2_scrn_scroll_x_set(VDP2_SCRN_NBG0, 0);
    vdp2_scrn_scroll_y_set(VDP2_SCRN_NBG0, 0);

    vdp2_scrn_scroll_x_set(VDP2_SCRN_NBG2, 0);
    vdp2_scrn_scroll_y_set(VDP2_SCRN_NBG2, 0);

    switch (scr)
    {
    case screen_error_no_sd:
        SET_CURRENT_SCREEN(ui_config->screens.error_no_sd);
        break;
    case screen_gamelist:
        SET_CURRENT_SCREEN(ui_config->screens.gamelist);
        break;
    case screen_error_bad_filesystem:
        SET_CURRENT_SCREEN(ui_config->screens.error_bad_filesystem);
        break;

    default:
        break;
    }
    dbgio_flush();
#endif
    VDP2_REG_SCXIN1 = 10;
    // vdp2_scrn_scroll_x_set(VDP2_SCRN_NBG1, FIX16(100));
}

void theme_update()
{
    static int32_t x = 0;
    x += theme_cfg.gamelist_background.x;
    /*
    if (theme_cfg.gamelist_background.x)
        vdp2_scrn_scroll_x_update(VDP2_SCRN_NBG1, fix16_int32_from(theme_cfg.gamelist_background.x));
    if (theme_cfg.gamelist_background.y)
        vdp2_scrn_scroll_y_update(VDP2_SCRN_NBG1, fix16_int32_from(theme_cfg.gamelist_background.y));
*/
    vdp2_scrn_scroll_x_update(VDP2_SCRN_NBG1, theme_cfg.gamelist_background.x);
    vdp2_scrn_scroll_y_update(VDP2_SCRN_NBG1, theme_cfg.gamelist_background.y);
    // VDP2_REG_SCXIN1 = x;
}

void theme_init_bgm()
{
}

/*****************************************************/

#include "../assets/theme.h"

/*****************************************************
 * theme loader
 ****************************************************/

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

static ui_config_t *ui_config = (ui_config_t *)&theme;

void theme_ui_load()
{
    uint32_t config_sz = 0;
    theme_cfg_t *user_theme_cfg = theme_get_ressource(theme_bin, THEME_ID_CONFIG_V0, &config_sz);

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
    }
}

ui_config_t *theme_get_ui_config()
{
    static int theme_loaded = 0;
    if (theme_loaded == 0)
    {
        theme_ui_load();
        theme_loaded = 1;
    }
    return ui_config;
}
