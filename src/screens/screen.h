#pragma once
typedef void (*screen_init_t)();
typedef void (*screen_update_t)();

typedef struct
{
    screen_update_t update;
    screen_init_t init;
} screen_t;
