#include <yaul.h>
#include "options.h"
#include "screen.h"
#include "../ui.h"

#define SCREEN_W (352)

enum
{
    UI_OPTIONS_IGR = 1,
    UI_SYS_BIOS_ID,
    UI_SYS_SMPC_REGION,
    UI_SYS_VIDEO_FREQ,
    UI_SYS_REGION_FLAGS,
    UI_SYS_HW_FW_REV,
    UI_SYS_HW_REV,
    UI_SYS_HW_UID,
    UI_SYS_HW_FW_DATE,
    UI_SYS_HW_WIFI_STATE,
    UI_SYS_HW_IP_ADDR,
    UI_SYS_HW_SD_NAME,
    UI_SYS_HW_SD_TYPE,
    UI_SYS_HW_SD_SIZE,
};

static void enable_igr(ui_item_t *i)
{
    i->number.value = !i->number.value;
}

static ui_item_t options_items[] = {
    _UI_LABEL("Settings"),
    _UI_BREAK,
    _UI_BREAK,
    //
    _UI_LABEL_W("Enable IGR", SCREEN_W / 2),
    _UI_BOOL(UI_OPTIONS_IGR, 0, enable_igr),
    // =======================
    _UI_LINE,
    _UI_LABEL("System informations"),
    _UI_BREAK,
    _UI_BREAK,

    _UI_LABEL_W("Bios", SCREEN_W / 2),
    _UI_LABEL_ID("BIOS_TODO", UI_SYS_BIOS_ID),
    _UI_BREAK,

    _UI_LABEL_W("Region", SCREEN_W / 2),
    _UI_LABEL_ID("BIOS_TODO", UI_SYS_SMPC_REGION),
    _UI_BREAK,

    _UI_LABEL_W("Video frequency", SCREEN_W / 2),
    _UI_LABEL_ID("BIOS_TODO", UI_SYS_VIDEO_FREQ),
    _UI_BREAK,

    _UI_LABEL_W("Region flags", SCREEN_W / 2),
    _UI_LABEL_ID("BIOS_TODO", UI_SYS_REGION_FLAGS),
    _UI_BREAK,

    _UI_LABEL_W("Firmware version", SCREEN_W / 2),
    _UI_LABEL_ID("BIOS_TODO", UI_SYS_HW_FW_REV),
    _UI_BREAK,

    _UI_LABEL_W("Hardware version", SCREEN_W / 2),
    _UI_LABEL_ID("BIOS_TODO", UI_SYS_HW_REV),
    _UI_BREAK,

    _UI_LABEL_W("Hardware UID", SCREEN_W / 2),
    _UI_LABEL_ID("BIOS_TODO", UI_SYS_HW_UID),
    _UI_BREAK,

    _UI_LABEL_W("Firmware date", SCREEN_W / 2),
    _UI_LABEL_ID("BIOS_TODO", UI_SYS_HW_FW_DATE),
    _UI_BREAK,

    _UI_LABEL_W("Wifi status", SCREEN_W / 2),
    _UI_LABEL_ID("BIOS_TODO", UI_SYS_HW_WIFI_STATE),
    _UI_BREAK,

    _UI_LABEL_W("Wifi Ip addr.", SCREEN_W / 2),
    _UI_LABEL_ID("BIOS_TODO", UI_SYS_HW_IP_ADDR),
    _UI_BREAK,

    _UI_LABEL_W("SD Name", SCREEN_W / 2),
    _UI_LABEL_ID("BIOS_TODO", UI_SYS_HW_SD_NAME),
    _UI_BREAK,

    _UI_LABEL_W("SD Type", SCREEN_W / 2),
    _UI_LABEL_ID("BIOS_TODO", UI_SYS_HW_SD_TYPE),
    _UI_BREAK,

    _UI_LABEL_W("SD Size", SCREEN_W / 2),
    _UI_LABEL_ID("BIOS_TODO", UI_SYS_HW_SD_SIZE),
    _UI_BREAK,

    _UI_END};

static void options_init()
{
    ui_item_t *it = ui_get_item_by_id(options_items, UI_OPTIONS_IGR);
    if (it)
    {
        it->number.value = 1;
    }

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
