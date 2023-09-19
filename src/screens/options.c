#include <yaul.h>
#include "options.h"
#include "screens.h"
#include "../ui.h"
#include "bios_ex.h"
#include "fenrir/fenrir.h"
#include "fenrir/region.h"

#define SCREEN_W (352)
#define OPTION_LABEL_W (120)

#define GET_LABEL_BY_ID(ID) ui_get_item_by_id(options_items, ID)->label.text

static const rgb1555_t option_bg_color = RGB888_RGB1555_INITIALIZER(1, 4, 8, 15);
static const rgb1555_t option_default_color = RGB888_RGB1555_INITIALIZER(1, 80, 125, 188);
static const rgb1555_t option_highlight_color = RGB888_RGB1555_INITIALIZER(1, 161, 198, 234);

extern fenrir_config_t *fenrir_config;

static void set_diag_screen(ui_item_t *item);
static void set_backup_bram_screen(ui_item_t *item);
static void set_restore_bram_screen(ui_item_t *item);

static const char *wifi_state_str[] = {
    "DISCONNECTED",
    "CONNECTING",
    "CONNECTED",
};

enum
{
    UI_OPTIONS_IGR = 1,
    UI_SYS_BIOS_ID,
    UI_SYS_SMPC_REGION,
    UI_SYS_VIDEO_FREQ,
    UI_SYS_REGION_FLAGS,
    UI_SYS_FW,
    UI_SYS_HW,
    UI_SYS_WIFI_STATE,
    UI_SYS_IP_ADDR,
    UI_SYS_SD,
    UI_SYS_SMB,
    UI_SYS_SMB_URL,
};

static ui_item_t options_items[] = {
    _UI_TITLE("Settings"),
    _UI_BREAK,
    //
    // _UI_LABEL_W("Enable IGR", OPTION_LABEL_W),
    // _UI_BOOL(UI_OPTIONS_IGR, 0, enable_igr),
    // _UI_BREAK,
    //
    {.type = UI_LABEL, .label = {.text = "Diagnostics (Missing)"}, .w = SCREEN_W / 2, .handler = set_diag_screen},
    _UI_BREAK,
    {.type = UI_LABEL, .label = {.text = "Backup BRAM (Missing)"}, .w = SCREEN_W / 2, .handler = set_backup_bram_screen},
    _UI_BREAK,
    {.type = UI_LABEL, .label = {.text = "Restore BRAM (Missing)"}, .w = SCREEN_W / 2, .handler = set_restore_bram_screen},
    // =======================
    _UI_LINE,
    _UI_TITLE("System informations"),
    _UI_BREAK,

    _UI_LABEL_W("Bios", OPTION_LABEL_W),
    _UI_LABEL_NULL_ID(UI_SYS_BIOS_ID),
    _UI_BREAK,

    _UI_LABEL_W("Video frequency", OPTION_LABEL_W),
    _UI_LABEL_NULL_ID(UI_SYS_VIDEO_FREQ),
    _UI_BREAK,

    _UI_LABEL_W("Region", OPTION_LABEL_W),
    _UI_LABEL_ID_W(UI_SYS_SMPC_REGION, SCREEN_W / 4),
    _UI_BREAK,

    _UI_LABEL_W("Region flags", OPTION_LABEL_W),
    _UI_LABEL_ID_W(UI_SYS_REGION_FLAGS, SCREEN_W / 4),
    _UI_BREAK,

    _UI_LABEL_W("Firmware ", OPTION_LABEL_W),
    _UI_LABEL_NULL_ID(UI_SYS_FW),
    _UI_BREAK,

    _UI_LABEL_W("Hardware", OPTION_LABEL_W),
    _UI_LABEL_NULL_ID(UI_SYS_HW),
    _UI_BREAK,

    _UI_LABEL_W("Wifi status", OPTION_LABEL_W),
    _UI_LABEL_NULL_ID(UI_SYS_WIFI_STATE),
    _UI_BREAK,

    _UI_LABEL_W("Wifi Ip addr.", OPTION_LABEL_W),
    _UI_LABEL_NULL_ID(UI_SYS_IP_ADDR),
    _UI_BREAK,

    _UI_LABEL_W("SD Card: ", OPTION_LABEL_W),
    _UI_LABEL_NULL_ID(UI_SYS_SD),
    _UI_BREAK,

    _UI_LABEL_W("SMB: ", OPTION_LABEL_W),
    _UI_LABEL_NULL_ID(UI_SYS_SMB),
    _UI_BREAK,

    _UI_LABEL_W("SMB URL: ", OPTION_LABEL_W),
    _UI_LABEL_NULL_ID(UI_SYS_SMB_URL),
    _UI_BREAK,

    _UI_END};

static void enable_igr(ui_item_t *i)
{
    i->number.value = !i->number.value;
}

static void set_diag_screen(ui_item_t *item)
{
    screens_select(screen_diagnostic);
}

static void set_backup_bram_screen(ui_item_t *item)
{
    screens_select(screen_backup_bram);
}

static void set_restore_bram_screen(ui_item_t *item)
{
    screens_select(screen_restore_bram);
}

static void options_diag_allocate_str()
{
    ui_item_t *item = options_items;

    while (item->type != UI_END)
    {
        if (item->type == UI_LABEL && item->id && item->label.text == NULL)
        {
            item->label.text = (char *)malloc(40);
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
            item->label.text = 0;
        }
        item++;
    }
}

static void options_input_handler(smpc_peripheral_digital_t *digital, void *unused)
{
    if (digital->held.button.b)
    {
        screens_select(screen_gamelist);
    }
}

static void options_draw_text()
{
    char *hw_rev = fenrir_hw_rev_str(fenrir_config->hdr.hw_rev);
    char ip_addr[(4 * 4) + 1];
    char fenrir_uuid[24];

    uint32_t region_flag = bios_get_region_flag();

    snprintf(GET_LABEL_BY_ID(UI_SYS_BIOS_ID), 24, "%s", (char *)0x800);
    strncpy(GET_LABEL_BY_ID(UI_SYS_SMPC_REGION), region_get_smpc_string(), 24);
    snprintf(GET_LABEL_BY_ID(UI_SYS_VIDEO_FREQ), 24, "%s Hz", region_flag & 0x800 ? "50" : "60");
    snprintf(GET_LABEL_BY_ID(UI_SYS_REGION_FLAGS), 24, "%08lx", region_flag);

    snprintf(GET_LABEL_BY_ID(UI_SYS_FW), 40, "%s / %s", fenrir_config->hdr.firmware_version, fenrir_config->hdr.build_date);
    snprintf(GET_LABEL_BY_ID(UI_SYS_HW), 40, "%s / %02x%02x%02x%02x", hw_rev, fenrir_config->hdr.card_uid[0], fenrir_config->hdr.card_uid[1], fenrir_config->hdr.card_uid[2], fenrir_config->hdr.card_uid[3]);

    strncpy(GET_LABEL_BY_ID(UI_SYS_WIFI_STATE), wifi_state_str[fenrir_config->hdr.wifi_state & 0b11], 24);
    snprintf(GET_LABEL_BY_ID(UI_SYS_IP_ADDR), 24, "%d.%d.%d.%d", fenrir_config->hdr.local_ip[0], fenrir_config->hdr.local_ip[1], fenrir_config->hdr.local_ip[2], fenrir_config->hdr.local_ip[3]);

    snprintf(GET_LABEL_BY_ID(UI_SYS_SMB), 24, "%02x", fenrir_config->hdr.smb_status);
    strncpy(GET_LABEL_BY_ID(UI_SYS_SMB_URL), fenrir_config->hdr.smb_url, 64);
}

static void options_init()
{
    options_diag_allocate_str();
    options_draw_text();

    ui_set_color(COLOR_BACKGROUND, option_bg_color);
    ui_set_color(COLOR_HIGHLIGHT, option_default_color);
    ui_set_color(COLOR_DEFAULT, option_highlight_color);

    ui_clear();
    ui_render(options_items);
}

static void options_update()
{
    ui_update(options_items, options_input_handler, NULL);
}

static void options_destroy()
{
    options_diag_free_str();
}

screen_t options_screen = {
    .type = screen_options,
    .init = options_init,
    .update = options_update,
    .destroy = options_destroy,
};

// @ todo
screen_t diagnostics_screen = {
    .type = screen_diagnostic,
    .init = options_init,
    .update = options_update,
    .destroy = options_destroy,
};
