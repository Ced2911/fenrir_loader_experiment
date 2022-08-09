#pragma once
#include <stdint.h>
#include <yaul.h>

typedef struct
{
    uint16_t h;
    uint16_t w;
    uintptr_t addr;
} font_texture_t;

size_t font_texture_font_create(font_texture_t *tex, char *text);
int font_get_text_len(char *format, ...);
