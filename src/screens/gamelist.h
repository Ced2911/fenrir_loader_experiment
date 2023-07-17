#pragma once
enum
{
    DEVICE_ICON_STATE_SD = 0,
    DEVICE_ICON_STATE_NO_SD,
    DEVICE_ICON_STATE_WIFI,
    DEVICE_ICON_STATE_NO_WIFI,
};

#define DEVICE_ICON_OFFSET(x) ((16*16*2*x)+ICONS_TEXTURE_ADDR)

#include "screens/screens.h"

extern screen_t gamelist_screen;