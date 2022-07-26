#pragma once
#include <stdint.h>
#include <yaul.h>
#include "font/font.h"

struct browser_s;
typedef struct browser_s browser_t;

typedef void (*browser_input_callback_t)(browser_t *);
typedef void (*browser_get_item_callback_t)(browser_t *, uint32_t item, char *dst, int max_len);

typedef struct
{
    uint16_t font_color;
    uint16_t font_focus_color;

    uint16_t font_height;
    uint16_t line_height;
    uint16_t x_offset;
    uint16_t y_offset;
    uint16_t browser_w;

    struct
    {
        uint8_t visible;
        uint16_t x;
        uint16_t h;
        uint16_t w;
    } bar;

} browser_ui_config_t;

#define BROWSER_UI_DEFAULT     \
    {                          \
        .font_color = 0,       \
        .browser_w = 300,      \
        .font_focus_color = 1, \
        .line_height = 10,     \
        .font_height = 9,      \
        .x_offset = 20,        \
        .y_offset = 25,        \
        .bar = {               \
            .visible = 1,      \
            .x = 290,          \
            .h = 12,           \
            .w = 10,           \
        },                     \
    }

typedef struct browser_s
{
    // config
    uint32_t count;
    uint32_t file_per_page;
    // event
    browser_input_callback_t input_handler;
    browser_get_item_callback_t get_item;
    void *userdata;
    // lifecycle
    int selected;
    int page;
    int old_page;

    // ressources
    uintptr_t texture_buffer;
    uintptr_t texture_base;
    uintptr_t pal_base;
    uintptr_t gouraud_base;
    font_texture_t font_textures[32];

    // private
    smpc_peripheral_digital_t digital;

    browser_ui_config_t browser_ui_config;
} browser_t;

void browser_init(browser_t *);
void browser_update(browser_t *);
