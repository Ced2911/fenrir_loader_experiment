#include <yaul.h>
#include "ui.config.h"
#include "../assets/bitmap.h"
#include "../assets/test.h"

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
    .screens = {
        .gamelist = {
            .background = {
                .velocity_x = FIX16(0.7),
                .velocity_y = FIX16(-0.7),
                .cell_addr = (uintptr_t)mvsc__cell,
                .cell_sz = mvsc__cell_sz,
                .pal_addr = (uintptr_t)mvsc__pal,
                .pal_sz = mvsc__pal_sz,
                .pattern_addr = (uintptr_t)mvsc__pattern,
                .pattern_sz = mvsc__pattern_sz
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
                .line_height = 16,
                /** */
                .item_colors = {
                    .color = COLOR_RGB1555(1, 0x0F, 0x0F, 0x0F),
                    .gouraud = {
                        COLOR_RGB1555(1, 0x00, 0x00, 0x1F),
                        COLOR_RGB1555(1, 0x00, 0x1F, 0x00),
                        COLOR_RGB1555(1, 0x1F, 0x00, 0x00),
                        COLOR_RGB1555(1, 0x1F, 0x1F, 0x00),
                    },
                },
                .item_focused_colors = {
                    .color = COLOR_RGB1555(1, 0x0F, 0x1F, 0x1F),
                    .gouraud = {
                        COLOR_RGB1555(1, 0x00, 0x1F, 0x00),
                        COLOR_RGB1555(1, 0x00, 0x00, 0x1F),
                        COLOR_RGB1555(1, 0x1F, 0x1F, 0x00),
                        COLOR_RGB1555(1, 0x1F, 0x00, 0x1F),
                    },
                },
                .position_bar = {
                    .color = COLOR_RGB1555(1, 0x0F, 0x1F, 0x1F),
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