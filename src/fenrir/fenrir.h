#pragma once

// Check if running in yabause with hle enabled...
#define EMU_BUILD (0 || (*(uint16_t *)0x800) == 0) // BTR

// smpc value cached by bios
#define bios_get_region_flag() (*(volatile uint32_t *)0x06000248)
#define bios_get_smpc_region() (bios_get_region_flag() >> 12)

#define DIR_ENTRY_ROOT (-1)
#define PARENT_DIRECTORY (3000)

enum fenrir_gamelist_source
{
    fenrir_gamelist_source_sd = 0,
    fenrir_gamelist_source_http = 1
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


#define SP_CMD_CHUNK_SIZE 2048

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

void fenrir_read_status_sector(status_sector_t *status_sector);
void fenrir_refresh_entries(sd_dir_t *sd_dir, sd_dir_entry_t *sd_dir_entries);
void fenrir_set_gamelist_source(uint8_t source);
void fenrir_launch_game(uint32_t id, int boot_method);
void fenrir_call(uint32_t sector_addr);
void fenrir_set_region(uint32_t region_id);

#define FENRIR_TOGGLE_FUNC(func_name, sector_addr, header_field) \
    static inline void fenrir_toggle_##func_name()               \
    {                                                            \
        fenrir_call(sector_addr);                                \
        /*sd_dir->hdr.##header_field ^= 1;*/                     \
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
