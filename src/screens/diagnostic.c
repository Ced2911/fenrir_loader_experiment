#include "ui.h"

static void test_sw_hdl(ui_item_t *i)
{
    i->toggle.value = !i->toggle.value;
}

ui_item_t error_box[] = {
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
    {.type = UI_LABEL,
     .label = {.text = "test b"}},
    {
        .type = UI_BOOL,
        .toggle = {.value = 0},
        .handler = test_sw_hdl,
    },
    {.type = UI_BOOL,
     .toggle = {.value = 1},
     .handler = test_sw_hdl},
    {.type = UI_END}};
