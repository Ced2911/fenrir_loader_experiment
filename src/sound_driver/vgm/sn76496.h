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
} sn76496_t;

static sn76496_t sn_chip_0;

typedef struct
{
    uint8_t oct;
    uint16_t fns;
} sn_scsp_map_t;

sn_scsp_map_t sn_scsp_map[] = {
#include "sn_map.h"
};

#define SN_NOTE (note_a4)
//#define SN_NOTE (note_c_d_4)

void sn76496_init()
{

    snd_init();
    // copy sinetable
    uint32_t addr = (0x2000);
    for (int i = 0; i < 5; i++)
    {

        memcpy((void *)(SCSP_RAM + addr + (i * sizeof(SN_NOTE))), SN_NOTE, sizeof(SN_NOTE));
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
        slots[i].raw[0] = (addr << 16) + 0x130;
        slots[i].raw[1] = addr & 0xffff;
        slots[i].raw[2] = 0;
        slots[i].raw[3] = sizeof(SN_NOTE);

        slots[i].raw[4] = 0x1F;
        slots[i].raw[5] = 0x1F;

        slots[i].raw[6] = 255;
        slots[i].raw[7] = 0;
        slots[i].raw[8] = 0;
        slots[i].raw[9] = 0;
        slots[i].raw[10] = 0;

        slots[i].raw[11] = 0xA000;
        slots[i].raw[12] = 0;
        slots[i].attack_rate = 31;
        slots[i].release_r = 31;
        slots[i].total_l = 7;

        // slots[i].dipan = 0;
        // slots[i].lpctl = 0;

        // slots[i].lpctl = 1;
        // slots[i].disdl = 7;
    }

    for (int i = 0; i < 4; i++)
    {
        slots[i].kyonb = 1;
        slots[i].kyonex = 1;
        slots[i].total_l = 0;
    }

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

    switch (r)
    {
    case 0: // tone 0: frequency
    case 2: // tone 1: frequency
    case 4: // tone 2: frequency
        if ((dd & 0x80) == 0)
            chip->registers[r] = (chip->registers[r] & 0x0f) | ((dd & 0x3f) << 4);

        chip->period[r] = chip->registers[r];
        // slots[chan].fns = 2 * chip->registers[r];
        slots[chan].fns = sn_scsp_map[chip->registers[r]].fns;
        slots[chan].oct = sn_scsp_map[chip->registers[r]].oct;

        if (r == 4)
        {
            // update noise shift frequency
            if ((chip->registers[6] & 0x03) == 0x03)
            {
                chip->period[3] = chip->period[2] << 1;

                slots[chan].fns = sn_scsp_map[chip->period[3]].fns;
                slots[chan].oct = sn_scsp_map[chip->period[3]].oct;
            }
        }

        //  dbgio_printf("set frequency: %04x %04x\n", chip->registers[r], sn_scsp_map[r].fns, sn_scsp_map[r].oct);
        // dbgio_printf("set frequency: %04x\n", chip->registers[r]);
        break;

    case 1: // tone 0: volume
    case 3: // tone 1: volume
    case 5: // tone 2: volume
    case 7: // noise: volume
        if ((dd & 0x80) == 0)
            chip->registers[r] = (chip->registers[r] & 0x3f0) | (dd & 0x0f);

        // dbgio_printf("set volume: %04x\n", dd & 0xf);
        slots[chan].total_l = (dd & 0xf) << 2;
        break;
    case 6: // noise: frequency, mode
        if ((dd & 0x80) == 0)
            chip->registers[r] = (chip->registers[r] & 0x3f0) | (dd & 0x0f);

        uint16_t n = chip->registers[6];
        uint16_t f = ((n & 3) == 3) ? (chip->period[2] << 1) : (1 << (5 + (n & 3)));
        slots[2].fns = sn_scsp_map[f].fns;
        slots[2].oct = sn_scsp_map[f].oct;
        break;

    default:
        break;
    }
}