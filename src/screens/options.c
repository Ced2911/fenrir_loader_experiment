#include <yaul.h>
#include "options.h"
#include "screen.h"
#include "../ui.h"

static void test_sw_hdl(ui_item_t *i)
{
    i->toggle.value = !i->toggle.value;
}

static ui_item_t options_items[] = {
    {
        .type = UI_LABEL,
        .label = {.text = "test"},
    },
    {
        .type = UI_LINE,
    },
    {
        .type = UI_LABEL,
        .label = {.text = "test a"},
    },
    {
        .type = UI_BOOL,
        .toggle = {.value = 0},
        .handler = test_sw_hdl,
    },
    {
        .type = UI_LABEL,
        .label = {.text = "test b"},
    },
    {
        .type = UI_LINE,
    },
    {
        .type = UI_LABEL,
        .label = {.text = "test b"},
    },
    {
        .type = UI_BOOL,
        .toggle = {.value = 0},
        .handler = test_sw_hdl,
    },
    {
        .type = UI_BOOL,
        .toggle = {.value = 1},
        .handler = test_sw_hdl,
    },
    {.type = UI_END}};

static void options_init()
{
    ui_render(options_items);
}

static void options_update()
{
    ui_update(options_items);
}

static void options_destroy()
{
}

screen_t options_screen = {
    .init = options_init,
    .update = options_update,
    .destroy = options_destroy,
};

// @ todo
screen_t diagnostics_screen = {
    .init = options_init,
    .update = options_update,
    .destroy = options_destroy,
};
