#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <yaul.h>
#include "browser.h"
#include "font/font.h"
#include "vdp1.config.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define BROWSER_OFFSET_Y 20
#define BROWSER_OFFSET_X 20
#define BROWSER_LINE_HEIGHT 16
#define BROWSER_FOCUSED_ITEM_COLOR 1
#define BROWSER_ITEM_COLOR 0

#define ELEMENT_COUNT(n) (sizeof((n)) / sizeof(*(n)))

#define MAX_INPUT 20

static vdp1_cmdt_t *cmdt = NULL;

static void draw_selector(browser_t *browser, int y)
{
    /*****************************************************
     * Draw browser selector position
     ****************************************************/
    const int16_t area_h = browser->browser_ui_config.line_height * browser->file_per_page;
    const int16_t area_y = browser->browser_ui_config.y_offset;
    const int16_t area_x = browser->browser_ui_config.bar.x;
    const int16_t area_w = browser->browser_ui_config.bar.w;
    const int16_t bar_w = browser->browser_ui_config.bar.w - 4;
    const int16_t bar_h = browser->browser_ui_config.bar.h;

    const vdp1_cmdt_draw_mode_t draw_mode = {
        .raw = 0x0000};

    const color_rgb1555_t color = COLOR_RGB1555(1, 0x0F, 0x1F, 0x1F);

    const int selected = browser->selected;
    const int count = browser->count;

    const int16_t position = (((selected << 16) / (count - 1)) * (area_h - bar_h - 4)) >> 16;
    const int16_t bar_x = area_x + 2;
    const int16_t bar_y = area_y + position + 2;

    cmdt->cmd_xa = bar_x + bar_w;
    cmdt->cmd_ya = bar_y;

    cmdt->cmd_xb = bar_x + bar_w;
    cmdt->cmd_yb = bar_y + bar_h;

    cmdt->cmd_xc = bar_x;
    cmdt->cmd_yc = bar_y + bar_h;

    cmdt->cmd_xd = bar_x;
    cmdt->cmd_yd = bar_y;

    vdp1_cmdt_polygon_set(cmdt);
    vdp1_cmdt_param_draw_mode_set(cmdt, draw_mode);
    vdp1_cmdt_param_color_set(cmdt, color);

    cmdt++;
}

static void browser_draw_items(browser_t *browser)
{
    char entry[40];
    const int menux = BROWSER_OFFSET_X;
    const int menuy = BROWSER_OFFSET_Y;

    const uint32_t start = MIN(browser->page * browser->file_per_page, browser->count);
    const uint32_t end = MIN(start + browser->file_per_page, browser->count);

    uintptr_t vdp_texture_base = browser->texture_base;
    uintptr_t texture_buffer = browser->texture_buffer;

    for (uint32_t i = start; i < end; i++)
    {
        const int color = (int)i == browser->selected ? BROWSER_FOCUSED_ITEM_COLOR : BROWSER_ITEM_COLOR;

        const uint32_t pal = (uint32_t)browser->pal_base;
        const uint32_t gouraud = (uint32_t)browser->gouraud_base;

        // get item text
        browser->get_item(browser, i, entry, 40);

        // create texture in ram buffer
        font_texture_t *tex = &browser->font_textures[i - start];
        tex->addr = texture_buffer;
        size_t texture_size = font_texture_font_create(tex, entry);

        // build and enqueue the polygone
        const vdp1_cmdt_draw_mode_t draw_mode = {
            .raw = 0x0000,
            .bits.cc_mode = 4,    // enable gouraud
            .bits.color_mode = 1, // 16color 4bit
            .bits.trans_pixel_disable = false,
            .bits.pre_clipping_disable = true,
            .bits.end_code_disable = true};

        const vdp1_cmdt_color_bank_t color_bank = {
            .type_0.data.dc = 0};

        cmdt->cmd_xa = menux;
        cmdt->cmd_ya = menuy + ((i - start) * BROWSER_LINE_HEIGHT);

        vdp1_cmdt_normal_sprite_set(cmdt);
        vdp1_cmdt_param_color_mode1_set(cmdt, pal);
        vdp1_cmdt_param_draw_mode_set(cmdt, draw_mode);
        vdp1_cmdt_param_size_set(cmdt, tex->w, tex->h);

        vdp1_cmdt_param_char_base_set(cmdt, vdp_texture_base);
        vdp1_cmdt_param_gouraud_base_set(cmdt, gouraud);

        vdp_texture_base += texture_size;
        texture_buffer += texture_size;

        cmdt++;
    }

    // copy buffer now
    vdp_dma_enqueue((void *)browser->texture_base, (void *)browser->texture_buffer, texture_buffer - browser->texture_buffer);
}

static int num_entry(browser_t *browser)
{
    int nummenu = MIN(browser->file_per_page, browser->count);
    if (((browser->page + 1) * browser->file_per_page) > browser->count)
    {
        nummenu = browser->count % browser->file_per_page;
    }
    return nummenu;
}

void browser_update(browser_t *browser)
{

    const int max_page = (int)(browser->count - 1) / browser->file_per_page;
    const uint32_t nummenu = num_entry(browser);

    /*****************************************************
     * Update input
     ****************************************************/
    smpc_peripheral_process();
    smpc_peripheral_digital_port(1, &browser->digital);

    const int page = browser->page;

    /*****************************************************
     * Process input
     ****************************************************/
    if (browser->digital.held.button.up)
    {
        if (browser->selected != 0)
        {
            browser->selected--;
            browser->page = browser->selected / browser->file_per_page;
        }
    }
    else if (browser->digital.held.button.down)
    {
        if (browser->selected < (int)(browser->count - 1))
        {
            browser->selected++;
            browser->page = browser->selected / browser->file_per_page;
        }
    }
    else if (browser->digital.held.button.left)
    {
        if (browser->page != 0)
        {
            browser->page--;
            browser->selected = MAX(0, (int)(browser->selected - browser->file_per_page));
        }
    }
    else if (browser->digital.held.button.right)
    {
        if (browser->page != max_page)
        {
            browser->page++;
            browser->selected = MIN(browser->count - 1, browser->selected + browser->file_per_page);
        }
    }
    else if (browser->digital.held.raw != 0)
    {
        // Send event to user defined callback
        browser->input_handler(browser);
    }

    /*****************************************************
     * Draw
     ****************************************************/
    cmdt = &cmdt_list->cmdts[ORDER_BUFFER_STARTING_INDEX];

    draw_selector(browser, browser->selected);

    // Only draw items if page changed
    if (browser->old_page != browser->page)
        browser_draw_items(browser);

    browser->old_page = browser->page;

    // finish drawing
    cmdt = &cmdt_list->cmdts[ORDER_BUFFER_STARTING_INDEX + 1 + browser->file_per_page];
    vdp1_cmdt_end_set(cmdt++);

    cmdt_list->count = cmdt - cmdt_list->cmdts;
    vdp1_sync_cmdt_list_put(cmdt_list, 0);

    vdp1_sync_render();
    vdp1_sync();
    vdp2_sync();
    vdp2_sync_wait();
}

void browser_init(browser_t *browser)
{
    /*****************************************************
     * Browser
     ****************************************************/
    browser->selected = 2;
    browser->page = 0;
    browser->old_page = -1;

    // Allocate a buffer for texture cache
    browser->texture_buffer = (uintptr_t)malloc(browser->file_per_page * (512 * 16));
}
