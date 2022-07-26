#include <stdlib.h>
#include <ctype.h>
#include <yaul.h>
#include "fenrir.h"

static int strcicmp(char const *a, char const *b)
{
    for (;; a++, b++)
    {
        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0 || !*a)
            return d;
    }
}

static int sd_compare(const void *s1, const void *s2)
{
    sd_dir_entry_t *e1 = (sd_dir_entry_t *)s1;
    sd_dir_entry_t *e2 = (sd_dir_entry_t *)s2;
    return strcicmp(e1->filename, e2->filename);
}

void fenrir_read_status_sector(status_sector_t *status_sector)
{
    if (EMU_BUILD)
    {
    }
    else
    {
        // cd build
        cd_block_sector_read(FENRIR_READ_STATUS_FAD, status_sector);
    }
}

void fenrir_refresh_entries(sd_dir_t *sd_dir, sd_dir_entry_t *sd_dir_entries)
{
    if (EMU_BUILD)
    {
        sd_dir->hdr.count = 2000;
        sd_dir->hdr.use_cover = 1;
        // emulator
        for (int i = 0; i < sd_dir->hdr.count; i++)
        {
            sd_dir_entries[i].id=i;
            snprintf(sd_dir_entries[i].filename, 55, "%d Dragon Ball Z - La Grande Legende des Boules de Cristal (France, Spain)", i);
        }
        // sd_dir->hdr.sd_card_status = 1;
    }
    else
    {
        // Refresh main configuration
        cd_block_sector_read(FENRIR_READ_CONFIGURATION_FAD, (void *)sd_dir);

        // Set auto patch region
        // set_auto_region();

        // Read gamelist
        uint32_t n_entry_per_sector = 2048 / sizeof(sd_dir_entry_t);
        uint32_t sect_to_read = (sd_dir->hdr.count + n_entry_per_sector - 1) / n_entry_per_sector;
        uint32_t read = 0;
        if (sect_to_read > 0)
            cd_block_sectors_read(FENRIR_READ_FILEBROWSER_START_FAD, (void *)sd_dir_entries, sect_to_read);
        else
            memset(sd_dir_entries, 0, 2048);
    }
    // sort !!
    // if (sd_dir->hdr.count > 1)
    //     qsort(sd_dir_entries, sd_dir->hdr.count, sizeof(sd_dir_entry_t), sd_compare);
}

void fenrir_launch_game(uint32_t id, int boot_method)
{
    fenrir_call(FENRIR_EVENT_LAUNCH_ID_START_FAD + id);

    // todo - stop all interupt + slave

    switch (boot_method)
    {
    case fenrir_boot_dev:
        /** todo **/
        break;
    case fenrir_boot_direct:
    /** todo **/
    case fenrir_boot_cd_player:
        bios_cd_player_execute();
        break;
    case fenrir_boot_card:
    /** todo **/
    case fenrir_boot_reset:
        smpc_smc_resenab_call();
        break;
    }
}

// uint8_t call_buffer[2352];
void fenrir_call(uint32_t sector_addr)
{
    /** todo **/
    // uint8_t *buffer = malloc(2048);
    cd_block_cmd_disk_seek(sector_addr);
    // free(buffer);
}

void fenrir_set_region(uint32_t region_id)
{
    fenrir_call(FENRIR_EVENT_SET_REGION_MIN_FAD + (region_id * 16));
}

void fenrir_set_gamelist_source(uint8_t source)
{
    fenrir_call(FENRIR_EVENT_CHOOSE_SOURCE + source);
}
