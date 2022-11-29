#include <yaul.h>
#include <string.h>
#include "ui.h"

static vdp2_scrn_bitmap_format_t rbg0 = {
    .bitmap_size = VDP2_SCRN_BITMAP_SIZE_512X256,
    .bitmap_base = VDP2_VRAM_ADDR(0, 0x00000),
    .palette_base = VDP2_CRAM_ADDR(0),
    .ccc = VDP2_SCRN_CCC_PALETTE_256,
    .scroll_screen = VDP2_SCRN_NBG0};

// to avoid complete redraw,
// each interactive item will use a new pallete number
// 0 is reserved for bg
// 1 is reserved for unfocused color
// 2 is reserved for highlight color
static const rgb1555_t bg_color = RGB888_RGB1555_INITIALIZER(1, 15, 0, 0);
static const rgb1555_t default_color = RGB888_RGB1555_INITIALIZER(1, 0x1f, 0x1f, 0x1F);
static const rgb1555_t highlight_color = RGB888_RGB1555_INITIALIZER(1, 0xfF, 0, 0x7f);
static const rgb1555_t active_color = RGB888_RGB1555_INITIALIZER(1, 0, 0xfF, 0xff);

#define COLOR_BACKGROUND (0)
#define COLOR_DEFAULT (1)
#define COLOR_HIGHLIGHT (2)
#define COLOR_ACTIVE (3)

#define LINE_HEIGHT (10)
#define SCREEN_W (352)
#define SCREEN_H (240)

#define V_ADDR(X, Y) (&ui_ctx.shadow[X + ((Y)*512)])

static struct ui_ctx
{
    uint8_t *shadow;
    uint8_t *vram;
    uint16_t *cram;
    int cur_item_x;
    int cur_item_y;
    int pal_nb;

    int cur_item;
} ui_ctx;

const uint8_t ui_inline_type[] = {
    UI_LABEL, UI_BOOL, UI_LIST, 0};

const uint8_t ui_block_type[] = {
    UI_BREAK, UI_LINE, 0};

const uint8_t ui_ctrl_type[] = {
    UI_BOOL, UI_LIST, 0};

static int ui_item_is_ctrl(uint8_t item_type)
{
    for (uint8_t *type = ui_ctrl_type; *type; type++)
    {
        if (*type == item_type)
        {
            return 1;
        }
    }
    return 0;
}

static int ui_item_is_inline(uint8_t item_type)
{
    for (uint8_t *type = ui_inline_type; *type; type++)
    {
        if (*type == item_type)
        {
            return 1;
        }
    }
    return 0;
}

/** mock **/
static void ui_render_label(ui_item_t *item)
{
    // mock...
    int len = strlen(item->label.text);

    for (int y = 0; y < LINE_HEIGHT; y++)
    {
        for (int x = 0; x < (len * 8); x++)
        {
            *V_ADDR(ui_ctx.cur_item_x + x, y + ui_ctx.cur_item_y) = COLOR_DEFAULT;
        }
    }

    ui_ctx.cur_item_x += len * 8;
}

/** mock **/
static void ui_render_switch(ui_item_t *item)
{
    int len = 8;

    for (int y = 0; y < LINE_HEIGHT; y++)
    {
        for (int x = 0; x < LINE_HEIGHT; x++)
        {
            *V_ADDR(ui_ctx.cur_item_x + x, y + ui_ctx.cur_item_y) = ui_ctx.pal_nb;
        }
    }

    ui_ctx.cur_item_x += 12;
}

static void ui_render_line(ui_item_t *item)
{
    ui_ctx.cur_item_x = 0;
    ui_ctx.cur_item_y += LINE_HEIGHT + LINE_HEIGHT / 2;

    for (int x = 0; x < SCREEN_W; x++)
    {
        *V_ADDR(x, ui_ctx.cur_item_y) = COLOR_HIGHLIGHT;
    }

    ui_ctx.cur_item_y += 1 + LINE_HEIGHT / 2;
}

static void ui_blit(uint8_t *shadow, uint8_t *vram)
{
    // scu_dma_transfer(0, (void *)shadow, vram, 512 * 256);
    memcpy(vram, shadow, 512 * 256);
}

void ui_init()
{
    vdp2_scrn_bitmap_format_set(&rbg0);
    vdp2_scrn_priority_set(VDP2_SCRN_NBG0, 1);
    vdp2_scrn_display_set(VDP2_SCRN_DISPTP_NBG0);

    vdp2_cram_offset_set(VDP2_SCRN_NBG0, 0);
}

static int ui_get_next_item_in_row(ui_item_t *diag, int cur_item)
{
    if (cur_item < 0)
    {
        cur_item = 0;
    }
    int n = cur_item + 1;
    ui_item_t *item = &diag[n];

    while (item->type != UI_END && ui_item_is_inline(item->type))
    {
        if (ui_item_is_ctrl(item->type))
        {
            return n;
        }
        item++;
        n++;
    }

    return cur_item;
}

static int ui_get_prev_item_in_row(ui_item_t *diag, int cur_item)
{
    if (cur_item < 0)
    {
        cur_item = 0;
    }
    int n = cur_item - 1;
    ui_item_t *item = &diag[n];

    while (n >= 0 && ui_item_is_inline(item->type))
    {
        if (ui_item_is_ctrl(item->type))
        {
            return n;
        }
        item--;
        n--;
    }

    return cur_item;
}

static int ui_get_bottom_item(ui_item_t *diag, int cur_item)
{
    if (cur_item < 0)
    {
        cur_item = 0;
    }
    // find next row
    int next_item = cur_item + 1;
    ui_item_t *item = &diag[next_item];

    int line_break = 0;

    //
    while (item->type != UI_END)
    {
        if (line_break && ui_item_is_ctrl(item->type))
        {
            return next_item;
        }
        if (!ui_item_is_inline(item->type))
        {
            line_break = 1;
        }
        next_item++;
        item++;
    }

    // no item
    return cur_item;
}

static int ui_get_top_item(ui_item_t *diag, int cur_item)
{
    if (cur_item < 0)
    {
        cur_item = 0;
    }
    // find next row
    int next_item = cur_item - 1;
    ui_item_t *item = &diag[next_item];

    int line_break = 0;

    //
    while (next_item >= 0)
    {
        if (line_break && ui_item_is_ctrl(item->type))
        {
            return next_item;
        }
        if (!ui_item_is_inline(item->type))
        {
            line_break = 1;
        }
        next_item--;
        item--;
    }

    // no item
    return cur_item;
}

static void ui_update_values(ui_item_t *diag)
{
    ui_item_t *item = diag;
    int pal = 10;

    // highlight
    while (item->type != UI_END)
    {
        switch (item->type)
        {
        case UI_BOOL:
            if (item->toggle.value)
                ui_ctx.cram[pal] = highlight_color.raw;

            break;
        default:
            break;
        }
        pal++;
        item++;
    }
}

static void ui_update_focused(ui_item_t *diag)
{
    ui_item_t *item = diag;
    int item_n = 0;
    int pal = 10;

    // highlight
    while (item->type != UI_END)
    {
        if (item_n == ui_ctx.cur_item)
        {
            ui_ctx.cram[pal] = active_color.raw;
        }

        pal++;
        item++;
        item_n++;
    }
}

static void ui_reset_colors()
{
    // palettes...
    for (int i = 10; i < 255; i++)
    {
        ui_ctx.cram[i] = default_color.raw;
    }
}

void ui_update(ui_item_t *diag)
{
    smpc_peripheral_digital_t digital;

    smpc_peripheral_process();
    smpc_peripheral_digital_port(1, &digital);

    //
    if (digital.held.button.left)
    {
        ui_ctx.cur_item = ui_get_prev_item_in_row(diag, ui_ctx.cur_item);
    }
    else if (digital.held.button.right)
    {
        ui_ctx.cur_item = ui_get_next_item_in_row(diag, ui_ctx.cur_item);
    }
    else if (digital.held.button.up)
    {
        ui_ctx.cur_item = ui_get_top_item(diag, ui_ctx.cur_item);
    }
    else if (digital.held.button.down)
    {
        ui_ctx.cur_item = ui_get_bottom_item(diag, ui_ctx.cur_item);
    }

    ui_reset_colors();
    ui_update_values(diag);
    ui_update_focused(diag);

    dbgio_printf("[H[2J");
    dbgio_printf("s: %d\n", ui_ctx.cur_item);
    dbgio_flush();
}

void ui_render(ui_item_t *diag, uint8_t *shadow, uint8_t *vram, uint16_t *cram)
{
    int n = 0;
    cram[COLOR_BACKGROUND] = bg_color.raw;
    cram[COLOR_DEFAULT] = default_color.raw;
    cram[COLOR_HIGHLIGHT] = highlight_color.raw;
    cram[COLOR_ACTIVE] = active_color.raw;

    // palettes...
    for (int i = 10; i < 255; i++)
    {
        cram[i] = default_color.raw;
    }

    // erase screen
    for (int i = 0; i < (512 * 256); i++)
    {
        shadow[i] = COLOR_BACKGROUND;
    }

    ui_item_t *item = diag;

    ui_ctx.pal_nb = 10;

    ui_ctx.cur_item_x = 0;
    ui_ctx.cur_item_y = 0;

    ui_ctx.shadow = shadow;
    ui_ctx.cram = cram;

    ui_ctx.cur_item = -1;

    while (item->type != UI_END)
    {
        switch (item->type)
        {
        case UI_LABEL:
            ui_render_label(item);
            break;
        case UI_BREAK:
            ui_ctx.cur_item_x = 0;
            break;
        case UI_BOOL:
            ui_render_switch(item);
            break;
        case UI_LINE:
            ui_render_line(item);
            break;
        default:
            break;
        }

        // select first item
        if ((ui_item_is_ctrl(item->type) && ui_ctx.cur_item == -1))
        {
            ui_ctx.cur_item = n;
        }
        n++;
        ui_ctx.pal_nb++;
        item++;
    }

    ui_blit(shadow, vram);
    ui_update(diag);
}