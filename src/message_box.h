#pragma once

typedef enum
{
    message_box_info,
    message_box_error
} message_box_type_t;

typedef struct
{
    message_box_type_t type;
    char *title;
    char *message;
} message_box_t;

// error
void error_set_message(const char *str);
void error_set_title(const char *str);
void error_wait_for_btn();

// info
void info_set_message(const char *str);
void info_set_title(const char *str);
void info_wait_for_btn();

void message_box(message_box_t *);