#pragma once
#include <stdint.h>
#include <string.h>
#include "../scsp.h"

typedef struct scsp_kc
{
    uint16_t fnc;
    uint16_t oct;
    char dbg[16];
} scsp_kc_t;
const scsp_kc_t scsp_kc_map[] = {
    /* C0 */ {0, 0xc, "C0"},
    /* C#0 */ {61, 0xc, "C#0"},
    /* D0 */ {125, 0xc, "D0"},
    /* D0 */ {125, 0xc, "D0"},
    /* D#0 */ {194, 0xc, "D#0"},
    /* E0 */ {266, 0xc, "E0"},
    /* F0 */ {343, 0xc, "F0"},
    /* F0 */ {343, 0xc, "F0"},
    /* F#0 */ {424, 0xc, "F#0"},
    /* G0 */ {510, 0xc, "G0"},
    /* G#0 */ {601, 0xc, "G#0"},
    /* G#0 */ {601, 0xc, "G#0"},
    /* A0 */ {698, 0xc, "A0"},
    /* A#0 */ {801, 0xc, "A#0"},
    /* B0 */ {909, 0xc, "B0"},
    /* B0 */ {909, 0xc, "B0"},
    /* C1 */ {0, 0xd, "C1"},
    /* C#1 */ {61, 0xd, "C#1"},
    /* D1 */ {125, 0xd, "D1"},
    /* D1 */ {125, 0xd, "D1"},
    /* D#1 */ {194, 0xd, "D#1"},
    /* E1 */ {266, 0xd, "E1"},
    /* F1 */ {343, 0xd, "F1"},
    /* F1 */ {343, 0xd, "F1"},
    /* F#1 */ {424, 0xd, "F#1"},
    /* G1 */ {510, 0xd, "G1"},
    /* G#1 */ {601, 0xd, "G#1"},
    /* G#1 */ {601, 0xd, "G#1"},
    /* A1 */ {698, 0xd, "A1"},
    /* A#1 */ {801, 0xd, "A#1"},
    /* B1 */ {909, 0xd, "B1"},
    /* B1 */ {909, 0xd, "B1"},
    /* C2 */ {0, 0xe, "C2"},
    /* C#2 */ {61, 0xe, "C#2"},
    /* D2 */ {125, 0xe, "D2"},
    /* D2 */ {125, 0xe, "D2"},
    /* D#2 */ {194, 0xe, "D#2"},
    /* E2 */ {266, 0xe, "E2"},
    /* F2 */ {343, 0xe, "F2"},
    /* F2 */ {343, 0xe, "F2"},
    /* F#2 */ {424, 0xe, "F#2"},
    /* G2 */ {510, 0xe, "G2"},
    /* G#2 */ {601, 0xe, "G#2"},
    /* G#2 */ {601, 0xe, "G#2"},
    /* A2 */ {698, 0xe, "A2"},
    /* A#2 */ {801, 0xe, "A#2"},
    /* B2 */ {909, 0xe, "B2"},
    /* B2 */ {909, 0xe, "B2"},
    /* C3 */ {0, 0xf, "C3"},
    /* C#3 */ {61, 0xf, "C#3"},
    /* D3 */ {125, 0xf, "D3"},
    /* D3 */ {125, 0xf, "D3"},
    /* D#3 */ {194, 0xf, "D#3"},
    /* E3 */ {266, 0xf, "E3"},
    /* F3 */ {343, 0xf, "F3"},
    /* F3 */ {343, 0xf, "F3"},
    /* F#3 */ {424, 0xf, "F#3"},
    /* G3 */ {510, 0xf, "G3"},
    /* G#3 */ {601, 0xf, "G#3"},
    /* G#3 */ {601, 0xf, "G#3"},
    /* A3 */ {698, 0xf, "A3"},
    /* A#3 */ {801, 0xf, "A#3"},
    /* B3 */ {909, 0xf, "B3"},
    /* B3 */ {909, 0xf, "B3"},
    /* C4 */ {0, 0x0, "C4"},
    /* C#4 */ {61, 0x0, "C#4"},
    /* D4 */ {125, 0x0, "D4"},
    /* D4 */ {125, 0x0, "D4"},
    /* D#4 */ {194, 0x0, "D#4"},
    /* E4 */ {266, 0x0, "E4"},
    /* F4 */ {343, 0x0, "F4"},
    /* F4 */ {343, 0x0, "F4"},
    /* F#4 */ {424, 0x0, "F#4"},
    /* G4 */ {510, 0x0, "G4"},
    /* G#4 */ {601, 0x0, "G#4"},
    /* G#4 */ {601, 0x0, "G#4"},
    /* A4 */ {698, 0x0, "A4"},
    /* A#4 */ {801, 0x0, "A#4"},
    /* B4 */ {909, 0x0, "B4"},
    /* B4 */ {909, 0x0, "B4"},
    /* C5 */ {0, 0x1, "C5"},
    /* C#5 */ {61, 0x1, "C#5"},
    /* D5 */ {125, 0x1, "D5"},
    /* D5 */ {125, 0x1, "D5"},
    /* D#5 */ {194, 0x1, "D#5"},
    /* E5 */ {266, 0x1, "E5"},
    /* F5 */ {343, 0x1, "F5"},
    /* F5 */ {343, 0x1, "F5"},
    /* F#5 */ {424, 0x1, "F#5"},
    /* G5 */ {510, 0x1, "G5"},
    /* G#5 */ {601, 0x1, "G#5"},
    /* G#5 */ {601, 0x1, "G#5"},
    /* A5 */ {698, 0x1, "A5"},
    /* A#5 */ {801, 0x1, "A#5"},
    /* B5 */ {909, 0x1, "B5"},
    /* B5 */ {909, 0x1, "B5"},
    /* C6 */ {0, 0x2, "C6"},
    /* C#6 */ {61, 0x2, "C#6"},
    /* D6 */ {125, 0x2, "D6"},
    /* D6 */ {125, 0x2, "D6"},
    /* D#6 */ {194, 0x2, "D#6"},
    /* E6 */ {266, 0x2, "E6"},
    /* F6 */ {343, 0x2, "F6"},
    /* F6 */ {343, 0x2, "F6"},
    /* F#6 */ {424, 0x2, "F#6"},
    /* G6 */ {510, 0x2, "G6"},
    /* G#6 */ {601, 0x2, "G#6"},
    /* G#6 */ {601, 0x2, "G#6"},
    /* A6 */ {698, 0x2, "A6"},
    /* A#6 */ {801, 0x2, "A#6"},
    /* B6 */ {909, 0x2, "B6"},
    /* B6 */ {909, 0x2, "B6"},
    /* C7 */ {0, 0x3, "C7"},
    /* C#7 */ {61, 0x3, "C#7"},
    /* D7 */ {125, 0x3, "D7"},
    /* D7 */ {125, 0x3, "D7"},
    /* D#7 */ {194, 0x3, "D#7"},
    /* E7 */ {266, 0x3, "E7"},
    /* F7 */ {343, 0x3, "F7"},
    /* F7 */ {343, 0x3, "F7"},
    /* F#7 */ {424, 0x3, "F#7"},
    /* G7 */ {510, 0x3, "G7"},
    /* G#7 */ {601, 0x3, "G#7"},
    /* G#7 */ {601, 0x3, "G#7"},
    /* A7 */ {698, 0x3, "A7"},
    /* A#7 */ {801, 0x3, "A#7"},
    /* B7 */ {909, 0x3, "B7"},
    /* B7 */ {909, 0x3, "B7"}};

static uint8_t ym2151_regs[256];

unsigned char ym2151_pcm[] = {
    0x00, 0x04, 0x09, 0x0E, 0x12, 0x17, 0x1C, 0x20, 0x25, 0x29, 0x2E, 0x32,
    0x36, 0x3B, 0x3F, 0x43, 0x47, 0x4B, 0x4E, 0x52, 0x56, 0x59, 0x5C, 0x60,
    0x63, 0x65, 0x68, 0x6B, 0x6D, 0x70, 0x72, 0x74, 0x76, 0x77, 0x79, 0x7A,
    0x7B, 0x7C, 0x7D, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7D, 0x7C,
    0x7B, 0x7A, 0x79, 0x78, 0x76, 0x74, 0x72, 0x70, 0x6E, 0x6C, 0x69, 0x66,
    0x63, 0x60, 0x5D, 0x5A, 0x57, 0x53, 0x4F, 0x4C, 0x48, 0x44, 0x40, 0x3C,
    0x38, 0x33, 0x2F, 0x2B, 0x26, 0x22, 0x1D, 0x18, 0x14, 0x0F, 0x0A, 0x06,
    0x01, 0xFD, 0xF8, 0xF4, 0xEF, 0xEA, 0xE6, 0xE1, 0xDC, 0xD8, 0xD3, 0xCF,
    0xCB, 0xC7, 0xC2, 0xBE, 0xBA, 0xB6, 0xB3, 0xAF, 0xAB, 0xA8, 0xA5, 0xA1,
    0x9E, 0x9B, 0x99, 0x96, 0x93, 0x91, 0x8F, 0x8D, 0x8B, 0x89, 0x88, 0x86,
    0x85, 0x84, 0x83, 0x83, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x83, 0x83,
    0x84, 0x85, 0x87, 0x88, 0x8A, 0x8B, 0x8D, 0x8F, 0x91, 0x94, 0x96, 0x99,
    0x9C, 0x9F, 0xA2, 0xA5, 0xA8, 0xAC, 0xAF, 0xB3, 0xB7, 0xBB, 0xBF, 0xC3,
    0xC7, 0xCB, 0xD0, 0xD4, 0xD9, 0xDD, 0xE2, 0xE6, 0xEB, 0xF0, 0xF4, 0xF9,
    0xFE, 0x02, 0x06, 0x0B, 0x10, 0x14, 0x19, 0x1E, 0x22, 0x27, 0x2B, 0x30,
    0x34, 0x38, 0x3D, 0x41, 0x45, 0x49, 0x4C, 0x50, 0x54, 0x57, 0x5B, 0x5E,
    0x61, 0x64, 0x67, 0x69, 0x6C, 0x6E, 0x71, 0x73, 0x75, 0x76, 0x78, 0x79,
    0x7B, 0x7C, 0x7D, 0x7D, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7D,
    0x7C, 0x7B, 0x7A, 0x78, 0x77, 0x75, 0x73, 0x71, 0x6F, 0x6D, 0x6A, 0x68,
    0x65, 0x62, 0x5F, 0x5C, 0x59, 0x55, 0x52, 0x4E, 0x4A, 0x46, 0x42, 0x3E,
    0x3A, 0x36, 0x31, 0x2D, 0x29, 0x24, 0x20, 0x1B, 0x16, 0x12, 0x0D, 0x08,
    0x03, 0x00, 0xFB, 0xF6, 0xF2, 0xED, 0xE8, 0xE4, 0xDF, 0xDA, 0xD6, 0xD2,
    0xCD, 0xC9, 0xC5, 0xC1, 0xBC, 0xB9, 0xB5, 0xB1, 0xAD, 0xAA, 0xA6, 0xA3,
    0xA0, 0x9D, 0x9A, 0x97, 0x95, 0x92, 0x90, 0x8E, 0x8C, 0x8A, 0x89, 0x87,
    0x86, 0x85, 0x84, 0x83, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x83,
    0x84, 0x85, 0x86, 0x87, 0x89, 0x8A, 0x8C, 0x8E, 0x90, 0x92, 0x95, 0x97,
    0x9A, 0x9D, 0xA0, 0xA3, 0xA7, 0xAA, 0xAD, 0xB1, 0xB5, 0xB9, 0xBD, 0xC1,
    0xC5, 0xC9, 0xCD, 0xD2, 0xD6, 0xDB, 0xDF, 0xE4, 0xE8, 0xED, 0xF2, 0xF6,
    0xFB};

uint16_t fbtable[] = {
    0, 0, 0x5000, 0x6000, 0x6000, 0x7000, 0x7000, 0x8000};

uint16_t mixtable[] = {
    0, 0, 0, 0xA000,
    0, 0, 0, 0xA000,
    0, 0, 0, 0xA000,
    0, 0, 0, 0xA000,
    0, 0, 0xA000, 0xA000,
    0, 0xA000, 0xA000, 0xA000,
    0, 0xA000, 0xA000, 0xA000,
    0xA000, 0xA000, 0xA000, 0xA000};

uint16_t algotable[] = {
    0x800, 0x9861, 0x979E, 0x979E,
    0x800, 0x985F, 0, 0x979E,
    0x800, 0x9861, 0, 0x979D,
    0x800, 0, 0x979E, 0x979F,
    0x800, 0, 0x979E, 0x979E,
    0x800, 0x97DF, 0x979E, 0x975D,
    0x800, 0, 0x979E, 0,
    0x800, 0, 0, 0};

uint8_t pantable[] = {
    0x60, 0x18, 0x08, 0};

void ym2151_init()
{
    snd_init();
    // copy sinetable
    uint32_t addr = (0x2000);
    for (int i = 0; i < 5; i++)
    {

        memcpy((void *)(SCSP_RAM + addr + (i * sizeof(ym2151_pcm))), ym2151_pcm, sizeof(ym2151_pcm));
    }

    memset(ym2151_regs, 0, 256);

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
        slots[i].raw[3] = sizeof(ym2151_pcm);

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
        // slots[i].disdl = 7;
        // slots[i].dipan = 0;
        // slots[i].lpctl = 0;

        // slots[i].lpctl = 1;
        // slots[i].disdl = 7;
        slots[i].kyonb = 0;
        slots[i].kyonex = 0;
    }
}

static void set_freq(uint32_t kc, uint32_t kf)
{
}

void ym2151_w(uint8_t aa, uint8_t dd)
{
    uint8_t op_slot = ((aa >> 3) & 3) + ((aa & 7) * 4);
    volatile scsp_slot_regs_t *slot = get_scsp_slot(op_slot);

    ym2151_regs[aa] = dd;

    switch (aa)
    {

    case 0x08: // key off/on
    {
        // chan
        uint8_t chan = (dd & 7) * 4;
        uint8_t slot_n = dd >> 3 & 0xf;
        volatile scsp_slot_regs_t *slots = get_scsp_slot(chan);
#if 0
        slots[0].kyonb = (slot_n & 0x40) ? 1 : 0;
        slots[1].kyonb = (slot_n & 0x10) ? 1 : 0;
        slots[2].kyonb = (slot_n & 0x10) ? 1 : 0;
        slots[3].kyonb = (slot_n & 0x08) ? 1 : 0;
#endif
#if 0
        slots[0].kyonb = (slot_n >> 6) & 1;
        slots[1].kyonb = (slot_n >> 4) & 1;
        slots[2].kyonb = (slot_n >> 5) & 1;
        slots[3].kyonb = (slot_n >> 3) & 1;
#endif
#if 0
        slots[0].kyonex = (slot_n >> 6) & 1;
        slots[1].kyonex = (slot_n >> 4) & 1;
        slots[2].kyonex = (slot_n >> 5) & 1;
        slots[3].kyonex = (slot_n >> 3) & 1;
#endif
        // damagex way
        slots[0].b[0] = (dd & 0x40) >> 3;
        slots[1].b[0] = (dd & 0x10) >> 1;
        slots[2].b[0] = (dd & 0x20) >> 2;
        slots[3].b[0] = (dd & 0x08) | 0x10;
        // slots[3].kyonex = 1;
        break;
    }
    case 0x0f: /* noise mode enable, noise period */
        uint8_t noise_en = dd >> 7;
        uint8_t noise_freq = dd & 0x1f;
        break;
    case 0x18: /* LFO frequency */
               // uint8_t lfo_overflow = (1 << ((15 - (dd >> 4)) + 3)) * (1 << LFO_SH);
        uint8_t lfo_counter_add = 0x10 + (dd & 0x0f);

        break;

    case 0x19: /* PMD (bit 7==1) or AMD (bit 7==0) */
               /*
                   if (dd & 0x80)
                       uint8_t pmd = dd & 0x7f;
                   else
                       uint8_t amd = dd & 0x7f;
                       */
        break;

    case 0x1b: /* CT2, CT1, LFO waveform */
        uint8_t ct = dd >> 6;
        uint8_t lfo_wsel = dd & 3;
        break;
    case 0x20 ... 0x27: /* RL enable, Feedback, Connection */
    {

        uint8_t pan = dd >> 6;
        uint8_t connect = dd & 7;
        uint8_t fb = (dd >> 3) & 7;

        // break;
        uint8_t chan = (aa & 7);
        volatile scsp_slot_regs_t *chan_slots = get_scsp_slot(chan * 4);

#if 0 
//damagex way

        // uint32_t w = ym2151_regs[0x20 + chan];
        //  uint32_t x = w & 7 << 3 + mixtable;
        //  uint32_t y = x + 0x40;

        uint8_t w = ym2151_regs[0x20 + chan];

        uint16_t *x = &mixtable[(w & 7) << 2];
        uint16_t *y = &algotable[(w & 7) << 2];
        uint32_t z = fbtable[(w & 0x38) >> 3];
        uint16_t xx = pantable[w >> 6] << 8;

        chan_slots[0].raw[0xe >> 1] = *y++ + z;
        chan_slots[1].raw[0xe >> 1] = *y++ + 0;
        chan_slots[2].raw[0xe >> 1] = *y++ + 0;
        chan_slots[3].raw[0xe >> 1] = *y++ + 0;

        chan_slots[0].raw[0x16 >> 1] = *x++ + xx;
        chan_slots[1].raw[0x16 >> 1] = *x++ + xx;
        chan_slots[2].raw[0x16 >> 1] = *x++ + xx;
        chan_slots[3].raw[0x16 >> 1] = *x++ + xx;
#else

        chan_slots[0].raw[0xe >> 1] = algotable[connect * 4 + 0] + fbtable[fb];
        chan_slots[1].raw[0xe >> 1] = algotable[connect * 4 + 1] + 0;
        chan_slots[2].raw[0xe >> 1] = algotable[connect * 4 + 2] + 0;
        chan_slots[3].raw[0xe >> 1] = algotable[connect * 4 + 3] + 0;

        chan_slots[0].raw[0x16 >> 1] = mixtable[connect * 4 + 0] + (pantable[pan] << 8);
        chan_slots[1].raw[0x16 >> 1] = mixtable[connect * 4 + 1] + (pantable[pan] << 8);
        chan_slots[2].raw[0x16 >> 1] = mixtable[connect * 4 + 2] + (pantable[pan] << 8);
        chan_slots[3].raw[0x16 >> 1] = mixtable[connect * 4 + 3] + (pantable[pan] << 8);
#endif

        break;
    }
    case 0x028 ... 0x2f: /* Key Code */
    {

        uint8_t chan = (aa & 7) * 4;
        uint8_t keycode = dd & 0x7f;

        volatile scsp_slot_regs_t *chan_slots = get_scsp_slot(chan);

        chan_slots[0].fns = scsp_kc_map[keycode].fnc;
        chan_slots[1].fns = scsp_kc_map[keycode].fnc;
        chan_slots[2].fns = scsp_kc_map[keycode].fnc;
        chan_slots[3].fns = scsp_kc_map[keycode].fnc;

        chan_slots[0].oct = scsp_kc_map[keycode].oct;
        chan_slots[1].oct = scsp_kc_map[keycode].oct;
        chan_slots[2].oct = scsp_kc_map[keycode].oct;
        chan_slots[3].oct = scsp_kc_map[keycode].oct;

        dbgio_printf("kc: 0x%4x => %s\n", keycode, scsp_kc_map[keycode].dbg);
        break;
    }
    case 0x30 ... 0x37: /* Key Fraction */
                        // 100 = ((2puissance 6)-1) * 1.6)

        dbgio_printf("kf: 0x%4x => %04x\n", dd, dd >> 2);
        break;
    case 0x38 ... 0x3f: /* PMS, AMS */
        uint8_t pms = (dd >> 4) & 7;
        uint8_t ams = (dd & 3);
        break;

    case 0x40: /* DT1, MUL */
        uint8_t dt1 = (dd >> 4) & 0x07;
        uint8_t mul = dd & 0x0f;
        break;
    case 0x60: /* TL */
        uint8_t tl = dd & 0x7f;

        slot->total_l = tl << 1;
        break;
    case 0x80: /* KS, AR */
        uint8_t ks = dd >> 6;
        uint8_t ar = dd & 0x1f;

        slot->attack_rate = ar;
        slot->kr_scale = ks;
        break;
    case 0xa0: /* LFO AM enable, D1R */
        uint8_t am = dd >> 7;
        uint8_t d1r = dd & 0x1f;

        slot->d1r = d1r;
        break;
    case 0xc0: /* DT2, D2R */
        uint8_t dt2 = dd >> 7;
        uint8_t d2r = dd & 0x1f;

        slot->d2r = d2r;
        break;
    case 0xe0: /* D1L, RR */
        uint8_t d1l = dd >> 4;
        uint8_t rr = dd & 0x0f;

        slot->release_r = rr;
        slot->decay_l = d1l;
        break;

    default:
        break;
    }
}