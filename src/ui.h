#pragma once
#include <stdint.h>

#define COLOR_BACKGROUND (0)
#define COLOR_HIGHLIGHT (1)
#define COLOR_ACTIVE (2)
#define COLOR_REF (3)
#define COLOR_DEFAULT (-1)

#define COLOR_ITEM(X) (10 + X)

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

#define _UI_LABEL_NULL_ID(ID) _UI_LABEL_ID(NULL, ID)

#define _UI_LABEL(STR)                            \
    {                                             \
        .type = UI_LABEL, .label = {.text = STR}, \
    }

#define _UI_TITLE(STR)                                                    \
    {                                                                     \
        .type = UI_LABEL, .pal = COLOR_HIGHLIGHT, .label = {.text = STR}, \
    }

#define _UI_LABEL_ID(STR, ID)                              \
    {                                                      \
        .type = UI_LABEL, .label = {.text = STR}, .id = ID \
    }

#define _UI_TITLE_ID(STR, ID)                                                      \
    {                                                                              \
        .type = UI_LABEL, .pal = COLOR_HIGHLIGHT, .label = {.text = STR}, .id = ID \
    }

#define _UI_LABEL_W(STR, WIDTH)                              \
    {                                                        \
        .type = UI_LABEL, .label = {.text = STR}, .w = WIDTH \
    }

#define _UI_LABEL_ID_W(ID, WIDTH)              \
    {                                          \
        .type = UI_LABEL, .id = ID, .w = WIDTH \
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
    int16_t type;

    // string id, 0 => allocated, others: need memory management (malloc/free)
    int16_t id;

    // 0: auto, >0 fixed
    int16_t w;

    uint8_t disabled;

    // 0: auto
    uint8_t pal;
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
void ui_clear();
void ui_render(ui_item_t *diag);
void ui_update(ui_item_t *diag, void (*input_handler)(smpc_peripheral_digital_t *, void *), void *user_data);
void ui_set_color(int pal_nb, rgb1555_t color);

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
