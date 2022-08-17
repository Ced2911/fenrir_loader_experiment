#include <yaul.h>
#include "ui.config.h"
#include "../assets/bitmap.h"
#include "../assets/mvsc_bg.h"
#include "../assets//marvel_fenrir.h"
#include "../assets/sfa.c"

#define THEME_FONT_WIDTH 256
#define THEME_FONT_HEIGHT 256
#define THEME_FONT_COUNT 256

typedef struct
{
    uint32_t font_height;
    uint8_t font_width[THEME_FONT_COUNT];
    uint8_t font[THEME_FONT_WIDTH * THEME_FONT_HEIGHT / 2]; // 8bpp
} menu_font_t;

#define NOISE_PALETTES                                    \
    {                                                     \
        COLOR_RGB1555(1, 0x00, 0xC0 >> 3, 0x40 >> 3),     \
            COLOR_RGB1555(1, 0x00, 0xF0 >> 3, 0x00 >> 3), \
            COLOR_RGB1555(1, 0x00, 0x80 >> 3, 0x60 >> 3), \
            COLOR_RGB1555(1, 0x00, 0xF0 >> 3, 0xF0 >> 3), \
                                                          \
            COLOR_RGB1555(1, 0x00, 0xC0 >> 3, 0x40 >> 3), \
            COLOR_RGB1555(1, 0x00, 0xF0 >> 3, 0x00 >> 3), \
            COLOR_RGB1555(1, 0x00, 0x80 >> 3, 0x60 >> 3), \
            COLOR_RGB1555(1, 0x00, 0xF0 >> 3, 0xF0 >> 3), \
                                                          \
            COLOR_RGB1555(1, 0x00, 0xC0 >> 3, 0x40 >> 3), \
            COLOR_RGB1555(1, 0x00, 0xF0 >> 3, 0x00 >> 3), \
            COLOR_RGB1555(1, 0x00, 0x80 >> 3, 0x60 >> 3), \
            COLOR_RGB1555(1, 0x00, 0xF0 >> 3, 0xF0 >> 3), \
                                                          \
            COLOR_RGB1555(1, 0x00, 0xC0 >> 3, 0x40 >> 3), \
            COLOR_RGB1555(1, 0x00, 0xF0 >> 3, 0x00 >> 3), \
            COLOR_RGB1555(1, 0x00, 0x80 >> 3, 0x60 >> 3), \
            COLOR_RGB1555(1, 0x00, 0xF0 >> 3, 0xF0 >> 3), \
    }

ui_config_t ui_config = {
    .main_font = {
        .char_height = sfa.char_height,
        .char_width = sfa.char_width,
        .char_spacing = sfa.char_spacing,
        .data = sfa.data,
    },
    .screens = {
        .gamelist = {
            .background = {
                .velocity_x = FIX16(0.7),
                .velocity_y = FIX16(-0.7),
                .cell_addr = (uintptr_t)mvsc_bg_cell,
                .cell_sz = mvsc_bg_cell_sz,
                .pal_addr = (uintptr_t)mvsc_bg_pal,
                .pal_sz = mvsc_bg_pal_sz,
                .pattern_addr = (uintptr_t)mvsc_bg_pattern,
                .pattern_sz = mvsc_bg_pattern_sz,
            },

            .fg = {
                .velocity_x = 0,
                .velocity_y = 0,
                .cell_addr = (uintptr_t)marvel_fenrir_cell,
                .cell_sz = marvel_fenrir_cell_sz,
                .pal_addr = (uintptr_t)marvel_fenrir_pal,
                .pal_sz = marvel_fenrir_pal_sz,
                .pattern_addr = (uintptr_t)marvel_fenrir_pattern,
                .pattern_sz = mvsc_bg_pattern_sz,
            },
            .cover = {
                .noise_palettes = NOISE_PALETTES,
                .x = 80,
                .y = 120,
                .w = 128,
                .h = 96,
            },
            .browser = {
                /** */
                .x = 20,
                .y = 24,
                .w = 200,
                .h = 200,
                .line_height = 12,
                /** */
                .item_colors = {
                    .colors = {
                        COLOR_RGB1555(1, 0x11, 0x11, 0x11),
                        COLOR_RGB1555(1, 0x05, 0x05, 0x05),
                    },
                    .gouraud = {
                        COLOR_RGB1555(1, 0x12, 0x12, 0x12),
                        COLOR_RGB1555(1, 0x12, 0x12, 0x12),
                        COLOR_RGB1555(1, 0x08, 0x08, 0x08),
                        COLOR_RGB1555(1, 0x08, 0x08, 0x08),
                    },
                },
                .item_focused_colors = {
                    .colors = {
                        COLOR_RGB1555(1, 0x11, 0x11, 0x11),
                        COLOR_RGB1555(1, 0x0, 0x0, 0x0),
                    },
                    .gouraud = {
                        COLOR_RGB1888_RGB1555_INITIALIZER(1, 0xdd, 0x33, 0x11),
                        COLOR_RGB1888_RGB1555_INITIALIZER(1, 0xdd, 0x33, 0x11),
                        COLOR_RGB1888_RGB1555_INITIALIZER(1, 0xff, 0xcc, 0x11),
                        COLOR_RGB1888_RGB1555_INITIALIZER(1, 0xff, 0xcc, 0x11),
                    },
                },
                .position_bar = {
                    .color = COLOR_RGB1555(1, 0x00, 0x11, 0x11),
                    .gouraud = {
                        COLOR_RGB1555(1, 0x00, 0x1F, 0x1F),
                        COLOR_RGB1555(1, 0x1F, 0x00, 0x1F),
                        COLOR_RGB1555(1, 0x1F, 0x00, 0x1F),
                        COLOR_RGB1555(1, 0x00, 0x1F, 0x1F),
                    },
                    .w = 5,
                    .h = 12,
                }
                /** end browser **/
            }

            /** end gamelist **/
        }

        /** end screens **/
    }
    /** end config **/
};