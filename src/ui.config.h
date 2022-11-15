#pragma once

#define NOISE_PALETTE_SIZE (16)

typedef enum {
    THEME_SCR_NBG0 = 0,
    THEME_SCR_NBG2 = 2
} theme_scr_t;

typedef struct
{
    int char_width;
    int char_height;
    uint8_t *data;
    uint8_t *char_spacing;
} ui_config_font_t;

typedef struct
{
    uint8_t screen;
    uintptr_t pattern_offset;
    fix16_t velocity_x;
    fix16_t velocity_y;
} ui_config_background_t;

typedef struct
{
    struct
    {
        uint16_t w;
        uint16_t h;
        rgb1555_t color;
        rgb1555_t gouraud[4];
        uint8_t enabled;
    } position_bar;

    struct
    {
        rgb1555_t colors[2];
        rgb1555_t gouraud[4];
    } item_colors, item_focused_colors;

    uint16_t x;
    uint16_t y;
    // used to create a clipping area
    uint16_t w;
    uint16_t h;
    uint16_t line_height;
} ui_config_browser_t;

typedef struct
{
    struct
    {
        struct
        {
            struct
            {
                uint16_t x;
                uint16_t y;
                uint16_t w;
                uint16_t h;
                rgb1555_t noise_palettes[NOISE_PALETTE_SIZE];
                uint8_t enabled;
            } cover;
            ui_config_browser_t browser;

            ui_config_background_t foreground;
            ui_config_background_t background;
        } gamelist;
        struct
        {
            ui_config_background_t foreground;
            ui_config_background_t background;
        } error_no_sd;
        struct
        {
            ui_config_background_t foreground;
            ui_config_background_t background;
        } error_bad_filesystem;
    } screens;

    ui_config_font_t main_font;
} ui_config_t;
