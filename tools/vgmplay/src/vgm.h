#pragma once
#include <stdint.h>
struct vgm_player_s
{
    uint8_t *vgm;
    uint8_t vgmend;
    uint32_t vgmpos;
    uint32_t vgmloopoffset;
    uint32_t datpos;
    uint32_t pcmpos;
    uint32_t pcmoffset;

    uint32_t clock_sn76489;
    uint32_t clock_ym2612;

    int **samples;

    int sampled;
    int sample_count;
    int frame_size;
};
typedef struct vgm_player_s vgm_player_t;

int vgm_init(vgm_player_t *vgm_player);
int vgm_seek(vgm_player_t *vgm_player, uint32_t seek);
int vgm_do_audio(vgm_player_t *vgm_player, uint32_t offset, uint8_t *data);
int vgm_do_samples(vgm_player_t *vgm_player);
