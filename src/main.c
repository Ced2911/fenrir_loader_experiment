#include <yaul.h>
#include <stdio.h>
#include <stdlib.h>
#include "fenrir/fenrir.h"
#include "vdp1.config.h"
#include "vdp2.config.h"
#include "font/font.h"
#include "screens/screen.h"
#include "sound_driver/pcm.h"
#include "theme.h"

#define RESOLUTION_WIDTH (352)
#define RESOLUTION_HEIGHT (224)

sd_dir_t *sd_dir;
status_sector_t *status_sector;
sd_dir_entry_t *sd_dir_entries;

int16_t hadoken_snd;


void *zalloc(size_t l)
{
    void *ptr = malloc(l);
    memset(ptr, 0, l);
    return ptr;
}

static void theme_update()
{
    static fix16_t __cx2 = 0;
    static fix16_t __cy2 = 0;

    __cx2 += theme_get_ui_config()->screens.gamelist.foreground.velocity_x;
    __cy2 += theme_get_ui_config()->screens.gamelist.foreground.velocity_y;

    // animate bg
    if (theme_get_ui_config()->screens.gamelist.background.velocity_x)
        vdp2_scrn_scroll_x_update(VDP2_SCRN_NBG0, theme_get_ui_config()->screens.gamelist.background.velocity_x);
    if (theme_get_ui_config()->screens.gamelist.background.velocity_y)
        vdp2_scrn_scroll_y_update(VDP2_SCRN_NBG0, theme_get_ui_config()->screens.gamelist.background.velocity_y);

    vdp2_scrn_scroll_x_set(VDP2_SCRN_NBG2, __cx2);
    vdp2_scrn_scroll_y_set(VDP2_SCRN_NBG2, __cy2);
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

    screen_init();
    // screen_select(screen_error_no_sd);
    screen_t *screen;

    while (1)
    {
        screen_t *screen = get_screen();
        theme_update();
        screen->update();

        vdp1_sync_render();
        vdp1_sync();
        vdp2_sync();
        vdp1_sync_wait();
        vdp2_sync_wait();
    }
    screen->destroy();
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
