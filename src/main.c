#include <yaul.h>
#include <stdio.h>
#include <stdlib.h>
#include "fenrir/fenrir.h"
#include "vdp1.config.h"
#include "vdp2.config.h"
#include "font/font.h"
#include "screens/screen.h"
#include "sound_driver/pcm.h"
#include "ui.h"
#include "message_box.h"

#define RESOLUTION_WIDTH (352)
#define RESOLUTION_HEIGHT (224)

fenrir_config_t *fenrir_config;
status_sector_t *status_sector;
sd_dir_entry_t *sd_dir_entries;

int16_t hadoken_snd;

void *zalloc(size_t l)
{
    void *ptr = malloc(l);
    memset(ptr, 0, l);
    return ptr;
}

int main(void)
{
    vdp1_vram_partitions_t vdp1_vram_partitions;
    vdp1_vram_partitions_get(&vdp1_vram_partitions);

    vdp1_init();
    vdp2_init();

    // slave cpu setup
    cpu_dual_comm_mode_set(CPU_DUAL_ENTRY_ICI);

    fenrir_config = (fenrir_config_t *)zalloc(sizeof(fenrir_config_t));
    status_sector = (status_sector_t *)zalloc(sizeof(status_sector_t));
    sd_dir_entries = (sd_dir_entry_t *)zalloc(sizeof(sd_dir_entry_t) * 2500);

    screens_init();

    ui_item_init_t ui_param = {
        .vram = VDP2_VRAM_ADDR(0, 0x00000),
        .cram = VDP2_CRAM_ADDR(0),
    };
    ui_init(&ui_param);

    dbgio_init();
    dbgio_dev_default_init(DBGIO_DEV_VDP2_ASYNC);
    dbgio_dev_font_load();

    // read status
    fenrir_read_configuration(fenrir_config);

#if 0
    switch (fenrir_config->hdr.sd_card_status)
    {

    case FENRIR_SD_CARD_STATUS_CARD_PRESENT:
        screens_select(screen_gamelist);
        break;
    case FENRIR_SD_CARD_STATUS_NO_CARD:
        screens_select(screen_error_no_sd);
        break;
    case FENRIR_SD_CARD_STATUS_WRONG_FS:
        screens_select(screen_error_bad_filesystem);
        break;
    }
#else
    screens_select(screen_options);
#endif

    message_box_t msg = {.type = message_box_error, .message = "Je suis la", .title = "titre"};
    message_box(&msg);
    msg.type = message_box_info;
    msg.message = "Je suis encore la";
    message_box(&msg);

    while (1)
    {
        screens_update();

        vdp1_sync_render();
        vdp1_sync();
        vdp2_sync();
        vdp1_sync_wait();
        vdp2_sync_wait();
    }
    screens_destroy();
}

static void _vblank_out_handler(void *work __unused)
{
    smpc_peripheral_intback_issue();
}
static void _vblank_in_handler(void *work __unused)
{
}

void user_init(void)
{
    static const struct vdp1_env env = {
        .bpp = VDP1_ENV_BPP_16,
        .rotation = VDP1_ENV_ROTATION_0,
        .color_mode = VDP1_ENV_COLOR_MODE_RGB_PALETTE,
        .sprite_type = 0,
        .erase_color = RGB1555(0, 0, 0, 0),
        .erase_points = {
            {0, 0},
            {RESOLUTION_WIDTH, RESOLUTION_HEIGHT}}};

    smpc_peripheral_init();
    cd_block_init();

    vdp2_tvmd_display_res_set(VDP2_TVMD_INTERLACE_NONE, VDP2_TVMD_HORZ_NORMAL_B,
                              VDP2_TVMD_VERT_240);

    vdp2_scrn_back_color_set(VDP2_VRAM_ADDR(3, 0x01FFFE),
                             RGB1555(0, 0, 0, 0));

    vdp2_sprite_priority_set(0, 6);
    vdp1_env_set(&env);
    vdp1_sync_interval_set(0);

    vdp_sync_vblank_out_set(_vblank_out_handler, NULL);
    vdp_sync_vblank_in_set(_vblank_in_handler, NULL);

    cpu_intc_mask_set(0);
    vdp2_tvmd_display_set();
}
