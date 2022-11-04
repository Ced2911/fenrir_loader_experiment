#pragma once
#include <stdint.h>

typedef struct vgm_player_s
{
    uint8_t *vgm;
    uint8_t vgmend;
    uint32_t vgmpos;
    uint32_t vgmloopoffset;
    uint32_t datpos;
    uint32_t pcmpos;
    uint32_t pcmoffset;

    uint32_t clock_ym2203;

    int sampled;
    int sample_count;
    int frame_size;
} vgm_player_t;

void ym2203_init();

int vgm_init(vgm_player_t *vgm_player);
uint16_t vgm_parse(vgm_player_t *vgm_player);