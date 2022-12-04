#include <stdlib.h>
#include "browser.h"
#include "screen.h"
#include "fenrir/fenrir.h"
#include "vdp1.config.h"
#include "vdp2.config.h"
#include "../assets/vdp2.h"
#include "theme.h"
#include "noise.h"

#define COVER_TEXTURE_ADDR (browser->texture_base + (FONT_CACHE_SIZE * 2))

static noise_cfg_t noise_cfg = {};

void gamelist_theme_update(browser_t *browser)
{
    ui_config_t *theme = theme_get_ui_config();
    // if (theme->screens.gamelist.cover.enabled)
    //    noise_update(&noise_cfg);
}

void gamelist_theme_apply(browser_t *browser)
{
    /*****************************************************
     * Apply theme configuration
     ****************************************************/
    // noise theme
    ui_config_t *theme = theme_get_ui_config();

    noise_cfg.cell_addr = theme->vdp2.nbg1.cell_addr,
    noise_cfg.pattern_addr = theme->vdp2.nbg1.pattern_addr,
    noise_cfg.pal_addr = theme->vdp2.nbg1.pal_addr;

    noise_cfg.noise_palette = (uintptr_t)theme->screens.gamelist.cover.noise_palettes;
    noise_cfg.noise_palettes_sz = NOISE_PALETTE_SIZE * sizeof(int16_t);
    noise_cfg.cell_x = theme->screens.gamelist.cover.x / 8;
    noise_cfg.cell_y = theme->screens.gamelist.cover.y / 8;
    noise_cfg.cell_w = theme->screens.gamelist.cover.w / 8;
    noise_cfg.cell_h = theme->screens.gamelist.cover.h / 8;

    // palette colors
    rgb1555_t *pal = (rgb1555_t *)browser->pal_base;
    pal[BROWSER_ITEM_COLOR + 1] = theme->screens.gamelist.browser.item_colors.colors[0];
    pal[BROWSER_ITEM_COLOR + 2] = theme->screens.gamelist.browser.item_colors.colors[1];

    pal[BROWSER_FOCUSED_ITEM_COLOR + 1] = theme->screens.gamelist.browser.item_focused_colors.colors[0];
    pal[BROWSER_FOCUSED_ITEM_COLOR + 2] = theme->screens.gamelist.browser.item_focused_colors.colors[1];

    pal[32 + 1] = theme->screens.gamelist.browser.position_bar.color[0];

    // gouraud colors
    rgb1555_t *gouraud = (rgb1555_t *)browser->gouraud_base;
    for (int i = 0; i < 4; i++)
    {
        gouraud[i + 0] = theme->screens.gamelist.browser.item_colors.gouraud[i];
        gouraud[i + 16] = theme->screens.gamelist.browser.item_focused_colors.gouraud[i];
        gouraud[i + 24] = theme->screens.gamelist.browser.position_bar.gouraud[i];
    }

    // vdp2 color (each banks is 0x10)
    rgb1555_t *cram = (rgb1555_t *)(VDP2_CRAM_MODE_0_OFFSET(0, 0, 0) + VDP2_CRAM_LUT);
    cram[1] = theme->screens.gamelist.browser.item_colors.colors[1];
    cram[16 + 1] = theme->screens.gamelist.browser.item_focused_colors.colors[1];

    /*****************************************************
     * add preview area
     ****************************************************/
    if (theme->screens.gamelist.cover.enabled)
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

        cmdt->cmd_xa = theme->screens.gamelist.cover.x;
        cmdt->cmd_ya = theme->screens.gamelist.cover.y;

        cmdt->cmd_xb = theme->screens.gamelist.cover.x + tex_w;
        cmdt->cmd_yb = theme->screens.gamelist.cover.y;

        cmdt->cmd_xc = theme->screens.gamelist.cover.x + tex_w;
        cmdt->cmd_yc = theme->screens.gamelist.cover.y + tex_h;

        cmdt->cmd_xd = theme->screens.gamelist.cover.x;
        cmdt->cmd_yd = theme->screens.gamelist.cover.y + tex_h;

        vdp1_cmdt_scaled_sprite_set(cmdt);
        vdp1_cmdt_param_color_mode1_set(cmdt, 0);
        vdp1_cmdt_param_gouraud_base_set(cmdt, 0);
        vdp1_cmdt_param_draw_mode_set(cmdt, draw_mode);
        vdp1_cmdt_param_size_set(cmdt, tex_w, tex_h);

        vdp1_cmdt_param_char_base_set(cmdt, COVER_TEXTURE_ADDR);

        // noise_init(&noise_cfg);
    }

    if (!theme->screens.gamelist.cover.enabled)
    {
        // vdp2_scrn_display_unset(VDP2_SCRN_DISP_NBG0);
    }

    rgb1555_t black = RGB1555_INITIALIZER(1, 0, 0, 0);
    vdp2_scrn_back_color_set(VDP2_VRAM_ADDR(3, 0x01FFFE), black);
}

void gamelist_theme_destroy(browser_t *browser)
{
    ui_config_t *theme = theme_get_ui_config();

    // if (theme->screens.gamelist.cover.enabled)
    //     noise_destroy(&noise_cfg);
}
