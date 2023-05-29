#pragma once
#include <stdint.h>
#include <yaul.h>
#include "font/font.h"
#include "ui.config.h"


#define BROWSER_FOCUSED_ITEM_COLOR (16)
#define BROWSER_ITEM_COLOR (0)

struct browser_s;
typedef struct browser_s browser_t;

typedef void (*browser_input_callback_t)(browser_t *);
typedef void (*browser_get_item_callback_t)(browser_t *, uint32_t item, char *dst, int max_len);

typedef struct browser_s
{
    // config
    uint32_t count;
    uint32_t file_per_page;
    uint32_t item_w;
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

    ui_config_browser_t browser_ui_config;
} browser_t;

void browser_init(browser_t *);
void browser_update(browser_t *);
void browser_destroy(browser_t *);