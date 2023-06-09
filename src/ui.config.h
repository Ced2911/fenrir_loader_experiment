#pragma once

#define NOISE_PALETTE_SIZE (16)

#define PACKED_STRUCT struct __attribute__((__packed__))

typedef enum
{
    THEME_SCR_NBG0 = 0,
    THEME_SCR_NBG2 = 2
} theme_scr_t;


// autoconv
typedef PACKED_STRUCT {
	int char_width;
	int char_height;
} ui_config_font_t;

typedef PACKED_STRUCT {
	uint32_t pattern_offset;
	fix16_t velocity_x;
	fix16_t velocity_y;
	uint8_t screen;
} ui_config_background_t;

typedef PACKED_STRUCT {
	uint16_t w;
	uint16_t h;
	rgb1555_t color[2];
	rgb1555_t gouraud[4];
	uint8_t enabled;
	uint8_t padding;
} ui_position_bar_t;

typedef PACKED_STRUCT {
	rgb1555_t colors[2];
	rgb1555_t gouraud[4];
} ui_item_color_t;

typedef PACKED_STRUCT {
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
	uint16_t line_height;
	ui_position_bar_t position_bar;
	ui_item_color_t item_colors;
	ui_item_color_t item_focused_colors;
} ui_config_browser_t;

typedef PACKED_STRUCT {
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
	rgb1555_t noise_palettes[16];
	uint8_t enabled;
	uint8_t padding;
} ui_config_screen_game_list_cover_t;

typedef PACKED_STRUCT {
	ui_config_screen_game_list_cover_t cover;
	ui_config_browser_t browser;
} ui_config_screen_game_list_t;

typedef PACKED_STRUCT {
	int a;
} ui_config_error_t;

typedef PACKED_STRUCT {
	ui_config_screen_game_list_t gamelist;
	ui_config_error_t error_no_sd;
	ui_config_error_t error_bad_filesystem;
} ui_config_screens_t;


typedef PACKED_STRUCT {
	uint32_t version;
	uint32_t size;
	ui_config_screens_t screens;
	ui_config_font_t main_font;
} ui_config_t;

extern  ui_config_t const * ui_config;
