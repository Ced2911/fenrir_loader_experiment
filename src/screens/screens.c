#include "screens.h"
#include "gamelist.h"
#include "error.h"
#include "options.h"
#include "diagnostic.h"
#include "bram_backup.h"
#include "credits.h"

void theme_update();
void theme_set_background(screens_type_t scr);

static screens_type_t next_screen;
static screens_type_t current_screen;

static screen_t *screens[] = {
    &gamelist_screen,
    &error_no_sd_screen,
    &error_bad_filesystem_screen,
    &options_screen,
    &diagnostics_screen,
    &bram_backup_screen,
    &bram_restore_screen,
    &credits_screen};

static screen_t *get(screens_type_t scr)
{
    for (int i = 0; i < screen_max; i++)
    {
        if (screens[i]->type == scr)
            return screens[i];
    }

    // fallback
    return &gamelist_screen;
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
