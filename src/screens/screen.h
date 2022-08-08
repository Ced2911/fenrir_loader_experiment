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
