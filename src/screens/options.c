#include <yaul.h>
#include "options.h"
#include "screen.h"
#include "../ui.h"

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
    i->toggle.value = !i->toggle.value;
}

static ui_item_t options_items[] = {
    _UI_LABEL("Settings"),
    _UI_BREAK,
    //
    _UI_LABEL("Enable IGR"),
    _UI_BOOL(UI_OPTIONS_IGR, 0, enable_igr),
    // =======================
    _UI_LINE,
    _UI_LABEL("System informations"),
    _UI_BREAK,
    
    _UI_LABEL("Bios"),
    _UI_LABEL("BIOS_TODO"),
    _UI_BREAK,
    
    _UI_LABEL("Region"),
    _UI_LABEL("BIOS_TODO"),
    _UI_BREAK,
    
    _UI_LABEL("Video frequency"),
    _UI_LABEL("BIOS_TODO"),
    _UI_BREAK,
    
    _UI_LABEL("Region flags"),
    _UI_LABEL("BIOS_TODO"),
    _UI_BREAK,

    _UI_LABEL("Firmware version"),
    _UI_LABEL("BIOS_TODO"),
    _UI_BREAK,

    _UI_LABEL("Hardware version"),
    _UI_LABEL("BIOS_TODO"),
    _UI_BREAK,

    _UI_LABEL("Hardware UID"),
    _UI_LABEL("BIOS_TODO"),
    _UI_BREAK,

    _UI_LABEL("Firmware date"),
    _UI_LABEL("BIOS_TODO"),
    _UI_BREAK,

    _UI_LABEL("Wifi status"),
    _UI_LABEL("BIOS_TODO"),
    _UI_BREAK,
    
    _UI_LABEL("Wifi Ip addr."),
    _UI_LABEL("BIOS_TODO"),
    _UI_BREAK,
    
    _UI_LABEL("SD Name"),
    _UI_LABEL("BIOS_TODO"),
    _UI_BREAK,
    
    _UI_LABEL("SD Type"),
    _UI_LABEL("BIOS_TODO"),
    _UI_BREAK,
    
    _UI_LABEL("SD Size"),
    _UI_LABEL("BIOS_TODO"),
    _UI_BREAK,

    _UI_END};

static void options_init()
{
    ui_item_t *it = ui_get_item_by_id(options_items, UI_OPTIONS_IGR);
    if (it)
    {
        it->toggle.value = 1;
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
