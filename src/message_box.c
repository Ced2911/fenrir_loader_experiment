#include <yaul.h>
#include "message_box.h"
#include "ui.h"

#define GET_LABEL_BY_ID(ID) ui_get_item_by_id(dialog, ID)->label.text

enum
{
    DIA_TITLE,
    DIA_MESSAGE
};

static const rgb1555_t error_bg_color = RGB888_RGB1555_INITIALIZER(1, 162, 62, 72);
static const rgb1555_t error_message_color = RGB888_RGB1555_INITIALIZER(1, 108, 142, 173);
static const rgb1555_t error_title_color = RGB888_RGB1555_INITIALIZER(1, 255, 60, 56);

static const rgb1555_t info_bg_color = RGB888_RGB1555_INITIALIZER(1, 9, 21, 64);
static const rgb1555_t info_message_color = RGB888_RGB1555_INITIALIZER(1, 72, 202, 228);
static const rgb1555_t info_title_color = RGB888_RGB1555_INITIALIZER(1, 0, 150, 199);

static char dia_title[50];
static char dia_message[50];

static ui_item_t dialog[] = {
    _UI_TITLE_ID(dia_title, DIA_TITLE),
    _UI_LINE,
    _UI_LABEL_ID(dia_message, DIA_MESSAGE),
    _UI_END};

static void set_message(const char *str)
{
    strcpy(GET_LABEL_BY_ID(DIA_MESSAGE), str);
}

static void set_title(const char *str)
{
    strcpy(GET_LABEL_BY_ID(DIA_TITLE), str);
}

static void wait_for_btn()
{
    smpc_peripheral_digital_t digital = {.held.raw = 0};

    while (!digital.held.button.a)
    {
        smpc_peripheral_process();
        smpc_peripheral_digital_port(1, &digital);

        vdp1_sync_render();
        vdp1_sync();
        vdp2_sync();
        vdp1_sync_wait();
        vdp2_sync_wait();
    }
}

void message_box(message_box_t *box)
{
    strcpy(GET_LABEL_BY_ID(DIA_TITLE), box->title);
    strcpy(GET_LABEL_BY_ID(DIA_MESSAGE), box->message);

    ui_render(dialog);

    switch (box->type)
    {
    case message_box_error:
        ui_set_color(COLOR_BACKGROUND, error_bg_color);
        ui_set_color(COLOR_HIGHLIGHT, error_title_color);
        ui_set_color(COLOR_DEFAULT, error_message_color);
        break;
    case message_box_info:
        ui_set_color(COLOR_BACKGROUND, info_bg_color);
        ui_set_color(COLOR_HIGHLIGHT, info_title_color);
        ui_set_color(COLOR_DEFAULT, info_message_color);
        break;
    }

    ui_update(dialog);
    wait_for_btn();
}

// error
void error_set_message(const char *str) { set_message(str); }
void error_set_title(const char *str) { set_title(str); }
void error_wait_for_btn() { wait_for_btn(); }

// info
void info_set_message(const char *str) { set_message(str); }
void info_set_title(const char *str) { set_title(str); }
void info_wait_for_btn() { wait_for_btn(); }
