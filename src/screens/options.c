#include <yaul.h>
#include "options.h"
#include "screens.h"
#include "../ui.h"
#include "bios_ex.h"
#include "fenrir/fenrir.h"
#include "fenrir/region.h"

#define SCREEN_W (352)

#define GET_LABEL_BY_ID(ID) ui_get_item_by_id(options_items, ID)->label.text

extern fenrir_config_t *fenrir_config;

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
    _UI_TITLE("Settings"),
    _UI_BREAK,
    //
    _UI_LABEL_W("Enable IGR", SCREEN_W / 2),
    _UI_BOOL(UI_OPTIONS_IGR, 0, enable_igr),
    // =======================
    _UI_LINE,
    _UI_TITLE("System informations"),
    _UI_BREAK,

    _UI_LABEL_W("Bios", SCREEN_W / 2),
    _UI_LABEL_NULL_ID(UI_SYS_BIOS_ID),
    _UI_BREAK,

    _UI_LABEL_W("Video frequency", SCREEN_W / 2),
    _UI_LABEL_NULL_ID(UI_SYS_VIDEO_FREQ),
    _UI_BREAK,

    _UI_LABEL_W("Region", SCREEN_W / 2),
    _UI_LABEL_NULL_ID(UI_SYS_SMPC_REGION),
    _UI_BREAK,

    _UI_LABEL_W("Region flags", SCREEN_W / 2),
    _UI_LABEL_NULL_ID(UI_SYS_REGION_FLAGS),
    _UI_BREAK,

    _UI_LABEL_W("Firmware version", SCREEN_W / 2),
    _UI_LABEL_NULL_ID(UI_SYS_HW_FW_REV),
    _UI_BREAK,

    _UI_LABEL_W("Hardware version", SCREEN_W / 2),
    _UI_LABEL_NULL_ID(UI_SYS_HW_REV),
    _UI_BREAK,

    _UI_LABEL_W("Hardware UID", SCREEN_W / 2),
    _UI_LABEL_NULL_ID(UI_SYS_HW_UID),
    _UI_BREAK,

    _UI_LABEL_W("Firmware date", SCREEN_W / 2),
    _UI_LABEL_NULL_ID(UI_SYS_HW_FW_DATE),
    _UI_BREAK,

    _UI_LABEL_W("Wifi status", SCREEN_W / 2),
    _UI_LABEL_NULL_ID(UI_SYS_HW_WIFI_STATE),
    _UI_BREAK,

    _UI_LABEL_W("Wifi Ip addr.", SCREEN_W / 2),
    _UI_LABEL_NULL_ID(UI_SYS_HW_IP_ADDR),
    _UI_BREAK,

    _UI_LABEL_W("SD Name", SCREEN_W / 2),
    _UI_LABEL_NULL_ID(UI_SYS_HW_SD_NAME),
    _UI_BREAK,

    _UI_LABEL_W("SD Type", SCREEN_W / 2),
    _UI_LABEL_NULL_ID(UI_SYS_HW_SD_TYPE),
    _UI_BREAK,

    _UI_LABEL_W("SD Size", SCREEN_W / 2),
    _UI_LABEL_NULL_ID(UI_SYS_HW_SD_SIZE),
    _UI_BREAK,

    _UI_END};

static void options_diag_allocate_str()
{
    ui_item_t *item = options_items;

    while (item->type != UI_END)
    {
        if (item->type == UI_LABEL && item->id && item->label.text == NULL)
        {
            item->label.text = (char *)malloc(24);
            item->label.text[0] = 0;
        }
        item++;
    }
}

static void options_diag_free_str()
{
    ui_item_t *item = options_items;

    while (item->type != UI_END)
    {
        if (item->type == UI_LABEL && item->id && item->label.text != NULL)
        {
            free(item->label.text);
        }
        item++;
    }
}

static const char *wifi_state_str[] = {
    "DISCONNECTED",
    "CONNECTING",
    "CONNECTED"};

static void options_init()
{
    char *hw_rev = fenrir_hw_rev_str(fenrir_config->hdr.hw_rev);
    char ip_addr[(4 * 4) + 1];
    char fenrir_uuid[24];

    uint32_t region_flag = bios_get_region_flag();

    options_diag_allocate_str();

    snprintf(GET_LABEL_BY_ID(UI_SYS_BIOS_ID), 24, "%s", (char *)0x800);
    strncpy(GET_LABEL_BY_ID(UI_SYS_SMPC_REGION), region_get_smpc_string(), 24);
    snprintf(GET_LABEL_BY_ID(UI_SYS_VIDEO_FREQ), 24, "%s Hz", region_flag & 0x800 ? "50" : "60");
    snprintf(GET_LABEL_BY_ID(UI_SYS_REGION_FLAGS), 24, "%08lx", region_flag);
    
    strncpy(GET_LABEL_BY_ID(UI_SYS_HW_FW_REV), fenrir_config->hdr.firmware_version, 24);
    strncpy(GET_LABEL_BY_ID(UI_SYS_HW_REV), hw_rev, 24);
    snprintf(GET_LABEL_BY_ID(UI_SYS_HW_UID), 24, "%02x%02x%02x%02x", fenrir_config->hdr.card_uid[0], fenrir_config->hdr.card_uid[1], fenrir_config->hdr.card_uid[2], fenrir_config->hdr.card_uid[3]);
    strncpy(GET_LABEL_BY_ID(UI_SYS_HW_FW_DATE), fenrir_config->hdr.build_date, 24);
    strncpy(GET_LABEL_BY_ID(UI_SYS_HW_WIFI_STATE), wifi_state_str[fenrir_config->hdr.wifi_state & 0b11], 24);    
    snprintf(GET_LABEL_BY_ID(UI_SYS_HW_IP_ADDR), 24, "%d.%d.%d.%d", fenrir_config->hdr.local_ip[0], fenrir_config->hdr.local_ip[1], fenrir_config->hdr.local_ip[2], fenrir_config->hdr.local_ip[3]);
    
    strncpy(GET_LABEL_BY_ID(UI_SYS_HW_SD_NAME), fenrir_config->hdr.sd_name, 24);
    strncpy(GET_LABEL_BY_ID(UI_SYS_HW_SD_TYPE), fenrir_config->hdr.sd_type, 24);
    snprintf(GET_LABEL_BY_ID(UI_SYS_HW_SD_SIZE), 24, "%ldMB", fenrir_config->hdr.sd_size);

    ui_item_t *it = ui_get_item_by_id(options_items, UI_OPTIONS_IGR);
    if (it)
    {
        it->number.value = fenrir_config->hdr.auto_reload;
    }

    ui_render(options_items);
}

static void options_update()
{
    ui_update(options_items);
}

static void options_destroy()
{
    options_diag_free_str();
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
