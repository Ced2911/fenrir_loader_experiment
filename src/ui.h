#pragma once
#include <stdint.h>

#define _UI_BREAK        \
    {                    \
        .type = UI_BREAK \
    }

#define _UI_LINE        \
    {                   \
        .type = UI_LINE \
    }

#define _UI_END        \
    {                  \
        .type = UI_END \
    }

#define _UI_LABEL(STR)                            \
    {                                             \
        .type = UI_LABEL, .label = {.text = STR}, \
    }

#define _UI_LABEL_ID(STR, ID)                              \
    {                                                      \
        .type = UI_LABEL, .label = {.text = STR}, .id = ID \
    }

#define _UI_LABEL_W(STR, WIDTH)                              \
    {                                                        \
        .type = UI_LABEL, .label = {.text = STR}, .w = WIDTH \
    }

#define _UI_BOOL(ID, VAL, HANDLER)                                               \
    {                                                                            \
        .id = ID, .type = UI_BOOL, .number = {.value = VAL}, .handler = HANDLER, \
    }

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

    // 0: auto, >0 fixed
    int w;

    union
    {
        struct
        {
            char *text;
        } label;

        struct
        {
            char **content;
            uint8_t value;
            uint8_t count;
        } list;

        struct
        {
            int value;
        } number;

        // todo
    };

    void (*handler)(struct ui_item_s *item);
};

typedef struct ui_item_s ui_item_t;

typedef struct
{
    uint8_t *vram;
    uint16_t *cram;
    int padding_top;
    int padding_left;
} ui_item_init_t;

void ui_init(ui_item_init_t *p);
void ui_render(ui_item_t *diag);
void ui_update(ui_item_t *diag);

static inline ui_item_t *ui_get_item_by_id(ui_item_t *diag, int id)
{
    ui_item_t *item = diag;
    while (item->type != UI_END)
    {
        if (item->id == id)
        {
            return item;
        }
        item++;
    }
    return NULL;
}
