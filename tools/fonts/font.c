#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#define THEME_FONT_WIDTH 256
#define THEME_FONT_HEIGHT 256
#define THEME_FONT_COUNT 256

#define WIDTH_DATA_OFFSET 20 // Offset to width data with BFF file
#define MAP_DATA_OFFSET 276  // Offset to texture image data with BFF file

typedef struct
{
    uint32_t font_height;
    uint8_t font_width[THEME_FONT_COUNT];
    uint8_t font[THEME_FONT_WIDTH * THEME_FONT_HEIGHT / 2]; // 8bpp
} menu_font_t;

typedef struct __attribute__((__packed__))
{
    uint8_t ID1;
    uint8_t ID2;
    uint32_t ImageWidth;
    uint32_t ImageHeight;
    uint32_t CellWidth;
    uint32_t CellHeight;
    uint8_t BPP;
    uint8_t StartPoint;
} FontFileHeader;

long filesize(FILE *fp)
{
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return sz;
}

void convert_font_8bpp(uint8_t *_src, uint8_t *_dst, uint8_t p, uint32_t w, uint32_t h, uint8_t grid_size_w, uint8_t grid_size_h)
{
    uint8_t *dst = _dst;
    int count = (w * h) / (grid_size_w * grid_size_h);

    for (int letter = 0; letter < count; letter++)
    {
        uint32_t x_offset = (letter * grid_size_w) % w;
        uint32_t y_offset = ((letter * grid_size_w) / w) * grid_size_h;
        for (uint32_t y = 0; y < grid_size_h; y++)
        {
            uint8_t *src = &_src[x_offset + ((y_offset + y) * w)];
            for (uint32_t x = 0; x < grid_size_w; x++)
            {
                uint32_t pixel = *src++;
                if (pixel)
                    *dst = p;
                dst++;
            }
        }
    }
}

void conv_8bpp_4bpp(uint8_t *src, uint8_t *dst, uint32_t len)
{
    uint8_t *px_src = src;
    uint8_t *px_dst = dst;
    for (int i = 0; i < len; i += 8)
    {
        for (int q = 0; q < 4; q++)
        {
            uint8_t pixel = 0;
            for (int p = 0; p < 2; p++)
            {
                uint8_t c = *px_src++;
                if (c)
                {
                    pixel = (pixel << 4) | (c & 7);
                }
                else
                {
                    pixel = (pixel << 4) | 0;
                }
            }
            *px_dst++ = pixel;
        }
    }
}

void conv_8bpp_1bpp(uint8_t *src, uint8_t *dst, uint32_t len)
{
    uint8_t *px_src = src;
    uint8_t *px_dst = dst;
    for (int i = 0; i < len; i += 8)
    {
        uint8_t pixel = 0;
        for (int p = 0; p < 8; p++)
        {
            if (*px_src++)
            {
                pixel = (pixel << 1) | 1;
            }
            else
            {
                pixel = (pixel << 1) | 0;
            }
        }
        *px_dst++ = pixel;
    }
}

int convert_font(const char *filename, menu_font_t *menu_font, FontFileHeader *ffh)
{
    FILE *fd = fopen(filename, "rb");
    if (fd == NULL)
    {
        printf("Error opening %s", filename);
        return -1;
    }

    size_t size = filesize(fd);
    uint8_t *file_buff = (uint8_t *)malloc(512 * 1024 * 4);
    uint8_t *buff = (uint8_t *)malloc(512 * 1024 * 4);
    uint8_t *buff1bpp = (uint8_t *)malloc(512 * 1024 * 4);

    fread(file_buff, size, 1, fd);

    FontFileHeader *font = (FontFileHeader *)file_buff;
    uint8_t *char_table = file_buff + WIDTH_DATA_OFFSET;
    uint8_t *pixel_buff = file_buff + MAP_DATA_OFFSET;

    assert(font->BPP == 8);

    memcpy(ffh, font, sizeof(FontFileHeader));

    // shadow
    // convert_font_8bpp(pixel_buff, buff + (1 + (font->CellWidth)), 2, font->ImageWidth, font->ImageHeight, font->CellWidth, font->CellHeight);

    // normal
    convert_font_8bpp(pixel_buff, buff, 1, font->ImageWidth, font->ImageHeight, font->CellWidth, font->CellHeight);
    conv_8bpp_4bpp(buff, buff1bpp, font->ImageWidth * font->ImageHeight);

    memcpy(menu_font->font_width, char_table, 256);
    memcpy(menu_font->font, buff1bpp, (THEME_FONT_WIDTH * THEME_FONT_HEIGHT) / 2);
    menu_font->font_height = font->CellHeight;

    free(file_buff);
    free(buff);
    free(buff1bpp);
    return 0;
}

void bin2c(FILE *f_output, const char *name, menu_font_t *buf, FontFileHeader *ffh)
{
    fprintf(f_output, "const uint8_t %s_font_width[] = {", name);
    for (int i = 0, need_comma = 0; i < THEME_FONT_COUNT; ++i)
    {
        if (need_comma)
            fprintf(f_output, ", ");
        else
            need_comma = 1;
        if ((i % 11) == 0)
            fprintf(f_output, "\n\t");

        fprintf(f_output, "0x%.2x", buf->font_width[i] & 0xff);
    }
    fprintf(f_output, "\n};\n\n");

    fprintf(f_output, "const uint8_t %s_font_bitmap[] = {", name);
    for (int i = 0, need_comma = 0; i < (ffh->CellWidth * ffh->CellHeight * THEME_FONT_COUNT) / 2; ++i)
    {
        if (need_comma)
            fprintf(f_output, ", ");
        else
            need_comma = 1;
        if ((i % 11) == 0)
            fprintf(f_output, "\n\t");

        fprintf(f_output, "0x%.2x", buf->font[i] & 0xff);
    }
    fprintf(f_output, "\n};\n\n");

    fprintf(f_output, "#ifndef FONT_STRUCT\n");

    fprintf(f_output, "typedef struct {int char_width;int char_height;const uint8_t *data;const uint8_t *char_spacing;} ui_font_t;\n");
    fprintf(f_output, "#define FONT_STRUCT\n");
    fprintf(f_output, "#endif\n");

    fprintf(f_output, "const ui_font_t %s = {.char_width=%d, .char_height=%d, .data=%s_font_bitmap, .char_spacing=%s_font_width};",
            name, ffh->CellWidth, ffh->CellHeight, name, name);
}

/**
 * ./a.out ../../assets/themes_sega_army_12.bff ../../assets/army12.c army12
 * **/
int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        fprintf(stderr, "Usage: %s binary_file output_file array_name\n", argv[0]);
        return -1;
    }

    FILE *fout = fopen(argv[2], "wb");
    FontFileHeader ffh;
    menu_font_t menu_font;
    convert_font(argv[1], &menu_font, &ffh);
    bin2c(fout, argv[3], &menu_font, &ffh);

    fclose(fout);
}