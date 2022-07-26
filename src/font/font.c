#include <stdint.h>
#include <yaul.h>
#include <assert.h>
#include "font.h"

#define align_8(addr) (((addr) + 7) & (~7))

#define THEME_FONT_WIDTH 256
#define THEME_FONT_HEIGHT 256
#define THEME_FONT_COUNT 256

typedef struct
{
    uint32_t font_height;
    uint8_t font_width[THEME_FONT_COUNT];
    uint8_t font[THEME_FONT_WIDTH * THEME_FONT_HEIGHT / 2]; // 8bpp
} menu_font_t;

typedef struct
{
    void *tex_base;
    void *pal_base;
    void *gouraud_base;
} menu_font_vdp_t;

menu_font_vdp_t menu_font_vdp = {};

extern const char army12[33028];

#define THEME_FONT ((const menu_font_t *)(army12))

#define C_RGB(r, g, b) (((b)&0x1F) << 10 | ((g)&0x1F) << 5 | ((r)&0x1F) | 0x8000)

static const uint16_t color_[] = {
    C_RGB(0, 0, 0),
    C_RGB(0, 0, 0),
    C_RGB(0, 0, 0),
    C_RGB(0, 0, 0),
    //
    C_RGB(31, 1, 31),
    C_RGB(31, 1, 31),
    C_RGB(31, 1, 31),
    C_RGB(31, 1, 31),
    //
    C_RGB(2, 2, 2),
    C_RGB(3, 3, 3),
    C_RGB(4, 4, 4),
    C_RGB(5, 5, 5),
    C_RGB(6, 6, 6),
    C_RGB(7, 7, 7),
    C_RGB(8, 8, 8),
    C_RGB(9, 9, 9),
    C_RGB(10, 10, 10),
    C_RGB(11, 11, 11),
    C_RGB(12, 12, 12),
    C_RGB(13, 13, 13),
    C_RGB(14, 14, 14),
    C_RGB(15, 15, 15),
    C_RGB(16, 16, 16),
    C_RGB(17, 17, 17),
    C_RGB(18, 18, 18),
    C_RGB(19, 19, 19),
    C_RGB(20, 20, 20),
    C_RGB(21, 21, 21),
    C_RGB(22, 22, 22),
    C_RGB(23, 23, 23),
    C_RGB(24, 24, 24),
    C_RGB(25, 25, 25),
    C_RGB(26, 26, 26),
    C_RGB(27, 27, 27),
    C_RGB(28, 28, 28),
    C_RGB(29, 29, 29),
    C_RGB(30, 30, 30),
    C_RGB(31, 31, 31),
};

static const uint16_t palettes[] = {
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0xFFFF,
};

static void font_dma_upload(menu_font_vdp_t *menu_font_vdp, void *dst, void *src, size_t len)
{
    vdp_dma_enqueue(dst, src, len);
}

void font_to_vram()
{
    menu_font_t *font = (menu_font_t *)army12;
    vdp1_vram_partitions_t vdp1_vram_partitions;
    vdp1_vram_partitions_get(&vdp1_vram_partitions);

    menu_font_vdp.tex_base = vdp1_vram_partitions.texture_base;
    menu_font_vdp.gouraud_base = vdp1_vram_partitions.gouraud_base;
    menu_font_vdp.pal_base = vdp1_vram_partitions.clut_base;

    // font_dma_upload(&menu_font_vdp, menu_font_vdp.tex_base, font->font, (THEME_FONT_WIDTH * THEME_FONT_HEIGHT / 2));
    font_dma_upload(&menu_font_vdp, menu_font_vdp.gouraud_base, (void *)color_, sizeof(color_) * sizeof(uint16_t));
    font_dma_upload(&menu_font_vdp, menu_font_vdp.pal_base, (void *)palettes, sizeof(palettes) * sizeof(uint16_t));
}

uint8_t draw_font(vdp1_cmdt_t *cmdt, uint8_t letter, int x, int y, uint16_t color)
{
    const menu_font_t *font = (menu_font_t *)army12;
    if (letter != ' ')
    {
        const uint32_t texture = (uint32_t)((uint8_t *)(menu_font_vdp.tex_base)) + ((letter - 32) * 16 * 8);
        const uint32_t pal = (uint32_t)((uint8_t *)(menu_font_vdp.pal_base));
        const uint32_t gouraud = (uint32_t)((uint8_t *)(menu_font_vdp.gouraud_base)) + (color * 8);

        const vdp1_cmdt_draw_mode_t draw_mode = {
            .raw = 0x0000,
            .bits.cc_mode = 4,    // enable gouraud
            .bits.color_mode = 1, // 16color 4bit
            .bits.trans_pixel_disable = false,
            .bits.pre_clipping_disable = true,
            .bits.end_code_disable = true};

        const vdp1_cmdt_color_bank_t color_bank = {
            .type_0.data.dc = 0};

        cmdt->cmd_xa = x;
        cmdt->cmd_ya = y;

        vdp1_cmdt_normal_sprite_set(cmdt);
        vdp1_cmdt_param_color_mode1_set(cmdt, pal);
        vdp1_cmdt_param_draw_mode_set(cmdt, draw_mode);
        vdp1_cmdt_param_size_set(cmdt, 16, 16);

        vdp1_cmdt_param_char_base_set(cmdt, texture);
        vdp1_cmdt_param_gouraud_base_set(cmdt, gouraud);

        // vdp1_cmdt_param_color_set(cmdt, color);
        return 0;
    }

    return -1;
}

uint8_t draw_char_width(uint8_t letter)
{
    const menu_font_t *font = (menu_font_t *)army12;
    return font->font_width[letter];
}

//
// 4bpp font support
//
static inline void memcpy4bpp(uint8_t *d, uint8_t *s, uint32_t nb, int off)
{
    if (off == 0)
    {
        for (uint32_t xoff = 0; xoff < (nb / 2); xoff++)
        {
            *d++ = *s++;
        }
    }
    else
    {
        uint8_t s0 = d[0] & 0xF;
        for (uint32_t xoff = 0; xoff < (nb / 2); xoff++)
        {
            //*d++ = *s++;
            uint8_t s1 = *s++;
            *d++ = s0 << 4 | s1 >> 4;

            s0 = s1;
        }
    }
}

#define CHAR_W 16
#define CHAR_H 16

static uint8_t __draw_font_10(uint8_t letter, int x, int y, uint8_t *dst, uint32_t pitch, uint16_t color)
{
    extern uint8_t bg0_vram[];
    const uint32_t w = CHAR_W;
    const uint32_t h = CHAR_H;
    uint8_t *font = (uint8_t *)(THEME_FONT->font);
    uint8_t *src = &font[(letter - 32) * ((w * h / 2))];

    for (uint32_t yoff = y; yoff < ((y + h)); yoff++)
    {
        memcpy4bpp(&dst[yoff * pitch + x / 2], src, w, x & 1);
        src += w / 2;
    }

    return THEME_FONT->font_width[letter];
}

size_t font_texture_font_create(font_texture_t *tex, char *text)
{
    int w_str = font_get_text_len(text) + 8;
    int length = strlen(text);
    uint32_t pitch = align_8((w_str + 1) / 2);
    int x = 0;
    int y = 0;
    tex->h = CHAR_H;
    tex->w = pitch * 2;
    uint16_t color = 1;

    // erase texture
    uint32_t texture_size = tex->h * pitch;
    for (uint32_t i = 0; i < texture_size; i++)
        ((uint8_t *)(tex->addr))[i] = 0;

    for (int i = 0; i < length; i++)
    {
        if (text[i] != ' ')
            x += __draw_font_10(text[i], x, y, (uint8_t *)tex->addr, pitch, color);
        else
            x += THEME_FONT->font_width[' '];
    }
    return tex->h * pitch;
}

static uint8_t font_get_letter_width(uint8_t letter)
{
    return THEME_FONT->font_width[letter];
}

int font_get_text_len(char *format, ...)
{
    char string[256]; // I hope that's enough
    va_list arg;

    va_start(arg, format);
    vsprintf(string, format, arg);

    int x = 0;
    int length = strlen(string);

    for (int i = 0; i < length; i++)
    {
        x += font_get_letter_width(string[i]);
    }

    va_end(arg);
    return x;
}