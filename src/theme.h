#pragma once
#include "ui.config.h"

#define THEME_UI (theme_get_ui_config())

ui_config_t *theme_get_ui_config();
void theme_init_vdp();
void theme_init_bgm();
