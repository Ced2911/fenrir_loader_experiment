#pragma once
#include <stdint.h>

struct ui_item_s;

enum
{
    UI_BTN_CANCEL,
    UI_BTN_OK,

};

enum
{
    UI_END,
    UI_BREAK,
    UI_LINE,
    UI_LABEL,
    UI_BOOL,
    UI_LIST
};

struct ui_item_s
{
    uint8_t type;
    uint8_t id;
    uint8_t enabled;

    union
    {
        struct
        {
            char *text;
        } label;

        struct {
            char ** content;
            uint8_t value;
            uint8_t count; 
        } list;

        struct
        {
            uint8_t value;
        } toggle;

        // todo
    };

    void (*handler)(struct ui_item_s *item);
};

typedef struct ui_item_s ui_item_t;

typedef struct 
{
    uint8_t *vram; uint16_t * cram;
} ui_item_init_t;


void ui_init(ui_item_init_t * p);
void ui_render(ui_item_t *diag);
void ui_update(ui_item_t *diag);
