#include <yaul.h>
#include <stdio.h>
#include <stdlib.h>
#include "fenrir/fenrir.h"
#include "vdp1.config.h"
#include "vdp2.config.h"
#include "font/font.h"
#include "screens/gamelist.h"
#include "sound_driver/pcmsys.h"
#include "sound_driver/sdrv.h"
#include "../assets/hav_flag.h"
#include "../assets/hado.h"

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

    load_drv(sdrv_bin, sdrv_bin_len, 11520);
    hadoken_snd = load_16bit_pcm(hado_pcm, hado_pcm_len, 11520);
    //pcm8snd = load_8bit_pcm(hav_flag_PCM, hav_flag_PCM_len, 11520);
    
int i = 0;
    while (1)
    {
        screen->update();

        if (i > 200) {
            pcm_play(hadoken_snd,PCM_PROTECTED, 6);
            i = 0;
        }

        i++;

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
static void _vblank_in_handler(void *work __unused)
{
    sdrv_vblank_rq();
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


// oops
void *memmove(void *dest, const void *src, size_t n)
{
	uint8_t* from = (uint8_t*) src;
	uint8_t* to = (uint8_t*) dest;

	if (from == to || n == 0)
		return dest;
	if (to > from && to-from < (int)n) {
		/* to overlaps with from */
		/*  <from......>         */
		/*         <to........>  */
		/* copy in reverse, to avoid overwriting from */
		int i;
		for(i=n-1; i>=0; i--)
			to[i] = from[i];
		return dest;
	}
	if (from > to && from-to < (int)n) {
		/* to overlaps with from */
		/*        <from......>   */
		/*  <to........>         */
		/* copy forwards, to avoid overwriting from */
		size_t i;
		for(i=0; i<n; i++)
			to[i] = from[i];
		return dest;
	}
	memcpy(dest, src, n);
	return dest;
}