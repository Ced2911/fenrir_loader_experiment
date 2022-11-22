#include <stdlib.h>
#include <yaul.h>
#include "fenrir.h"
#include "bios_ex.h"
#include "cd-miss.h"

extern void sys_reset(void);

static int sd_compare(const void *s1, const void *s2)
{
    sd_dir_entry_t *e1 = (sd_dir_entry_t *)s1;
    sd_dir_entry_t *e2 = (sd_dir_entry_t *)s2;
    return strcicmp(e1->filename, e2->filename);
}

static void __noreturn fenrir_direct_boot()
{
    cd_block_status_t cd;

    // wait 2 frames
    for (int i = 0; i < 2; i++)
    {

        vdp1_sync_render();
        vdp1_sync();
        vdp2_sync();
        vdp1_sync_wait();
    }
    int s = 0;

    for (;;)
    {
        switch (s)
        {
        // wait until disc is mounted
        case 0:
        {
            cd_block_cmd_status_get(&cd);
            if (cd.cd_status == CD_STATUS_NO_DISC)
            {
                vdp1_sync_render();
                vdp1_sync();
                vdp2_sync();
                vdp1_sync_wait();
            }
            else
            {
                s = 1;
            }
            break;
        }

        // wait auth
        case 1:
        {
            uint16_t check;
            cd_block_cmd_disk_auth();
            // Wait till operation is finished
            while ((MEMORY_READ(16, CD_BLOCK(HIRQ)) & EFLS) == 0)
                ;
            cd_block_cmd_auth_check(&check);

            // check finished
            s = 0x10 + check;
            break;
        }

        // launch disc
        case 0x14:
        {
            bios_cd_init();
            bios_cd_read();
            while (1)
                bios_cd_boot();
            break;
        }
        // launch cd player (unknown cd, audio or video)
        default:
            bios_cd_player_execute();
            break;
        }
    }
}

void fenrir_read_configuration(sd_dir_t *sd_dir)
{
    if (EMU_BUILD)
    {
        sd_dir->hdr.sd_card_status = 0;//FENRIR_SD_CARD_STATUS_NO_CARD;
        sd_dir->hdr.count = 25;
        sd_dir->hdr.use_cover = 1;
    }
    else
    {
        cd_block_sector_read(FENRIR_READ_CONFIGURATION_FAD, (void *)sd_dir);
    }
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
        sd_dir->hdr.count = 25;
        sd_dir->hdr.use_cover = 1;
        // emulator
        for (int i = 0; i < sd_dir->hdr.count; i++)
        {
            sd_dir_entries[i].id = i;
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
            cd_block_sectors_read(FENRIR_READ_FILEBROWSER_START_FAD, (void *)sd_dir_entries, sect_to_read * 2048);
        else
            memset(sd_dir_entries, 0, 2048);
    }
    // sort !!
    // if (sd_dir->hdr.count > 1)
    //     qsort(sd_dir_entries, sd_dir->hdr.count, sizeof(sd_dir_entry_t), sd_compare);
}

void __noreturn fenrir_launch_game(uint32_t id, int boot_method)
{
    // stop interrupts and slave cpu
    sys_reset();

    // launch game
    fenrir_call(FENRIR_EVENT_LAUNCH_ID_START_FAD + id);

    while (1)
    {
        switch (boot_method)
        {
            /** todo **/
        case fenrir_boot_dev:
            break;
        case fenrir_boot_direct:
            fenrir_direct_boot();
            break;
        case fenrir_boot_cd_player:
            bios_cd_player_execute();
            break;
        /** todo **/
        case fenrir_boot_card:
        case fenrir_boot_reset:
            smpc_smc_nmireq_call();
            break;
        };
    }
}

void fenrir_get_cover(uint32_t id, uint8_t *cover)
{
    if (EMU_BUILD)
    {
        // memset(cover, 0xFF, FENRIR_COVER_SIZE);
        static int j = 0;
        uint16_t *dst = (uint16_t *)cover;
        for (int i = 0; i < (128 * 96); i++)
        {
            *dst++ = 0x8000 | (j << 5) | i;
        }
        j++;
    }
    else
    {
        cd_block_sectors_read(FENRIR_SCREENSHOT_ID_START_FAD + id, cover, FENRIR_COVER_SIZE);
    }
}

static uint8_t buffer[2352];
void fenrir_call(uint32_t sector_addr)
{
    /** todo **/
    // uint8_t *buffer = malloc(2048);
    cd_block_sector_read(sector_addr, buffer);
    // free(buffer);
    // cd_block_cmd_disk_seek(sector_addr);
}

void fenrir_set_region(uint32_t region_id)
{
    fenrir_call(FENRIR_EVENT_SET_REGION_MIN_FAD + (region_id * 16));
}

void fenrir_set_gamelist_source(uint8_t source)
{
    fenrir_call(FENRIR_EVENT_CHOOSE_SOURCE + source);
}
