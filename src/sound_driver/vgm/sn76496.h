#pragma once
#include <stdint.h>
#include <string.h>
#include "../scsp.h"
#include "notes.h"

static uint8_t sn74xx_regs[256];
typedef struct
{
    uint8_t last_register;
    uint16_t registers[8];
    uint16_t period[4];
    uint16_t vol[4];
} sn76496_t;

static sn76496_t sn_chip_0;

typedef struct
{
    uint8_t oct;
    uint16_t fns;
} sn_scsp_map_t;

#include "sn_map.h"

// #define SN_NOTE (note_c_d_4)

// The SN76489 attenuates the volume by 2dB for each step in the volume register
// 32767, 26028, 20675, 16422, 13045, 10362,  8231,  6568,
// 5193,  4125,  3277,  2603,  2067,  1642,  1304,     0
uint8_t sn_scsp_tl[] = {

};

#define SN_NOTE (note_a4_u16)
#define SN_PER_NOTE (sn_noise_per)
#define SN_NOISE_NOTE (sn_noise_per)

uint32_t sn_square_addr = (0x2000);
uint32_t sn_periodic_addr = (0x2400);
uint32_t sn_white_noise_addr = (0x2800);

uint8_t volume_map[] = {
    // 0db
    0,
    // 1.9db
    5,
    // 4.2db
    11,
    // 6db
    16,
    // 7.9db
    21,
    // 10.2db
    27,
    // 12db
    32,
    // 13.9db
    37,
    // 16.2db
    43,
    // 18db
    48,
    // 19.9db
    53,
    // 22.2db
    59,
    // 24db
    64,
    // 25.9db
    69,
    // 28.2db
    75,
    // 30db
    80,
    // 31.9db
    85};

void sn76496_init()
{
    snd_init();
    // copy sinetable
    for (int i = 0; i < 1; i++)
    {

        memcpy((void *)(SCSP_RAM + sn_square_addr + (i * sizeof(SN_NOTE))), SN_NOTE, sizeof(SN_NOTE));
        memcpy((void *)(SCSP_RAM + sn_periodic_addr + (i * sizeof(SN_PER_NOTE))), SN_PER_NOTE, sizeof(SN_PER_NOTE));
        memcpy((void *)(SCSP_RAM + sn_white_noise_addr + (i * sizeof(SN_NOISE_NOTE))), SN_NOISE_NOTE, sizeof(SN_NOISE_NOTE));
    }

    memset(sn74xx_regs, 0, 256);

    volatile uint16_t *control = (uint16_t *)0x25b00400;
    volatile uint16_t *intr = (uint16_t *)0x25b0042a;
    control[0] = 0xf; // master vol
    intr[0] = 0;      // irq

    // init slots...
    volatile scsp_slot_regs_t *slots = get_scsp_slot(0);
    // 8 chanl * 4 op
    for (int i = 0; i < 8 * 4; i++)
    {
        scsp_slot_regs_t slot = {};

        //
        slot.sa = sn_square_addr;
        slot.lsa = 0;
        slot.lea = sn_square_addr + sizeof(SN_NOTE) / 2;
        // uned 16 bit
        slot.pcm8b = 0;
        slot.sbctl = 1;

        slot.lpctl = 1;
        slot.attack_rate = 31;
        slot.release_r = 31;
        slot.loop_start = 1;
        slot.kr_scale = 1;
        slot.sdir = 0;
        slot.disdl = 0;

        // memcpy(&slots[i], 0, sizeof(scsp_slot_regs_t));
        for (int r = 0; r < 16; r++)
        {
            slots[i].raw[r] = slot.raw[r];
        }
    }

    for (int i = 0; i < 4; i++)
    {
        //    slots[i].total_l = 0;
        //    slots[i].disdl = 0;
    }

    // slots[3].alfows = 3; // noise
    // slots[3].plfows = 3; // noise
    // slots[3].ssctl = 1;  // select noise

    for (int i = 0; i < 3; i++)
    {
        slots[i].total_l = 7;
        slots[i].disdl = 7;

        slots[i].kyonb = 0;
        slots[i].kyonex = 0;
    }

    slots[0].kyonb = 1;
    slots[0].kyonex = 1;
    sn76496_t *chip = &sn_chip_0;
    chip->last_register = 0;
    for (int i = 0; i < 8; i += 2)
    {
        chip->registers[i] = 0;
        chip->registers[i + 1] = 0x0f; /* volume = 0 */
    }
}

void sn76496_w(uint8_t dd)
{
    volatile scsp_slot_regs_t *slots = get_scsp_slot(0);
    uint16_t r = 0;
    sn76496_t *chip = &sn_chip_0;

    emu_printf("sn76496_w %02x\n", dd);

    if (dd & 0x80)
    {
        // r = chan + type
        r = (dd & 0x70) >> 4;
        chip->last_register = r;
        chip->registers[r] = (chip->registers[r] & 0x3f0) | (dd & 0x0f);
    }
    else
    {
        r = chip->last_register;
    }

    uint8_t chan = r >> 1;
    uint16_t tone = 0;
    uint16_t vol = 0;

    switch (r)
    {
    case 0: // tone 0: frequency
    case 2: // tone 1: frequency
    case 4: // tone 2: frequency
        if ((dd & 0x80) == 0)
            chip->registers[r] = (chip->registers[r] & 0x0f) | ((dd & 0x3f) << 4);

        chip->period[chan] = chip->registers[r] & 0x3ff;
        break;

    case 1: // tone 0: volume
    case 3: // tone 1: volume
    case 5: // tone 2: volume
    case 7: // noise: volume
        if ((dd & 0x80) == 0)
            chip->registers[r] = (chip->registers[r] & 0x3f0) | (dd & 0x0f);

        chip->vol[chan] = chip->registers[r] & 0x3ff;
        break;
    case 6: // noise: frequency, mode
        break;

        if ((dd & 0x80) == 0)
            chip->registers[r] = (chip->registers[r] & 0x3f0) | (dd & 0x0f);

        uint16_t n = chip->registers[6];

        uint8_t tt = (n >> 2) & 3; // selects the mode (white (1) or "periodic" (0)).
        uint8_t rr = n & 3;        // select the shift rate

        tone = 0;

        slots[3].kyonb = 0;
        slots[3].kyonex = 0;
        if (tt == 1)
        {
            slots[3].sa = sn_white_noise_addr;
            slots[3].lea = sizeof(SN_PER_NOTE);
        }
        else
        {
            slots[3].sa = sn_periodic_addr;
            slots[3].lea = sizeof(SN_NOISE_NOTE);
        }
        slots[3].lpctl = 1;
        slots[3].pcm8b = 1;
        //    slots[3].kyonb = 1;
        //   slots[3].kyonex = 1;

        switch (rr)
        {
            // periodic
        case 0:
            slots[3].fns = sn_noise_scsp_map[0].fns;
            slots[3].oct = sn_noise_scsp_map[0].oct;
            break;
        case 1:
            slots[3].fns = sn_noise_scsp_map[1].fns;
            slots[3].oct = sn_noise_scsp_map[1].oct;
            break;
        case 2:
            slots[3].fns = sn_noise_scsp_map[2].fns;
            slots[3].oct = sn_noise_scsp_map[2].oct;
            break;
        case 3:
            tone = chip->period[2];
            slots[3].fns = sn_scsp_map[tone].fns;
            slots[3].oct = sn_scsp_map[tone].oct;
            break;
        }

        break;

    default:
        break;
    }

    if ((dd & 0x80) == 0)
    {
        if (chan < 3)
        {
            // slots[chan].fns = sn_scsp_map[chip->period[chan]].fns;
            // slots[chan].oct = sn_scsp_map[chip->period[chan]].oct;
            // slots[chan].oct = 5;
            slots[chan].total_l = 7;
            // slots[chan].disdl = 7;
            // slots[chan].attack_rate = 31;
            // slots[chan].release_r = 31;

            // slots[chan].sdir = 1;
            // slots[chan].kyonb = 0;
            // slots[chan].kyonex = 0;
            // slots[chan].kyonb = 1;
            // slots[chan].kyonex = 1;

            // slots[chan].dipan = 0;

            // slots[chan].total_l = 0;//volume_map[chip->vol[chan] & 0xf];
            // slots[chan].total_l = 0x1f; // chip->vol[chan];

            emu_printf("chip->period[%d] %04x\n", chan, chip->period[chan]);
            emu_printf("slots[%d].fns %04x\n", chan, sn_scsp_map[chip->period[chan]].fns);
            emu_printf("slots[%d].oct %04x\n", chan, sn_scsp_map[chip->period[chan]].oct);
        }
    }
}