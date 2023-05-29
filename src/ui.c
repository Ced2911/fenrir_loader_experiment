#include <yaul.h>
#include <string.h>
#include "ui.h"
#include "ui_font.h"

// @todo move to somewhere else
// uint8_t *ui_shadow = (uint8_t *)malloc(512 * 256);
__attribute__((section(".noload"))) static uint8_t ui_shadow[512 * 256];

#define align_8(addr) (((addr) + 7) & (~7))

#define THEME_FONT (&small_fonts_12)

// to avoid complete redraw,
// each interactive item will use a new pallete number
// 0 is reserved for bg
// 1 is reserved for unfocused color
// 2 is reserved for highlight color
static const rgb1555_t bg_color = RGB888_RGB1555_INITIALIZER(1, 15, 0, 0);
static const rgb1555_t default_color = RGB888_RGB1555_INITIALIZER(1, 0x1f, 0x1f, 0x1F);
static const rgb1555_t highlight_color = RGB888_RGB1555_INITIALIZER(1, 0xfF, 0, 0x7f);
static const rgb1555_t active_color = RGB888_RGB1555_INITIALIZER(1, 0, 0xfF, 0xff);

#define LINE_HEIGHT (10)
#define SCREEN_W (352)
#define SCREEN_H (240)

#define V_ADDR(X, Y) (&ui_ctx.shadow[X + ((Y)*512)])

static struct ui_ctx
{
    uint8_t *shadow;
    uint8_t *vram;
    uint16_t *cram;

    int padding_top;
    int padding_left;

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

typedef struct
{
    uint8_t *dst;
    char *text;
    int x;
    int y;
    int pitch;
    int pal;
} ui_render_text_param_t;

static int ui_render_text(ui_render_text_param_t *param)
{
    int len = strlen(param->text);
    int text_size = 0;
    uint8_t *font = (uint8_t *)(THEME_FONT->data);
    const uint32_t h = THEME_FONT->char_height;
    const uint32_t w = THEME_FONT->char_width;

    for (int i = 0; i < len; i++)
    {
        const uint8_t letter_w = THEME_FONT->char_spacing[param->text[i]];
        uint8_t *src = &font[(param->text[i] - 32) * ((w * h / 2))];

        // do font
        for (int y0 = 0; y0 < h; y0++)
        {
            for (int x0 = 0; x0 < w; x0 += 2)
            {
                uint8_t *dst = &param->dst[(y0 + param->y) * param->pitch + param->x + x0 + text_size];
                dst[0] = ((*src) >> 4) ? ui_ctx.pal_nb : COLOR_BACKGROUND;
                dst[1] = ((*src) & 0xf) ? ui_ctx.pal_nb : COLOR_BACKGROUND;
                *src++;
            }
        }

        text_size += letter_w;
    }

    return text_size;
}

static int ui_item_is_ctrl(ui_item_t *item)
{
    if (item->handler)
        return 1;

    for (const uint8_t *type = ui_ctrl_type; *type; type++)
    {
        if (*type == item->type)
        {
            return 1;
        }
    }
    return 0;
}

static int ui_item_is_inline(ui_item_t *item)
{
    for (const uint8_t *type = ui_inline_type; *type; type++)
    {
        if (*type == item->type)
        {
            return 1;
        }
    }
    return 0;
}

static int ui_render_label(ui_item_t *item)
{
    ui_render_text_param_t param = {.dst = ui_ctx.shadow, .pitch = 512, .text = item->label.text, .x = ui_ctx.cur_item_x, .y = ui_ctx.cur_item_y};

    return ui_render_text(&param);
}

/** mock **/
static int ui_render_bool(ui_item_t *item)
{
    int len = 8;

    for (int y = 0; y < LINE_HEIGHT; y++)
    {
        for (int x = 0; x < LINE_HEIGHT; x++)
        {
            *V_ADDR(ui_ctx.cur_item_x + x, y + ui_ctx.cur_item_y) = ui_ctx.pal_nb;
        }
    }

    return 12;
}

static void ui_render_line(ui_item_t *item)
{
    ui_ctx.cur_item_x = ui_ctx.padding_left;
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
    // memcpy(vram, shadow, 512 * 256);
#ifdef FENRIR_480i
    vdp_dma_enqueue(vram, shadow, 1024 * 240);

#else
    vdp_dma_enqueue(vram, shadow, 512 * 240);
#endif
}

static int ui_get_next_item_in_row(ui_item_t *diag, int cur_item)
{
    if (cur_item < 0)
    {
        cur_item = 0;
    }
    int n = cur_item + 1;
    ui_item_t *item = &diag[n];

    while (item->type != UI_END && ui_item_is_inline(item))
    {
        if (ui_item_is_ctrl(item))
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

    while (n >= 0 && ui_item_is_inline(item))
    {
        if (ui_item_is_ctrl(item))
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
        if (line_break && ui_item_is_ctrl(item))
        {
            return next_item;
        }
        if (!ui_item_is_inline(item))
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
        if (line_break && ui_item_is_ctrl(item))
        {
            return next_item;
        }
        if (!ui_item_is_inline(item))
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
            if (item->number.value)
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
    uint16_t def = ui_ctx.cram[COLOR_REF];
    // palettes...
    for (int i = 10; i < 250; i++)
    {
        ui_ctx.cram[i] = def;
    }
}

void ui_update(ui_item_t *diag, void (*input_handler)(smpc_peripheral_digital_t *, void *), void *user_data)
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
    else if (digital.held.button.a)
    {
        if (ui_ctx.cur_item >= 0 && diag[ui_ctx.cur_item].handler)
        {
            diag[ui_ctx.cur_item].handler(&diag[ui_ctx.cur_item]);
        }
    }

    if (input_handler)
    {
        input_handler(&digital, user_data);
    }

    if (digital.held.raw)
    {
        ui_reset_colors();
        ui_update_values(diag);
        ui_update_focused(diag);
    }
}

static void _vblank_out_handler(void *work __unused)
{
    smpc_peripheral_intback_issue();
}

void ui_set_color(int pal_nb, rgb1555_t color)
{
    if (pal_nb < 0)
    {
        uint16_t def = color.raw;
        ui_ctx.cram[COLOR_REF] = def;
        for (int i = 10; i < 250; i++)
        {
            ui_ctx.cram[i] = def;
        }
    }
    else
    {
        ui_ctx.cram[pal_nb] = color.raw;
        if (pal_nb == COLOR_BACKGROUND)
        {
            vdp2_scrn_back_color_set(VDP2_VRAM_ADDR(3, 0x01FFFE), color);
        }
    }
}

void ui_render(ui_item_t *diag)
{
    int n = 0;
    // palettes...
    ui_ctx.cram[COLOR_BACKGROUND] = bg_color.raw;
    ui_ctx.cram[COLOR_HIGHLIGHT] = highlight_color.raw;
    ui_ctx.cram[COLOR_ACTIVE] = active_color.raw;
    ui_ctx.cram[COLOR_REF] = default_color.raw;

    ui_reset_colors();

    // erase screen
    for (int i = 0; i < (512 * 256); i++)
    {
        ui_shadow[i] = COLOR_BACKGROUND;
    }

    ui_item_t *item = diag;

    int pal_nb = 10;

    ui_ctx.cur_item_x = ui_ctx.padding_left;
    ui_ctx.cur_item_y = ui_ctx.padding_top;

    ui_ctx.cur_item = -1;

    while (item->type != UI_END)
    {
        int w = 0;
        ui_ctx.pal_nb = item->pal ? item->pal : pal_nb;

        switch (item->type)
        {
        case UI_LABEL:
            w = ui_render_label(item);
            break;
        case UI_BREAK:
            ui_ctx.cur_item_x = ui_ctx.padding_left;
            ui_ctx.cur_item_y += 10;
            break;
        case UI_BOOL:
            w = ui_render_bool(item);
            break;
        case UI_LINE:
            ui_render_line(item);
            break;
        default:
            break;
        }

        if (item->w == 0 && w)
        {
            ui_ctx.cur_item_x += w;
        }
        if (item->w > 0)
        {
            ui_ctx.cur_item_x += item->w;
        }

        // select first item
        if ((ui_item_is_ctrl(item) && ui_ctx.cur_item == -1))
        {
            ui_ctx.cur_item = n;
        }
        n++;
        pal_nb++;
        item++;
    }

    ui_reset_colors();
    ui_update_values(diag);
    ui_update_focused(diag);
    ui_blit(ui_shadow, ui_ctx.vram);

    vdp_sync_vblank_out_set(_vblank_out_handler, NULL);

    vdp2_scrn_back_color_set(VDP2_VRAM_ADDR(3, 0x01FFFE), bg_color);

    vdp2_scrn_display_set(VDP2_SCRN_DISPTP_NBG0 /*|VDP2_SCRN_DISPTP_NBG1|VDP2_SCRN_DISPTP_NBG2*/);

#if 1 && defined(FENRIR_480i)
    // setup draw window
    vdp2_ioregs_t *const vdp2_regs = vdp2_regs_get();
    vdp2_regs->wpsx0 = 0;
    vdp2_regs->wpsy0 = 0;
    vdp2_regs->wpex0 = 512;
    vdp2_regs->wpey0 = 256;
    vdp2_regs->wctla = 3;
#endif
}

void ui_init(ui_item_init_t *param)
{
    ui_ctx.padding_top = 24;
    ui_ctx.padding_left = 16;

    ui_ctx.cram = param->cram;
    ui_ctx.vram = param->vram;

    ui_ctx.shadow = ui_shadow;
    ui_ctx.cram = ui_ctx.cram;
}
