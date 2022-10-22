#include <stdlib.h>
#include "browser.h"
#include "screen.h"
#include "fenrir/fenrir.h"
#include "vdp1.config.h"
#include "vdp2.config.h"
#include "ui.config.h"
#include "noise.h"


#define COVER_TEXTURE_ADDR (browser->texture_base + (FONT_CACHE_SIZE * 2))

static noise_cfg_t noise_cfg = {
    .cell_addr = NBG1_CELL_ADDR,
    .pattern_addr = NGB1_PATTERN_ADDR,
    .pal_addr = VDP2_CRAM_ADDR(0x100)};

void gamelist_theme_update(browser_t * browser)
{
    static fix16_t __cx2 = 0;
    static fix16_t __cy2 = 0;

    __cx2 += ui_config.screens.gamelist.fg.velocity_x;
    __cy2 += ui_config.screens.gamelist.fg.velocity_y;

    // animate bg
    if (ui_config.screens.gamelist.background.velocity_x)
        vdp2_scrn_scroll_x_update(VDP2_SCRN_NBG0, ui_config.screens.gamelist.background.velocity_x);
    if (ui_config.screens.gamelist.background.velocity_y)
        vdp2_scrn_scroll_y_update(VDP2_SCRN_NBG0, ui_config.screens.gamelist.background.velocity_y);
    /**
        // animate fg
        if (ui_config.screens.gamelist.fg.velocity_x)
            vdp2_scrn_scroll_x_update(VDP2_SCRN_NBG2, ui_config.screens.gamelist.fg.velocity_x);
        if (ui_config.screens.gamelist.fg.velocity_y)
            vdp2_scrn_scroll_y_update(VDP2_SCRN_NBG2, ui_config.screens.gamelist.fg.velocity_y);
            */
    vdp2_scrn_scroll_x_set(VDP2_SCRN_NBG2, __cx2);
    vdp2_scrn_scroll_y_set(VDP2_SCRN_NBG2, __cy2);
    if (ui_config.screens.gamelist.cover.enabled)
        noise_update(&noise_cfg);
}

void gamelist_theme_apply(browser_t * browser)
{
    /*****************************************************
     * Apply theme configuration
     ****************************************************/
    // noise theme
    noise_cfg.noise_palette = (uintptr_t)ui_config.screens.gamelist.cover.noise_palettes;
    noise_cfg.noise_palettes_sz = NOISE_PALETTE_SIZE * sizeof(int16_t);
    noise_cfg.cell_x = ui_config.screens.gamelist.cover.x / 8;
    noise_cfg.cell_y = ui_config.screens.gamelist.cover.y / 8;
    noise_cfg.cell_w = ui_config.screens.gamelist.cover.w / 8;
    noise_cfg.cell_h = ui_config.screens.gamelist.cover.h / 8;

    // palette colors
    color_rgb1555_t *pal = (color_rgb1555_t *)browser->pal_base;
    pal[BROWSER_ITEM_COLOR + 1] = ui_config.screens.gamelist.browser.item_colors.colors[0];
    pal[BROWSER_ITEM_COLOR + 2] = ui_config.screens.gamelist.browser.item_colors.colors[1];

    pal[BROWSER_FOCUSED_ITEM_COLOR + 1] = ui_config.screens.gamelist.browser.item_focused_colors.colors[0];
    pal[BROWSER_FOCUSED_ITEM_COLOR + 2] = ui_config.screens.gamelist.browser.item_focused_colors.colors[1];

    pal[32 + 1] = ui_config.screens.gamelist.browser.position_bar.color;

    // gouraud colors
    color_rgb1555_t *gouraud = (color_rgb1555_t *)browser->gouraud_base;
    for (int i = 0; i < 4; i++)
    {
        gouraud[i + 0] = ui_config.screens.gamelist.browser.item_colors.gouraud[i];
        gouraud[i + 16] = ui_config.screens.gamelist.browser.item_focused_colors.gouraud[i];
        gouraud[i + 24] = ui_config.screens.gamelist.browser.position_bar.gouraud[i];
    }

    // vdp2 color (each banks is 0x10)
    color_rgb1555_t *cram = (color_rgb1555_t *)(VDP2_CRAM_MODE_0_OFFSET(0, 0, 0) + VDP2_CRAM_LUT);
    cram[1] = ui_config.screens.gamelist.browser.item_colors.colors[1];
    cram[16 + 1] = ui_config.screens.gamelist.browser.item_focused_colors.colors[1];

    /*****************************************************
     * add preview area
     ****************************************************/
    if (ui_config.screens.gamelist.cover.enabled)
    {
        vdp1_cmdt_t *cmdt = &cmdt_list->cmdts[ORDER_BUFFER_SKIP];
        // build and enqueue the polygon
        const vdp1_cmdt_draw_mode_t draw_mode = {
            .raw = 0x0000,
            .bits.cc_mode = 0,
            .bits.color_mode = CMDT_PMOD_CM_RGB_32768_COLORS,
            .bits.trans_pixel_disable = false,
            .bits.pre_clipping_disable = true,
            .bits.end_code_disable = true};

        const int tex_w = 128;
        const int tex_h = 96;

        const vdp1_cmdt_color_bank_t color_bank = {
            .type_0.data.dc = 0};

        cmdt->cmd_xa = ui_config.screens.gamelist.cover.x;
        cmdt->cmd_ya = ui_config.screens.gamelist.cover.y;

        cmdt->cmd_xb = ui_config.screens.gamelist.cover.x + tex_w;
        cmdt->cmd_yb = ui_config.screens.gamelist.cover.y;

        cmdt->cmd_xc = ui_config.screens.gamelist.cover.x + tex_w;
        cmdt->cmd_yc = ui_config.screens.gamelist.cover.y + tex_h;

        cmdt->cmd_xd = ui_config.screens.gamelist.cover.x;
        cmdt->cmd_yd = ui_config.screens.gamelist.cover.y + tex_h;

        vdp1_cmdt_scaled_sprite_set(cmdt);
        vdp1_cmdt_param_color_mode1_set(cmdt, 0);
        vdp1_cmdt_param_gouraud_base_set(cmdt, 0);
        vdp1_cmdt_param_draw_mode_set(cmdt, draw_mode);
        vdp1_cmdt_param_size_set(cmdt, tex_w, tex_h);

        vdp1_cmdt_param_char_base_set(cmdt, COVER_TEXTURE_ADDR);

        noise_init(&noise_cfg);
    }

    if (!ui_config.screens.gamelist.cover.enabled)
    {
        vdp2_scrn_display_unset(VDP2_SCRN_NBG1_DISP);
    }
}

void gamelist_theme_destroy(browser_t * browser)
{
    if (ui_config.screens.gamelist.cover.enabled)
        noise_destroy(&noise_cfg);
}
