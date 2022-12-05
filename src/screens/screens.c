#include "screens.h"
#include "gamelist.h"
#include "error.h"
#include "options.h"
#include "diagnostic.h"
#include "bram_backup.h"

void theme_update();
void theme_set_background(screens_type_t scr);

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
    case screen_options:
        return &options_screen;
        break;
    case screen_diagnostic:
        return &diagnostics_screen;
    case screen_backup_bram:
        return &bram_backup;
    case screen_restore_bram:
        return &bram_backup;
    case screen_gamelist:
    default:
        return &gamelist_screen;
        break;
    }
}

screen_t *get_screen()
{
    return get(current_screen);
}

void screens_init()
{
    // next_screen = screen_error_no_sd;
    next_screen = screen_gamelist;
    current_screen = screen_max;
}

void screens_select(screens_type_t scr)
{
    next_screen = scr;
}

void screens_update()
{
    if (next_screen != current_screen)
    {
        get(current_screen)->destroy();
        get(next_screen)->init();
        current_screen = next_screen;

        // theme_set_background(current_screen);
    }
    screen_t *s = get_screen();
    // theme_update();
    s->update();
}

void screens_destroy()
{
    screen_t *s = get_screen();
    s->destroy();
}