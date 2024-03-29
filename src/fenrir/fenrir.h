#pragma once

// Check if running in yabause with hle enabled...
#define EMU_BUILD (0 || (*(uint16_t *)0x800) == 0) // BTR

#define DIR_ENTRY_ROOT (-1)
#define PARENT_DIRECTORY (3000)

#define SP_CMD_CHUNK_SIZE 2048

// cover
#define FENRIR_COVER_W (128)
#define FENRIR_COVER_H (128)
#define FENRIR_COVER_SIZE (FENRIR_COVER_W * FENRIR_COVER_H * 2)

enum fenrir_gamelist_source
{
    fenrir_gamelist_source_sd = 0,
    fenrir_gamelist_source_http,
    fenrir_gamelist_source_smb,
    // fenrir_gamelist_source_usb = 2,
};

enum fenrir_boot_method
{
    fenrir_boot_dev = 0,
    fenrir_boot_direct = 1,
    fenrir_boot_cd_player = 2,
    fenrir_boot_card = 3,
    fenrir_boot_reset = 4,
};

enum sd_card_status
{
    sd_card_status_card_present = 0,
    sd_card_status_no_card = 1,
    sd_card_status_wrong_fs = 2
};

enum fenrir_entry_type
{
    TYPE_ERR = 0,
    TYPE_DIR = 1,
    TYPE_FILE = 2,
};
typedef struct __attribute__((__packed__))
{
    int16_t id;
    uint16_t flag;
    uint32_t size;
    char name[54];
} fenrir_dir_entry_t;

// packed in a 2048bytes struct
typedef struct __attribute__((__packed__))
{
    uint16_t count; // => -1 end or empty
    fenrir_dir_entry_t entries[33];
} fenrir_dir_data_t;

// subcmd
enum
{
    // general
    sp_cmd_0x00 = 0,
    // write
    sp_cmd_write_open = 0x10,
    sp_cmd_write = 0x11,
    sp_cmd_write_flush = 0x12,
    sp_cmd_write_close = 0x13
    //
};

#include "sd_dir.h"
#include "menu.sectors.h"

/*****************************************************
 * fenrir main functions
 ****************************************************/
void fenrir_read_configuration(fenrir_config_t *fenrir_config);
void fenrir_read_status_sector(status_sector_t *status_sector);
void fenrir_refresh_entries(fenrir_config_t *fenrir_config, sd_dir_entry_t *sd_dir_entries);
void fenrir_set_gamelist_source(uint8_t source);
void fenrir_launch_game(uint32_t id, int boot_method);
void fenrir_call(uint32_t sector_addr);
void fenrir_set_region(uint32_t region_id);
void fenrir_get_cover(uint32_t id, uint8_t *cover);

// send a read requset
int fenrir_read_configuration_req();
int fenrir_async_req_ready();
int fenrir_async_data_ready();
int fenrir_async_read_response(void *out);

#define FENRIR_TOGGLE_FUNC(func_name, sector_addr, header_field) \
    static inline void fenrir_toggle_##func_name()               \
    {                                                            \
        fenrir_call(sector_addr);                                \
        /*fenrir_config->hdr.##header_field ^= 1;*/              \
    }

FENRIR_TOGGLE_FUNC(auto_reload, FENRIR_EVENT_TOGGLE_AUTO_RELOAD, auto_reload)
FENRIR_TOGGLE_FUNC(cover, FENRIR_EVENT_TOGGLE_COVER, use_cover)

/*****************************************************
 * fenrir directory operation
 ****************************************************/

// use DIR_ENTRY_ROOT to read from the root of sd
// if id select a dir, use fenrir_readdir
// if id select a file, use fenrir_readfile_packet
// return -1 on error
int fenrir_select_direntry(int16_t id);

// Read dir, by 2048 bytes chunks, no seek possible
// max entries is 1000 (limited by fw implementation)
int fenrir_readdir(fenrir_dir_data_t *fenrir_dir_data);

// Read file, by 2048 bytes chunks, no seek possible, file size is given by fenrir_readdir
int fenrir_readfile_packet(void *buffer);

// Look for a file
fenrir_dir_entry_t *fenrir_find_entry(fenrir_dir_data_t *fenrir_dir_data, char *name);

/*****************************************************
 * fenrir hw revision
 ****************************************************/

#define FENRIR_HW_REV_0_STR "20"
#define FENRIR_HW_REV_1_STR "21"
#define FENRIR_HW_REV_2_STR "DUO"

static inline char *fenrir_hw_rev_str(uint8_t rev)
{
    switch (rev)
    {
    case 1:
        return FENRIR_HW_REV_1_STR;
    case 2:
        return FENRIR_HW_REV_2_STR;
    case 0:
    default:
        return FENRIR_HW_REV_0_STR;
    }
}
