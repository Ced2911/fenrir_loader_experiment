#pragma once
#include "ui.config.h"
#include "screens/screens.h"

#define THEME_UI (theme_get_ui_config())

// local
void theme_update();

// shared
ui_config_t *theme_get_ui_config();
void theme_init_vdp();
void theme_init_bgm();
void theme_set_background(screens_type_t scr);

//
typedef struct __attribute__((__packed__))
{
    rgb1555_t color;
    rgb1555_t shadow;
    rgb1555_t gradients[4];
} theme_cfg_item_color_t;

typedef struct __attribute__((__packed__))
{
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
} theme_cfg_area_t;

typedef struct __attribute__((__packed__))
{
    theme_cfg_area_t area;
    uint16_t line_height;
    theme_cfg_item_color_t item;
    theme_cfg_item_color_t focused;
} theme_cfg_browser_t;

typedef struct __attribute__((__packed__))
{
    theme_cfg_area_t area;
    uint16_t enabled;
} theme_cfg_cover_t;

typedef struct __attribute__((__packed__))
{
    uint32_t x;
    uint32_t y;
} theme_cfg_background_t;

typedef struct __attribute__((__packed__))
{
    theme_cfg_browser_t gamelist_browser;
    theme_cfg_cover_t gamelist_cover;
    theme_cfg_background_t gamelist_background;
} theme_cfg_t;

//

/*****************************************************/
#define THEME_ID_CONFIG_V0 0x000000001
#define THEME_ID_VDP2_BG 0x000000010
#define THEME_ID_VDP2_FG 0x000000011
#define THEME_ID_VGM_SN76xxx 0x000000100

/*****************************************************/
typedef struct
{
    uint32_t version;
    uint32_t size;
    uint32_t count;
} theme_bin_t;

typedef struct
{
    uint32_t id;
    uint32_t offset;
    uint32_t size;
    uint32_t _pad;
} ressource_t;

static uint8_t *theme_get_ressource(uint8_t *thm, uint32_t id, uint32_t *size)
{
    theme_bin_t *theme_bin = (theme_bin_t *)thm;
    ressource_t *rsc = (ressource_t *)(thm + 0x80);
    for (int i = 0; i < theme_bin->count; i++)
    {
        if (rsc->id == id)
        {
            *size = rsc->size;
            return thm + rsc->offset;
        }
        rsc++;
    }

    return NULL;
}
/*****************************************************/
