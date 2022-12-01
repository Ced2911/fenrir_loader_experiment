#pragma once

#include <yaul.h>
#include <stdlib.h>
#include "screen.h"
#include "fenrir/fenrir.h"
#include "fenrir/fileio/fileio.h"
#include "bram.h"

#define BRAM_SIZE (32 * 1024)

struct
{
    uint32_t bram_offset;
    char filename[256];
    uint8_t *data;
} bck_ctx;

static void smpc_get_time(smpc_time_dec_t *time_dec)
{
    const smpc_time_t *const time = smpc_rtc_time_get();
    vdp2_tvmd_vblank_in_wait();
    vdp2_tvmd_vblank_out_wait();
    smpc_peripheral_process();
    smpc_rtc_time_bcd_from(time, time_dec);
}

static void _vbk()
{
    // smpc - input
    smpc_peripheral_intback_issue();
}

static void do_progress_bar(int cur, int max)
{
}

static void bram_backup_init()
{
    smpc_time_dec_t time_dec;
    bck_ctx.bram_offset = 0;
    if (!bck_ctx.data)
    {
        bck_ctx.data = (uint8_t *)malloc(SP_CMD_CHUNK_SIZE);
    }

    sprintf(bck_ctx.filename, "/sdcard/bram_%04d%02d%02d_%02d%02d%02d.bin",
            time_dec.year, time_dec.month, time_dec.day, time_dec.hours, time_dec.minutes, time_dec.seconds);

    fenrir_special_cmd_start();
    fenrir_openfile(bck_ctx.filename);

    // setup vblank
    vdp_sync_vblank_out_set(_vbk, NULL);
}

static void bram_backup_update()
{
    if (bck_ctx.bram_offset > BRAM_SIZE)
    {
        info_set_title("Backup ram");
        info_set_message("Write complete");
        info_wait_for_btn();
    }
    else
    {

        bram_read_u8(bck_ctx.data, SP_CMD_CHUNK_SIZE, bck_ctx.bram_offset);
        bck_ctx.bram_offset += SP_CMD_CHUNK_SIZE;

        do_progress_bar(bck_ctx.bram_offset, BRAM_SIZE);

        // send data
        if (fenrir_writefile(bck_ctx.data, SP_CMD_CHUNK_SIZE) != 0)
        {
            // Display error...
            error_set_title("Backup ram");
            error_set_message("Failed to write to sd card");
            error_wait_for_btn();

            // close
            screens_select(screen_options);
            return;
        }
    }
}

static void bram_backup_destroy()
{
    // close
    fenrir_closefile();

    if (bck_ctx.data)
    {
        free(bck_ctx.data);
        bck_ctx.data = NULL;
    }
}

screen_t bram_backup = {
    .init = bram_backup_init,
    .update = bram_backup_update,
    .destroy = bram_backup_destroy,
};