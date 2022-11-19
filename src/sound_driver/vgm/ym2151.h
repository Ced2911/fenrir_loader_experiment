#pragma once
#include <stdint.h>
#include <string.h>
#include "../scsp.h"
#include "notes.h"

typedef struct scsp_kc
{
    uint16_t fns;
    uint16_t oct;
    char dbg[16];
} scsp_kc_t;

#include "sound_table.h"

static uint8_t ym2151_regs[256];

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
/*
uint16_t algotable[] = {
    0x800, 0x9861, 0x979E, 0x979E,
    0x800, 0x985F, 0, 0x979E,
    0x800, 0x9861, 0, 0x979D,
    0x800, 0, 0x979E, 0x979F,
    0x800, 0, 0x979E, 0x979E,
    0x800, 0x97DF, 0x979E, 0x975D,
    0x800, 0, 0x979E, 0,
    0x800, 0, 0, 0};
*/
// 8algo
/**
algo ym2151
    see:    https://user-images.githubusercontent.com/1408749/45317517-3f3b0b00-b508-11e8-81b3-c44d60247177.png       
            https://github.com/vgmrips/vgmplay/blob/master/VGMPlay/chips/ym2151.c

    
	MEM is simply one sample delay

    0:  M1---C1---MEM---M2---C2---OUT 

    1:  M1------+-MEM---M2---C2---OUT
             C1-+

    2:  M1------+-MEM---M2---C2---OUT
             C1-+
                            
    3:  M1---C1---MEM------+-C2---OUT 
		                M2-+          

    4:  M1---C1-+-OUT 
	    M2---C2-+     
		MEM: not used
    
    5:          +----C1----+
             M1-+-MEM---M2-+-OUT
                +----C2----+
    
    6:  M1---C1-+
             M2-+-OUT
             C2-+

    7:  M1-+
        C1-+-OUT
        M2-+
        C2-+
        MEM: not used

// https://antime.kapsi.fi/sega/files/ST-077-R2-052594.pdf
M - C = J
*/
    uint16_t algotable[] = {
    0x800, 0x9861, 0x979E, 0x979E,
    0x800, 0x985F, 0, 0x979E,
    0x800, 0x9861, 0, 0x979D,
    0x800, 0, 0x979E, 0x979F,
    0x800, 0, 0x979E, 0x979E,
    0x800, 0x97DF, 0x979E, 0x975D,
    0x800, 0, 0x979E, 0,
    0x800, 0, 0, 0};

/*
uint16_t pantable[] = {
    0x60, 0x18, 0x08, 0};
*/

uint16_t pantable[] = {
    0x70, 0x1e, 0x0e, 0};

void ym2151_init()
{
    snd_init();
    // copy sinetable
    uint32_t addr = (0x2000);
    for (int i = 0; i < 5; i++)
    {

        memcpy((void *)(SCSP_RAM + addr + (i * sizeof(note_c_d_4))), note_c_d_4, sizeof(note_c_d_4));
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
        slots[i].raw[3] = sizeof(note_c_d_4);

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
        
        slots[0].kyonb = (dd & 0x40) >> 6;  // c2
        slots[1].kyonb = (dd & 0x10) >> 4;  // c1
        slots[2].kyonb = (dd & 0x20) >> 5;  // m2
        slots[3].kyonb = (dd & 0x08) >> 3;  // m1
        slots[3].kyonex = (dd & 0x08) >> 3; // 
                                            // dbgio_printf("ko/foff %d %d %02x\n", chan, slot_n, dd);

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

#if 1
        // damagex way

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
#if 0
#if 1
        chan_slots[0].raw[0xe >> 1] = algotable[connect * 4 + 0] + fbtable[fb];
        chan_slots[1].raw[0xe >> 1] = algotable[connect * 4 + 1] + 0;
        chan_slots[2].raw[0xe >> 1] = algotable[connect * 4 + 2] + 0;
        chan_slots[3].raw[0xe >> 1] = algotable[connect * 4 + 3] + 0;
#else
        chan_slots[0].chan_slots[1].chan_slots[2].chan_slots[3].chan_slots[0].modulation_l = fb;
        chan_slots[1].modulation_l = fb;
        chan_slots[2].modulation_l = fb;
        chan_slots[3].modulation_l = fb;
#endif
#if 0
        chan_slots[0].raw[0x16 >> 1] = mixtable[connect * 4 + 0] + (pantable[pan] << 8);
        chan_slots[1].raw[0x16 >> 1] = mixtable[connect * 4 + 1] + (pantable[pan] << 8);
        chan_slots[2].raw[0x16 >> 1] = mixtable[connect * 4 + 2] + (pantable[pan] << 8);
        chan_slots[3].raw[0x16 >> 1] = mixtable[connect * 4 + 3] + (pantable[pan] << 8);
#endif
#endif
#endif

        break;
    }
    case 0x028 ... 0x2f: /* Key Code */
    {

        uint8_t chan = (aa & 7) * 4;
        uint8_t keycode = dd & 0x7f;

        volatile scsp_slot_regs_t *chan_slots = get_scsp_slot(chan);

        uint16_t fns = scsp_kc_map[keycode].fns;
        uint8_t oct = scsp_kc_map[keycode].oct;

        uint8_t mul0 = ym2151_regs[0x40 + 0 + chan * 4];
        uint8_t mul1 = ym2151_regs[0x40 + 1 + chan * 4];
        uint8_t mul2 = ym2151_regs[0x40 + 2 + chan * 4];
        uint8_t mul3 = ym2151_regs[0x40 + 3 + chan * 4];

        chan_slots[0].fns = fns;
        chan_slots[1].fns = fns;
        chan_slots[2].fns = fns;
        chan_slots[3].fns = fns;

        chan_slots[0].oct = oct; // + mul0;
        chan_slots[1].oct = oct; // + mul1;
        chan_slots[2].oct = oct; // + mul2;
        chan_slots[3].oct = oct; // + mul3;

        // dbgio_printf("kc: 0x%4x => %s\n", keycode, scsp_kc_map[keycode].dbg);
        break;
    }
#if 1
    case 0x30 ... 0x37: /* Key Fraction */
                        // 100 = ((2puissance 6)-1) * 1.6)

        // dbgio_printf("kf: 0x%4x => %04x\n", dd, dd >> 2);
        break;
    case 0x38 ... 0x3f: /* PMS, AMS */
        uint8_t pms = (dd >> 4) & 7;
        uint8_t ams = (dd & 3);
        break;

    case 0x40 ... 0x5F: /* DT1, MUL */
        uint8_t dt1 = (dd >> 4) & 0x07;
        uint8_t mul = dd & 0x0f;
        break;
    case 0x60 ... 0x7F: /* TL */
        uint8_t tl = dd & 0x7f;

        slot->total_l = tl << 1;
        break;
    case 0x80 ... 0x9F: /* KS, AR */
        uint8_t ks = dd >> 6;
        uint8_t ar = dd & 0x1f;

        slot->attack_rate = ar;
        slot->kr_scale = ks << 2;
        break;
    case 0xa0 ... 0xbF: /* LFO AM enable, D1R */
        uint8_t am = dd >> 7;
        uint8_t d1r = dd & 0x1f;

        slot->d1r = d1r;
        break;
    case 0xc0 ... 0xdF: /* DT2, D2R */
        uint8_t dt2 = dd >> 7;
        uint8_t d2r = dd & 0x1f;

        slot->d2r = d2r;
        break;
    case 0xe0 ... 0xff: /* D1L, RR */
        uint8_t d1l = dd >> 4;
        uint8_t rr = dd & 0x0f;

        slot->release_r = rr << 1;
        slot->decay_l = d1l << 1;
        break;
#endif
    default:
        break;
    }
}