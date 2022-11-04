#include <stdint.h>
#include <string.h>
#include <yaul.h>
#include "scsp.h"
#include "pcm.h"

#define PCM_MSK1(a) ((a)&0x0001)
#define PCM_MSK3(a) ((a)&0x0007)
#define PCM_MSK4(a) ((a)&0x000F)
#define PCM_MSK5(a) ((a)&0x001F)
#define PCM_MSK10(a) ((a)&0x03FF)

#define PCM_SCSP_FREQUENCY (44100L)

static const int logtbl[] = {
    /* 0 */ 0,
    /* 1 */ 1,
    /* 2 */ 2, 2,
    /* 4 */ 3, 3, 3, 3,
    /* 8 */ 4, 4, 4, 4, 4, 4, 4, 4,
    /* 16 */ 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    /* 32 */ 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    /* 64 */ 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    /* 128 */ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};

#define PCM_CALC_OCT(smpling_rate) \
    ((int)logtbl[PCM_SCSP_FREQUENCY / ((smpling_rate) + 1)])

#define PCM_CALC_SHIFT_FREQ(oct) \
    (PCM_SCSP_FREQUENCY >> (oct))

#define PCM_CALC_FNS(smpling_rate, shift_freq) \
    ((((smpling_rate) - (shift_freq)) << 10) / (shift_freq))

#define PCM_SET_PITCH_WORD(oct, fns) \
    ((int)((PCM_MSK4(-(oct)) << 11) | PCM_MSK10(fns)))

static short convert_bitrate_to_pitchword(short sampleRate)
{

    int octr;
    int shiftr;
    int fnsr;

    octr = PCM_CALC_OCT(sampleRate);
    shiftr = PCM_CALC_SHIFT_FREQ(octr);
    fnsr = PCM_CALC_FNS(sampleRate, shiftr);

    return PCM_SET_PITCH_WORD(octr, fnsr);
}

void pcm_load_sample(pcm_sample_t *s, uint8_t *data, size_t sz)
{
    volatile uint16_t *control = (uint16_t *)0x25b00400;
    control[0] = 7; // master vol

    /*
        // setp dma to sound memory
        cpu_dmac_cfg_t cfg = {
            .channel = 0,
            .src_mode = CPU_DMAC_SOURCE_INCREMENT,
            .dst = SCSP_RAM + s->addr,
            .dst_mode = CPU_DMAC_DESTINATION_INCREMENT,
            .src = data,
            .len = sz,
            .stride = CPU_DMAC_STRIDE_1_BYTE,
            .bus_mode = CPU_DMAC_BUS_MODE_BURST,
            .ihr = NULL,
            .ihr_work = NULL};

        cpu_dmac_channel_wait(0);
        cpu_dmac_channel_config_set(&cfg);
        cpu_dmac_channel_start(0);
        */
    memcpy((void *)(SCSP_RAM + s->addr), data, sz);

    // fill slot
    scsp_slot_regs_t *slot = get_scsp_slot(s->slot);

    // setup sample
    slot->pcm8b = s->bit & 1;
    slot->sa = s->addr;
    slot->lsa = 0;
    slot->lea = (sz / s->bit) - 1;

    // why 31 ?
    slot->attack_rate = 31;
    slot->release_r = 31;

    // slot->sdir = 1;
    // volume & pan
    slot->disdl = 7;
    slot->dipan = 0;

    // wait dma copy
    // cpu_dmac_channel_wait(0);
}

void pcm_sample_set_samplerate(pcm_sample_t *s, uint32_t samplerate)
{
    scsp_slot_regs_t *slot = get_scsp_slot(s->slot);
    int octr;
    int shiftr;
    int fnsr;

    octr = PCM_CALC_OCT(samplerate);
    shiftr = PCM_CALC_SHIFT_FREQ(octr);
    fnsr = PCM_CALC_FNS(samplerate, shiftr);

    // frequency
    slot->oct = (-octr) & 0xf;
    slot->fns = fnsr;
}

void pcm_sample_start(pcm_sample_t *s)
{
    scsp_slot_regs_t *slot = get_scsp_slot(s->slot);
    // start playback
    slot->kyonb = 1;
    slot->kyonex = 1;
}

void pcm_sample_stop(pcm_sample_t *s)
{
    scsp_slot_regs_t *slot = get_scsp_slot(s->slot);
    // stop playback
    slot->kyonb = 0;
    slot->kyonex = 0;
}

void pcm_sample_set_loop(pcm_sample_t *s, pcm_sample_loop_t loop)
{
    scsp_slot_regs_t *slot = get_scsp_slot(s->slot);
    slot->lpctl = 1;
}
