#include <yaul.h>
#include <stdio.h>
#include <stdlib.h>
#include "fenrir/fenrir.h"
#include "vdp1.config.h"
#include "vdp2.config.h"
#include "font/font.h"
#include "screens/gamelist.h"

#define RESOLUTION_WIDTH (352)
#define RESOLUTION_HEIGHT (224)

sd_dir_t *sd_dir;
status_sector_t *status_sector;
sd_dir_entry_t *sd_dir_entries;

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

    sd_dir = (sd_dir_t *)zalloc(sizeof(sd_dir_t));
    status_sector = (status_sector_t *)zalloc(sizeof(status_sector_t));
    sd_dir_entries = (sd_dir_entry_t *)zalloc(sizeof(sd_dir_entry_t) * 2500);

    screen_t *screen = &gamelist_screen;
    screen->init();

    while (1)
    {
        screen->update();

        vdp1_sync_render();
        vdp1_sync();
        vdp2_sync();
        vdp1_sync_wait();
        //  vdp2_sync_wait();
    }
    screen->destroy();
}

static void _vblank_out_handler(void *work __unused)
{
    smpc_peripheral_intback_issue();
}

void user_init(void)
{
    static const struct vdp1_env env = {
        .bpp = VDP1_ENV_BPP_16,
        .rotation = VDP1_ENV_ROTATION_0,
        .color_mode = VDP1_ENV_COLOR_MODE_RGB_PALETTE,
        .sprite_type = 0,
        .erase_color = COLOR_RGB1555(0, 0, 0, 0),
        .erase_points = {
            {0, 0},
            {RESOLUTION_WIDTH, RESOLUTION_HEIGHT}}};

    smpc_peripheral_init();
    cd_block_init();

    vdp2_tvmd_display_res_set(VDP2_TVMD_INTERLACE_NONE, VDP2_TVMD_HORZ_NORMAL_B,
                              VDP2_TVMD_VERT_240);

    vdp2_scrn_back_color_set(VDP2_VRAM_ADDR(3, 0x01FFFE),
                             COLOR_RGB1555(0, 0, 0, 0));

    vdp2_sprite_priority_set(0, 6);
    vdp1_env_set(&env);
    vdp1_sync_interval_set(0);

    vdp_sync_vblank_out_set(_vblank_out_handler, NULL);

    cpu_intc_mask_set(0);
    vdp2_tvmd_display_set();
}
