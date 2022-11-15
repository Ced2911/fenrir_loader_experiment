#pragma once
typedef void (*screen_init_t)();
typedef void (*screen_destroy_t)();
typedef void (*screen_update_t)();

typedef struct
{
    screen_init_t init;
    screen_destroy_t destroy;
    screen_update_t update;
} screen_t;

typedef enum screens
{
    screen_gamelist,
    screen_error_no_sd,
    screen_error_bad_filesystem,
    screen_max
} screens_type_t;

void screens_init();
void screens_select(screens_type_t scr);
void screens_update();
void screens_destroy();
