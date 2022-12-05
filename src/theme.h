#pragma once
#include "ui.config.h"
#include "screens/screens.h"

#define THEME_UI (theme_get_ui_config())

// local
void theme_update();

// shared
ui_config_t *theme_get_ui_config();
void theme_init_vdp();
void theme_init_bgm();
void theme_set_background(screens_type_t scr);
