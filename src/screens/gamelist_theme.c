#include <stdlib.h>
#include "browser.h"
#include "gamelist.h"
#include "screens.h"
#include "fenrir/fenrir.h"
#include "vdp1.config.h"
#include "vdp2.config.h"
#include "theme.h"
extern fenrir_config_t *fenrir_config;

static void cmdt_set_x_y_w_h(vdp1_cmdt_t *cmdt, int16_t x, int16_t y, int16_t w, int16_t h)
{
    cmdt->cmd_xa = x;
    cmdt->cmd_ya = y;

    cmdt->cmd_xc = x + w;
    cmdt->cmd_yc = y + h;

    cmdt->cmd_xb = x + w;
    cmdt->cmd_yb = y;

    cmdt->cmd_xd = x;
    cmdt->cmd_yd = y + h;

    vdp1_cmdt_char_size_set(cmdt, w, h);
}

void gamelist_theme_update(browser_t *browser)
{
    ui_config_t *const theme = ui_config;

    // draw sd/wifi bar..
    const int tex_w = 16;
    const int tex_h = 16;

    vdp1_cmdt_t *cmdt = &cmdt_list->cmdts[ORDER_BUFFER_DEV_ICON];
    // sd
    if (fenrir_config->hdr.sd_size > 0)
        vdp1_cmdt_char_base_set(cmdt++, DEVICE_ICON_OFFSET(DEVICE_ICON_STATE_SD));
    else
        vdp1_cmdt_char_base_set(cmdt++, DEVICE_ICON_OFFSET(DEVICE_ICON_STATE_NO_SD));

    // wifi
    if (fenrir_config->hdr.smb_status == 1)
        vdp1_cmdt_char_base_set(cmdt++, DEVICE_ICON_OFFSET(DEVICE_ICON_STATE_WIFI));
    else
        vdp1_cmdt_char_base_set(cmdt++, DEVICE_ICON_OFFSET(DEVICE_ICON_STATE_NO_WIFI));
}

static void draw_status_icons()
{
    ui_config_t *const theme = ui_config;
    vdp1_cmdt_t *cmdt = &cmdt_list->cmdts[ORDER_BUFFER_DEV_ICON];
    // build and enqueue the polygon
    const vdp1_cmdt_draw_mode_t draw_mode = {
        .raw = 0x0000,
        .cc_mode = 0,
        .color_mode = VDP1_CMDT_CM_RGB_32768,
        .trans_pixel_disable = false,
        .pre_clipping_disable = true,
        .end_code_disable = true};

    const int tex_w = 16;
    const int tex_h = 16;

    const vdp1_cmdt_color_bank_t color_bank = {
        .type_0.dc = 0};

    // draw sd
    cmdt_set_x_y_w_h(cmdt, theme->screens.gamelist.browser.device_icon.x, theme->screens.gamelist.browser.device_icon.y, tex_w, tex_h);
    vdp1_cmdt_normal_sprite_set(cmdt);
    vdp1_cmdt_color_mode1_set(cmdt, 0);
    vdp1_cmdt_gouraud_base_set(cmdt, 0);
    vdp1_cmdt_draw_mode_set(cmdt, draw_mode);
    vdp1_cmdt_char_size_set(cmdt, tex_w, tex_h);

    vdp1_cmdt_char_base_set(cmdt, ICONS_TEXTURE_ADDR);

    // draw wifi
    cmdt++;
    cmdt_set_x_y_w_h(cmdt, theme->screens.gamelist.browser.device_icon.x + tex_w, theme->screens.gamelist.browser.device_icon.y, tex_w, tex_h);
    vdp1_cmdt_normal_sprite_set(cmdt);
    vdp1_cmdt_color_mode1_set(cmdt, 0);
    vdp1_cmdt_gouraud_base_set(cmdt, 0);
    vdp1_cmdt_draw_mode_set(cmdt, draw_mode);
    vdp1_cmdt_char_size_set(cmdt, tex_w, tex_h);

    vdp1_cmdt_char_base_set(cmdt, ICONS_TEXTURE_ADDR + 2 * 16 * 16);

    cmdt++;
}

void gamelist_theme_apply(browser_t *browser)
{
    ui_config_t *const theme = ui_config;
    /*****************************************************
     * Apply theme configuration
     ****************************************************/
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
            .cc_mode = 0,
            .color_mode = VDP1_CMDT_CM_RGB_32768,
            .trans_pixel_disable = false,
            .pre_clipping_disable = true,
            .end_code_disable = true};

        const int tex_w = 128;
        const int tex_h = 96;

        const vdp1_cmdt_color_bank_t color_bank = {
            .type_0.dc = 0};

        cmdt->cmd_xa = theme->screens.gamelist.cover.x;
        cmdt->cmd_ya = theme->screens.gamelist.cover.y;

        cmdt->cmd_xb = theme->screens.gamelist.cover.x + theme->screens.gamelist.cover.w;
        cmdt->cmd_yb = theme->screens.gamelist.cover.y;

        cmdt->cmd_xc = theme->screens.gamelist.cover.x + theme->screens.gamelist.cover.w;
        cmdt->cmd_yc = theme->screens.gamelist.cover.y + theme->screens.gamelist.cover.h;

        cmdt->cmd_xd = theme->screens.gamelist.cover.x;
        cmdt->cmd_yd = theme->screens.gamelist.cover.y + theme->screens.gamelist.cover.h;

        vdp1_cmdt_distorted_sprite_set(cmdt);
        vdp1_cmdt_color_mode1_set(cmdt, 0);
        vdp1_cmdt_gouraud_base_set(cmdt, 0);
        vdp1_cmdt_draw_mode_set(cmdt, draw_mode);
        vdp1_cmdt_char_size_set(cmdt, tex_w, tex_h);

        vdp1_cmdt_char_base_set(cmdt, COVER_TEXTURE_ADDR);

        // noise_init(&noise_cfg);
    }

    /*****************************************************
     * upload icons
     ****************************************************/
    {
        uint32_t icon_sz = 0;
        theme_vdp1_sprites_t *icons = (theme_vdp1_sprites_t *)theme_get_ressource(theme_bin, THEME_ID_ICONS, &icon_sz);
        if (icons && icon_sz)
        {
            memcpy(ICONS_TEXTURE_ADDR, icons->data, icon_sz - sizeof(theme_vdp1_sprites_t));
        }
    }

    // draw sd/wifi bar..
    draw_status_icons();

    /*****************************************************
     * ...
     ****************************************************/

    rgb1555_t black = RGB1555_INITIALIZER(1, 0, 0, 0);
    vdp2_scrn_back_color_set(VDP2_VRAM_ADDR(3, 0x01FFFE), black);

    browser->item_w = theme->screens.gamelist.browser.w;
    browser->file_per_page = theme->screens.gamelist.browser.h / theme->screens.gamelist.browser.line_height;

    vdp2_upload_cells();
}

void gamelist_theme_destroy(browser_t *browser)
{
}
