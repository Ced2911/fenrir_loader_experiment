#include "screen.h"
#include "gamelist.h"
#include "error.h"

static screens_type_t next_screen;
static screens_type_t current_screen;

static screen_t *get(screens_type_t scr)
{
    switch (scr)
    {
    case screen_error_no_sd:
        return &error_no_sd;
        break;
    case screen_error_bad_filesystem:
        return &error_bad_filesystem;
        break;
    case screen_gamelist:
    default:
        return &gamelist_screen;
        break;
    }
}

void screen_init()
{
    next_screen = screen_gamelist;
    current_screen = screen_max;
}

void screen_select(screens_type_t scr)
{
    next_screen = scr;
}


screen_t *get_screen()
{
    if (next_screen != current_screen)
    {
        get(current_screen)->destroy();
        get(next_screen)->init();
        current_screen = next_screen;
    }
    return get(current_screen);
}
