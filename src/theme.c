#include "yaul.h"
#include "theme.h"

static ui_config_t *ui_config;

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
    ui_config_t *ui_config = theme_get_ui_config();

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

void theme_set_background(screens_type_t scr)
{
#if 0
    ui_config_t *ui_config = theme_get_ui_config();

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
}

void theme_update()
{
#if 0
    ui_config_t *ui_config = theme_get_ui_config();
    ui_config_background_t *background = current_theme_config.background;
    ui_config_background_t *foreground = current_theme_config.foreground;

    // animate bg
    if (background && background->velocity_x)
        vdp2_scrn_scroll_x_update(VDP2_SCRN_NBG0, background->velocity_x);
    if (background && background->velocity_y)
        vdp2_scrn_scroll_y_update(VDP2_SCRN_NBG0, background->velocity_y);

    if (foreground && foreground->velocity_x)
        vdp2_scrn_scroll_x_update(VDP2_SCRN_NBG2, foreground->velocity_x);
    if (foreground && foreground->velocity_y)
        vdp2_scrn_scroll_y_update(VDP2_SCRN_NBG2, foreground->velocity_y);
#endif
}

void theme_init_bgm()
{
}

/*****************************************************
 * theme loader
 ****************************************************/
#include "themes/demo/theme.demo.h"
static ui_config_t *ui_config = (ui_config_t *)theme;
ui_config_t *theme_get_ui_config()
{
    return ui_config;
}
