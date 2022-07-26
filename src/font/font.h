#pragma once
#include <stdint.h>
#include <yaul.h>


typedef struct
{
    uint16_t h;
    uint16_t w;
    uintptr_t addr;
} font_texture_t;

uint8_t draw_font(vdp1_cmdt_t *cmdt, uint8_t letter, int x, int y, uint16_t color);
uint8_t draw_char_width(uint8_t letter);

size_t font_texture_font_create(font_texture_t *tex, char *text);
int font_get_text_len(char *format, ...);

// @deprecated
void font_to_vram();
