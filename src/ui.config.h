#pragma once

#define NOISE_PALETTE_SIZE (16)

typedef struct
{
    int char_width;
    int char_height;
    uintptr_t data;
    uintptr_t char_spacing;
} ui_config_font_t;

typedef struct
{
    uintptr_t pattern_addr;
    uintptr_t cell_addr;
    uintptr_t pal_addr;
    
    size_t pattern_sz;
    size_t cell_sz;
    size_t pal_sz;

    fix16_t velocity_x;
    fix16_t velocity_y;
} ui_config_background_t;

typedef struct
{
    struct
    {
        uint16_t w;
        uint16_t h;
        color_rgb1555_t color;
        color_rgb1555_t gouraud[4];
    } position_bar;

    struct
    {
        color_rgb1555_t color;
        color_rgb1555_t gouraud[4];
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
                color_rgb1555_t noise_palettes[NOISE_PALETTE_SIZE];
            } cover;

            ui_config_background_t background;
            ui_config_browser_t browser;

        } gamelist;
    } screens;

    ui_config_font_t main_font;
} ui_config_t;

extern ui_config_t ui_config;