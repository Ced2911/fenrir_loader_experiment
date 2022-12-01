/*****************************************************
 * WIP
 * Abuse seek to transfer data to sd card
 *****************************************************/

#include <yaul.h>
#include "fastlz/fastlz.h"
#include "fenrir/fenrir.h"
#include "fileio.h"

// @todo
extern void timer_stop();
#define timer_setup(...)

#define timer_delay(...)
#define log_debug(...)
#define log_error(...)
#define display_error(...)
#define stop_cd_audio(...)

static uint8_t compressed[SP_CMD_CHUNK_SIZE + 100];

enum
{
    sp_cmd_command = 1,
    sp_cmd_continue = 2
};


static uint32_t timer_freq = 0;

void fenrir_special_cmd_end()
{
    // timer_stop();
}

void fenrir_special_cmd_start()
{
   // timer_setup(TIMER_HBLANK, &timer_freq);
}

static int fenrir_special_cmd_wait_stats(cd_block_status_t *cd_status, uint32_t fad)
{
    int count = 0;
    while (1)
    {
        count++;
        // wait for seek end...
        // if (cd_get_stat(cd_status) == 0 && cd_status->status == STATUS_PAUSE && cd_status->FAD == fad)
        if (cd_block_cmd_status_get(cd_status) == 0 && cd_status->cd_status == CD_STATUS_SEEK && cd_status->fad == fad)
        {
            return 0;
        }

        // wait a bit
        for (int i = 0; i < 20; i++)
        {
        }

        // timeout...
        if (count > 24000)
        {
            return -1;
        }
    }
}

static int fenrir_special_cmd_write(uint8_t cmd, uint16_t args)
{
    int retry = 3;
    uint32_t sent_fad = cmd << 16 | args;

    while (retry > 0)
    {
        cd_block_status_t cd_status = {
            .fad = -1,
            .cd_status = -1};

        if (cd_block_cmd_disk_seek(sent_fad) == 0)
        {
            // wait a few...
            timer_delay(timer_freq, 7);
            // vdp_vsync();

            // wait for return status
            if (fenrir_special_cmd_wait_stats(&cd_status, sent_fad) == 0)
            {
                // transfert complete
                return 0;
            }
        }

        retry--;
    }

    // tx error...
    return -1;
}

int fenrir_special_cmd(uint8_t subcmd, uint8_t arg)
{
    return fenrir_special_cmd_write(sp_cmd_command, (subcmd << 8) | arg);
}

int fenrir_special_cmd_continue(uint8_t arg_0, uint8_t arg_1)
{
    return fenrir_special_cmd_write(sp_cmd_continue, (arg_0 << 8) | arg_1);
}

int fenrir_special_cmd_continue_mem(void *data, int len)
{
    uint8_t *ptr = (uint8_t *)data;

    for (int i = 0; i < len; i += 2)
    {
        uint8_t a = *ptr++;
        uint8_t b = *ptr++;
        if (fenrir_special_cmd_write(sp_cmd_continue, (a << 8) | b) != 0)
        {
            // tx error...
            return -1;
        }
    }

    return 0;
}

extern void display_error(const char *err);

#define FENRIR_SPC_CHECK(x)                   \
    do                                        \
    {                                         \
        if (x != 0)                           \
        {                                     \
            display_error("Tx error... " #x); \
            return -1;                        \
        }                                     \
    } while (0);

///

// Write is asynchrone
// Check status_sector flags
// When write is finished
// status => FENRIR_STATUS_BUSY
// type => FENRIR_STATUS_TYPE_WRITE_FILE
// write_op => FENRIR_WRITE_CHUNK / FENRIR_WRITE_CHUNK_ERROR
enum
{
    FENRIR_CHECK_WRITE_STATUS_OK = 0,
    FENRIR_CHECK_WRITE_STATUS_ERROR = 1,
    FENRIR_CHECK_WRITE_STATUS_PENDING,
    FENRIR_CHECK_WRITE_STATUS_FATAL,
};
static int fenrir_check_write_status(status_sector_t *status_sector)
{
    // log_status_sector(status_sector);
    if (status_sector->status == FENRIR_STATUS_BUSY)
    {
        if (status_sector->type == FENRIR_STATUS_TYPE_WRITE_FILE)
        {
            if ((status_sector->write_op == FENRIR_WRITE_CHUNK))
            {
                log_debug("write complete");
                return FENRIR_CHECK_WRITE_STATUS_OK;
            }
            if ((status_sector->write_op == FENRIR_WRITE_CHUNK_ERROR))
            {
                log_debug("write error");
                return FENRIR_CHECK_WRITE_STATUS_ERROR;
            }
            if (status_sector->write_op == FENRIR_WRITE_PENDING)
            {
                log_debug("pending operation");
                return FENRIR_CHECK_WRITE_STATUS_PENDING;
            }
        }
    }

    log_debug("busy ?");
    return FENRIR_CHECK_WRITE_STATUS_FATAL;
}

int fenrir_writefile(void *in, uint16_t datalen)
{
#if 1
    int len = fastlz_compress_level(1, in, datalen, compressed);
#else
    int len = 256;
    for (int i = 0; i < len; i++)
    {
        compressed[i] = i;
    }
#endif
    int chksum = chunk_checksum(compressed, len);

    status_sector_t status_sector;
    for (int retry = 0; retry < 3; retry++)
    {
        status_sector.status = -1;
        status_sector.write_op = -1;
        status_sector.chunk_checksum = -1;

#if 1
        FENRIR_SPC_CHECK(fenrir_special_cmd(sp_cmd_write, chksum));
#else
        // Create a error for testing...
        FENRIR_SPC_CHECK(fenrir_special_cmd(sp_cmd_write, (retry == 0) ? chksum + 1 : chksum));
#endif
        // vdp_vsync();

        FENRIR_SPC_CHECK(fenrir_special_cmd_continue((len >> 8) & 0xFF, len & 0xFF));
        // vdp_vsync();

        FENRIR_SPC_CHECK(fenrir_special_cmd_continue_mem(compressed, len));
        // vdp_vsync();

        FENRIR_SPC_CHECK(fenrir_special_cmd(sp_cmd_write_flush, 0));

        // Check ....
        stop_cd_audio();

        do
        {
            fenrir_read_status_sector(&status_sector);
#if 0
            if (status_sector.status != FENRIR_STATUS_BUSY)
            {
                // special cmd failed, no retry !!
                return -1;
            }

            if (status_sector.write_op == FENRIR_WRITE_PENDING)
            {
                // not complete yet...
                continue;
            }

            if (status_sector.write_op == FENRIR_WRITE_CHUNK_ERROR)
            {
                // chunk error retry
                break;
            }

            if (status_sector.write_op == FENRIR_WRITE_CHUNK &&
                status_sector.chunk_checksum != chksum)
            {
                // checksum error retry
                break;
            }

            if (status_sector.write_op == FENRIR_WRITE_CHUNK &&
                status_sector.chunk_checksum == chksum)
            {
                // complete without error !
                return 0;
            }
#else
            int err = fenrir_check_write_status(&status_sector);
            if (err == FENRIR_CHECK_WRITE_STATUS_OK)
            {
                // complete without error !
                log_debug("status ok");
                return 0;
            }
            else if (err == FENRIR_CHECK_WRITE_STATUS_ERROR)
            {
                // complete with error !
                log_debug("status err, retry");
                break;
            }

            else if (err == FENRIR_CHECK_WRITE_STATUS_PENDING)
            {
                // not yet completed !
                log_debug("status pending, wait...");
                continue;
            }
            else if (err == FENRIR_CHECK_WRITE_STATUS_FATAL)
            {
                // status error
                log_error("status error");
                return -1;
            }

#endif

        } while (1);

        // wait a few
        vdp2_sync_wait();
    }

    log_error("status error");
    return -1;
}
