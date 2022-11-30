#include <stdint.h>
#include <yaul.h>
#include <assert.h>
#include "font.h"
#include "theme.h"
//#include "./themes/demo/font.sfa.h"

#define align_8(addr) (((addr) + 7) & (~7))

// #define THEME_FONT (&theme_get_ui_config()->main_font)
typedef struct
{
    int char_width;
    int char_height;
    uint8_t *data;
    uint8_t *char_spacing;
} __font_t;
static const __font_t sfa = {.char_width = 8, .char_height = 9, .data = NULL, .char_spacing = NULL};
#define THEME_FONT (&sfa)

static inline void memcpy4bpp(uint8_t *d, uint8_t *s, uint32_t nb, int off)
{
    // src is always aligned to 8b
    if (off == 0)
    {
        for (uint32_t xoff = 0; xoff < nb; xoff++)
        {
            *d++ = *s++;
        }
    }
    else
    {
        uint8_t s0 = d[0] & 0xF;
        for (uint32_t xoff = 0; xoff < nb; xoff++)
        {
            uint8_t s1 = *s++;
            *d++ = s0 << 4 | s1 >> 4;

            s0 = s1;
        }
    }
}

static uint8_t font_get_letter_width(uint8_t letter)
{
    return THEME_FONT->char_spacing[letter];
}

static uint8_t __draw_font_10(uint8_t letter, int x, int y, uint8_t *dst, uint32_t pitch)
{
    const uint8_t letter_w = font_get_letter_width(letter);
    const uint32_t h = THEME_FONT->char_height;
    const uint32_t w = THEME_FONT->char_width;

    uint8_t *font = (uint8_t *)(THEME_FONT->data);
    uint8_t *src = &font[(letter - 32) * ((w * h / 2))];

    for (uint32_t yoff = y; yoff < ((y + h)); yoff++)
    {
        memcpy4bpp(&dst[yoff * pitch + x / 2], src, letter_w / 2, x & 1);
        src += w / 2;
    }

    return letter_w;
}

size_t font_texture_font_create(font_texture_t *tex, char *text)
{
    int w_str = font_get_text_len(text);
    uint32_t pitch = align_8(w_str + 1) / 2;
    int length = strlen(text);
    int x = 0;
    int y = 0;
    tex->h = THEME_FONT->char_height;
    tex->w = pitch * 2;

    // erase texture
    uint32_t texture_size = tex->h * pitch;
    for (uint32_t i = 0; i < texture_size; i++)
        ((uint8_t *)(tex->addr))[i] = 0;

    for (int i = 0; i < length; i++)
    {
        if (text[i] != ' ')
            x += __draw_font_10(text[i], x, y, (uint8_t *)tex->addr, pitch);
        else
            x += THEME_FONT->char_spacing[' '];
    }
    return tex->h * pitch;
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