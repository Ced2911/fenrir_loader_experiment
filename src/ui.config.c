#include <yaul.h>
#include "ui.config.h"

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
                        COLOR_RGB1555(1, 0x1F, 0x1F, 0x1F),
                        COLOR_RGB1555(1, 0x1F, 0x1F, 0x1F),
                        COLOR_RGB1555(1, 0x1F, 0x1F, 0x1F),
                        COLOR_RGB1555(1, 0x1F, 0x1F, 0x1F),
                    },
                },
                .item_focused_colors = {
                    .color = COLOR_RGB1555(1, 0x0F, 0x1F, 0x1F),
                    .gouraud = {
                        COLOR_RGB1555(1, 0x1F, 0x1F, 0x1F),
                        COLOR_RGB1555(1, 0x1F, 0x1F, 0x1F),
                        COLOR_RGB1555(1, 0x1F, 0x1F, 0x1F),
                        COLOR_RGB1555(1, 0x1F, 0x1F, 0x1F),
                    },
                },
                .position_bar = {
                    .color = COLOR_RGB1555(1, 0x0F, 0x1F, 0x1F),
                    .gouraud = {
                        COLOR_RGB1555(1, 0x1F, 0x1F, 0x1F),
                        COLOR_RGB1555(1, 0x1F, 0x1F, 0x1F),
                        COLOR_RGB1555(1, 0x1F, 0x1F, 0x1F),
                        COLOR_RGB1555(1, 0x1F, 0x1F, 0x1F),
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