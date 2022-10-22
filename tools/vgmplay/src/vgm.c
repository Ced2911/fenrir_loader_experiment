#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vgm.h"
#include "qsound.h"
#define FRAME_SIZE (2352 / 4) // audio sector size / (16bit#stereo)
#define SAMPLING_RATE 44100

// create a mask that covers the range 0...v-1
uint32_t rom_mask(uint32_t v)
{
    if (v == 0)
        return 0;
    v--;
    v |= (v >> 1);
    v |= (v >> 2);
    v |= (v >> 4);
    v |= (v >> 8);
    v |= (v >> 16);
    return v;
}

static struct qsound_chip *chip;

void qsound_alloc_rom(struct qsound_chip *chip, uint32_t memsize)
{
    chip->rom_data = (uint8_t *)realloc(chip->rom_data, memsize);
    chip->rom_mask = rom_mask(memsize);
    memset(chip->rom_data, 0xFF, memsize);

    return;
}
static void qsound_write_rom(struct qsound_chip *chip, uint32_t offset, uint32_t len, uint8_t *d)
{
    memcpy(&chip->rom_data[offset], d, len);
    // for (int i =0;i<len;i++)
    // chip->rom_data[offset+i]=*d++;
}

short audio_write_sound_stereo(int sample32)
{
    short sample16;

    if (sample32 < -0x7FFF)
    {
        sample16 = -0x7FFF;
    }
    else if (sample32 > 0x7FFF)
    {
        sample16 = 0x7FFF;
    }
    else
    {
        sample16 = (short)(sample32);
    }

    // for I2S_MODE_DAC_BUILT_IN
    // sample16 = sample16 ^ 0x8000U;

    return sample16;
}

uint8_t get_vgm_ui8(vgm_player_t *vgm_player)
{
    return vgm_player->vgm[vgm_player->vgmpos++];
}

uint16_t get_vgm_ui16(vgm_player_t *vgm_player)
{
    return get_vgm_ui8(vgm_player) + (get_vgm_ui8(vgm_player) << 8);
}

uint32_t get_vgm_ui32(vgm_player_t *vgm_player)
{
    return get_vgm_ui8(vgm_player) + (get_vgm_ui8(vgm_player) << 8) + (get_vgm_ui8(vgm_player) << 16) + (get_vgm_ui8(vgm_player) << 24);
}

uint16_t parse_vgm(vgm_player_t *vgm_player)
{
    uint8_t command;
    uint16_t wait = 0;
    uint8_t reg;
    uint8_t dat;

    command = get_vgm_ui8(vgm_player);
    printf("cmd: %08x\n", command);
    switch (command)
    {
    case 0x50:
    case 0x52:
    case 0x53:
    case 0x61:
        wait = get_vgm_ui16(vgm_player);
        break;
    case 0x62:
        wait = 735;
        break;
    case 0x63:
        wait = 882;
        break;
    case 0x66:
        if (vgm_player->vgmloopoffset == 0)
        {
            vgm_player->vgmend = 1;
        }
        else
        {
            vgm_player->vgmpos = vgm_player->vgmloopoffset;
        }
        break;
    case 0x67:
        uint8_t o66 = get_vgm_ui8(vgm_player);         // 0x66
        uint8_t type = get_vgm_ui8(vgm_player) & 0xC0; // 0x00 data type
        uint32_t sz = get_vgm_ui32(vgm_player);
        uint32_t pos = vgm_player->vgmpos + sz;
        if (type == 0x80)
        {
            uint32_t rom_sz = get_vgm_ui32(vgm_player);
            uint32_t offset = get_vgm_ui32(vgm_player);
            qsound_alloc_rom(chip, rom_sz);
            qsound_write_rom(chip, offset, sz - 0x08, &vgm_player->vgm[vgm_player->vgmpos]);
        }
        else
        {
            printf("type: %08x\n", type);
        }
        // vgm_player->vgmpos += get_vgm_ui32(vgm_player); // size of data, in bytes
        // vgm_player->datpos = vgm_player->vgmpos + 4;
        vgm_player->vgmpos = pos;
        break;
    case 0x70:
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
    case 0x75:
    case 0x76:
    case 0x77:
    case 0x78:
    case 0x79:
    case 0x7a:
    case 0x7b:
    case 0x7c:
    case 0x7d:
    case 0x7e:
    case 0x7f:
        wait = (command & 0x0f) + 1;
        break;
    case 0x80:
    case 0x81:
    case 0x82:
    case 0x83:
    case 0x84:
    case 0x85:
    case 0x86:
    case 0x87:
    case 0x88:
    case 0x89:
    case 0x8a:
    case 0x8b:
    case 0x8c:
    case 0x8d:
    case 0x8e:
    case 0x8f:
        wait = (command & 0x0f);
        //   YM2612_Write(0, 0x2a);
        //   YM2612_Write(1, vgm_player->vgm[vgm_player->datpos + vgm_player->pcmpos + vgm_player->pcmoffset]);
        vgm_player->pcmoffset++;
        break;
    case 0xe0:
        vgm_player->pcmpos = get_vgm_ui32(vgm_player);
        vgm_player->pcmoffset = 0;
        break;
    case 0xc4:
        uint8_t data0 = get_vgm_ui8(vgm_player);
        uint8_t data1 = get_vgm_ui8(vgm_player);
        uint8_t offset = get_vgm_ui8(vgm_player);
        qsound_w(chip, 0, data0);
        qsound_w(chip, 1, data1);
        qsound_w(chip, 2, offset);
        break;
    default:
        printf("unknown cmd at 0x%x: 0x%x\n", vgm_player->vgmpos, vgm_player->vgm[vgm_player->vgmpos]);
        vgm_player->vgmpos++;
        break;
    }

    // printf("command: %x\nwait:%d\n", command, wait);

    return wait;
}

static void vgm_parse_header(vgm_player_t *vgm_player)
{
    // vgm_player->vgmpos = 0x40;
    vgm_player->vgmend = 0;

    // parse header
    // read vgm header
    vgm_player->vgmpos = 0x0C;
    vgm_player->clock_sn76489 = get_vgm_ui32(vgm_player);

    vgm_player->vgmpos = 0x2C;
    vgm_player->clock_ym2612 = get_vgm_ui32(vgm_player);

    vgm_player->vgmpos = 0x1c;
    vgm_player->vgmloopoffset = get_vgm_ui32(vgm_player);

    vgm_player->vgmpos = 0x34;
    vgm_player->vgmpos = 0x34 + get_vgm_ui32(vgm_player);

    if (vgm_player->clock_ym2612 == 0)
        vgm_player->clock_ym2612 = 7670453;
    if (vgm_player->clock_sn76489 == 0)
        vgm_player->clock_sn76489 = 3579545;
}

int vgm_init(vgm_player_t *vgm_player)
{
    vgm_parse_header(vgm_player);

    // init sound chip
    chip = (struct qsound_chip *)calloc(1, sizeof(struct qsound_chip));
    chip->rom_data = NULL;
    chip->rom_mask = 0x00;
    qsound_reset(chip);

    qsound_start(chip, 4000000*15);

    vgm_player->sampled = 0;
    vgm_player->frame_size = -1;
    vgm_player->sample_count = 0;

    return 0;
}

static int vgm_update_chips(vgm_player_t *vgm_player, int **samples, int max)
{
    int32_t space_available = FRAME_SIZE - vgm_player->sample_count;
    int32_t len = max > space_available ? space_available : max;
    uint8_t s = qsound_stream_update(chip, samples, len);
    //  YM2612_Update(samples, len);
    //  YM2612_DacAndTimers_Update(samples, len);

    return len;
}

int vgm_do_samples(vgm_player_t *vgm_player)
{
    int *working_samples[2] = {
        &vgm_player->samples[0][vgm_player->sample_count],
        &vgm_player->samples[1][vgm_player->sample_count]};

    // Do we have some samples left ?
    if (vgm_player->sample_count > 0)
    {
        int32_t to_sample = vgm_update_chips(vgm_player, working_samples, vgm_player->sample_count);

        working_samples[0] = working_samples[0] + to_sample;
        working_samples[1] = working_samples[1] + to_sample;

        vgm_player->sample_count -= to_sample;
    }

    // do we need to sample more ?
    while (vgm_player->sample_count < FRAME_SIZE)
    {
        // read cmd
        if (vgm_player->frame_size <= 0)
        {
            vgm_player->frame_size = parse_vgm(vgm_player);
        }
        vgm_player->sampled += vgm_player->frame_size;

        // after each command we need to update chips
        int32_t to_sample = vgm_update_chips(vgm_player, working_samples, vgm_player->frame_size);
        working_samples[0] = working_samples[0] + to_sample;
        working_samples[1] = working_samples[1] + to_sample;

        vgm_player->sample_count += to_sample;
        vgm_player->frame_size -= to_sample;
    }

    vgm_player->sample_count -= FRAME_SIZE;

    return 0;
}

int vgm_do_audio(vgm_player_t *vgm_player, uint32_t offset, uint8_t *data)
{
    vgm_do_samples(vgm_player);

    uint16_t *pcm = (uint16_t *)data;

    // convert audio
    for (uint32_t i = 0; i < FRAME_SIZE; i++)
    {
        *pcm++ = audio_write_sound_stereo(vgm_player->samples[0][i]);
        *pcm++ = audio_write_sound_stereo(vgm_player->samples[1][i]);
    }
    return 0;
}
