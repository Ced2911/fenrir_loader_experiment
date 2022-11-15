#define NOISE_PALETTES                              \
    {                                               \
        RGB1555(1, 0x00, 0xC0 >> 3, 0x40 >> 3),     \
            RGB1555(1, 0x00, 0xF0 >> 3, 0x00 >> 3), \
            RGB1555(1, 0x00, 0x80 >> 3, 0x60 >> 3), \
            RGB1555(1, 0x00, 0xF0 >> 3, 0xF0 >> 3), \
                                                    \
            RGB1555(1, 0x00, 0xC0 >> 3, 0x40 >> 3), \
            RGB1555(1, 0x00, 0xF0 >> 3, 0x00 >> 3), \
            RGB1555(1, 0x00, 0x80 >> 3, 0x60 >> 3), \
            RGB1555(1, 0x00, 0xF0 >> 3, 0xF0 >> 3), \
                                                    \
            RGB1555(1, 0x00, 0xC0 >> 3, 0x40 >> 3), \
            RGB1555(1, 0x00, 0xF0 >> 3, 0x00 >> 3), \
            RGB1555(1, 0x00, 0x80 >> 3, 0x60 >> 3), \
            RGB1555(1, 0x00, 0xF0 >> 3, 0xF0 >> 3), \
                                                    \
            RGB1555(1, 0x00, 0xC0 >> 3, 0x40 >> 3), \
            RGB1555(1, 0x00, 0xF0 >> 3, 0x00 >> 3), \
            RGB1555(1, 0x00, 0x80 >> 3, 0x60 >> 3), \
            RGB1555(1, 0x00, 0xF0 >> 3, 0xF0 >> 3), \
    }

static ui_config_t ui_config = {
    .main_font = {
        .char_height = sfa.char_height,
        .char_width = sfa.char_width,
        .char_spacing = sfa.char_spacing,
        .data = sfa.data,
    },
    .screens = {
        .error_no_sd = {
            .background = {
                .pattern_offset = NBG0_PATTERN_ADDR, .screen = THEME_SCR_NBG0,
                .velocity_x = 0, // FIX16(-0.7),
                .velocity_y = 0, // FIX16(-0.7),
            },

            .foreground = {
                .pattern_offset = NBG2_PATTERN_ADDR,
                .screen = THEME_SCR_NBG2,
                .velocity_x = FIX16(-3),
                .velocity_y = 0,
            },
        },
        .error_bad_filesystem = {
            .background = {
                .pattern_offset = NBG0_PATTERN_ADDR, .screen = THEME_SCR_NBG0,
                .velocity_x = 0, // FIX16(-0.7),
                .velocity_y = 0, // FIX16(-0.7),
            },

            .foreground = {
                .pattern_offset = NBG2_PATTERN_ADDR + pattern_offset_error_front,
                .screen = THEME_SCR_NBG2,
                .velocity_x = 0,
                .velocity_y = 0,
            },
        },
        .gamelist = {
            .background = {
                .pattern_offset = NBG0_PATTERN_ADDR, .screen = THEME_SCR_NBG0,
                .velocity_x = 0, // FIX16(-0.7),
                .velocity_y = 0, // FIX16(-0.7),
            },

            .foreground = {
                .pattern_offset = NBG2_PATTERN_ADDR,
                .screen = THEME_SCR_NBG2,
                .velocity_x = FIX16(-3),
                .velocity_y = 0,
            },

            // use vdp2 so coord need to be aligned to 8
            .cover = {
                .noise_palettes = NOISE_PALETTES,
                .x = 8,
                .y = 56,
                .w = 128,
                .h = 96,
                .enabled = 1,
            },
            .browser = {
                /** */
                .x = 148,
                .y = 56,
                .w = 100,
                .h = 200,
                .line_height = 12,
                /** */
                .item_colors = {
                    .colors = {
                        RGB1555(1, 0x11, 0x11, 0x11),
                        RGB1555(1, 0x05, 0x05, 0x05),
                    },
                    .gouraud = {
                        RGB1555(1, 0x12, 0x12, 0x12),
                        RGB1555(1, 0x12, 0x12, 0x12),
                        RGB1555(1, 0x08, 0x08, 0x08),
                        RGB1555(1, 0x08, 0x08, 0x08),
                    },
                },
                .item_focused_colors = {
                    .colors = {
                        RGB1555(1, 0x11, 0x11, 0x11),
                        RGB1555(1, 0x0, 0x0, 0x0),
                    },
                    .gouraud = {
                        RGB888_RGB1555_INITIALIZER(1, 0xdd, 0x33, 0x11),
                        RGB888_RGB1555_INITIALIZER(1, 0xdd, 0x33, 0x11),
                        RGB888_RGB1555_INITIALIZER(1, 0xff, 0xcc, 0x11),
                        RGB888_RGB1555_INITIALIZER(1, 0xff, 0xcc, 0x11),
                    },
                },
                .position_bar = {
                    .enabled = 0,
                    .color = RGB1555(1, 0x00, 0x11, 0x11),
                    .gouraud = {
                        RGB1555(1, 0x00, 0x1F, 0x1F),
                        RGB1555(1, 0x1F, 0x00, 0x1F),
                        RGB1555(1, 0x1F, 0x00, 0x1F),
                        RGB1555(1, 0x00, 0x1F, 0x1F),
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