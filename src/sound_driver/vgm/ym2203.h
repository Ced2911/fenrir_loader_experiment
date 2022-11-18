#pragma once
#include <stdint.h>
#include <string.h>
#include "../scsp.h"
#include "xfo.h"

static uint16_t algo_table[] = {
    /**/
    0, 0X87DF, 0X87DF, 0X87DF,
    0, 0X87DF, 0X87DF, 0X87DF,
    0, 0X87DF, 0X87DF, 0X87DF,
    0, 0, 0, 0,

    /**/
    0, 0, 0X879F, 0X87DF,
    0, 0, 0X879F, 0X87DF,
    0, 0, 0X879F, 0X87DF,
    0, 0, 0, 0,

    /**/
    0, 0, 0X87DF, 0X87DD,
    0, 0, 0X87DF, 0X87DD,
    0, 0, 0X87DF,
    0X87DD, 0, 0, 0, 0,

    /**/
    0, 0X87DF, 0, 0X87DE,
    0, 0X87DF, 0, 0X87DE,
    0, 0X87DF, 0, 0X87DE,
    0, 0, 0, 0,

    /**/
    0, 0X87DF, 0, 0X87DF, 0,
    0X87DF, 0, 0X87DF,
    0, 0X87DF, 0, 0X87DF,
    0, 0, 0, 0,

    /**/
    0, 0X87DF, 0X879E, 0X875D,
    0, 0X87DF, 0X879E, 0X875D,
    0, 0X87DF, 0X879E, 0X875D,
    0, 0, 0, 0,

    /**/
    0, 0X87DF, 0, 0,
    0, 0X87DF, 0, 0,
    0, 0X87DF, 0, 0,
    0, 0, 0, 0,

    /**/ 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

unsigned char c4_8bpp[337] = {
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

/*****************************************************
 * ym2203
 ****************************************************/
/**
 * from https://antime.kapsi.fi/sega/files/ST-077-R2-052594.pdf
 * see Table 4.20
 */
#define SCSP_FREQ_R ((int)(65536.f / ((440.0f - 261.63f) / 698.2f)))

// calcule frequence ym
// example:
// * ratio (~2.359296)
//       ratio = (Math.pow(2, 20))/(8*Math.pow(10,6))/Math.pow(2, 3)*144
// *  freq vers Fnumber
//       Fnumber = 440 * r
//       * 440 => notes A4
// * Fnumber vers freq
//       freq = FNumber/ratio
#define YM_FREQ_R ((int)(65536.f / 4.718592f))

uint8_t ym2203_regs[256];

uint32_t freq_ym2203_ratio = 1736; //; (uint32_t)((1.0 / (144.0 * ((1 << 20) / (4000000)))) * (1 << 16));

void ym2203_init()
{
    snd_init();
    // copy sinetable
    uint32_t addr = (0x2000);
// 5a02000
#if 0
   memcpy((void *)(SCSP_RAM + addr), SINEPATT_BIN, SINEPATT_BIN_len);
   memcpy((void *)(SCSP_RAM + addr + SINEPATT_BIN_len), SINEPATT_BIN, SINEPATT_BIN_len);
   memcpy((void *)(SCSP_RAM + addr + SINEPATT_BIN_len + SINEPATT_BIN_len), SINEPATT_BIN, SINEPATT_BIN_len);
#else

    memcpy((void *)(SCSP_RAM + addr), c4_8bpp, sizeof(c4_8bpp));
    memcpy((void *)(SCSP_RAM + addr + sizeof(c4_8bpp)), c4_8bpp, sizeof(c4_8bpp));
    memcpy((void *)(SCSP_RAM + addr + sizeof(c4_8bpp) + sizeof(c4_8bpp)), c4_8bpp, sizeof(c4_8bpp));
#endif

    // memcpy((void *)(SCSP_RAM + addr), note_c4, sizeof(note_c4));
    memset(ym2203_regs, 0, 256);

    ym2203_regs[0xb4] = 0xC0;
    ym2203_regs[0xb5] = 0xC0;
    ym2203_regs[0xb6] = 0xC0;

    volatile uint16_t *control = (uint16_t *)0x25b00400;
    volatile uint16_t *intr = (uint16_t *)0x25b0042a;
    control[0] = 0xf; // master vol
    intr[0] = 0;      // irq

    // init slots...
    volatile scsp_slot_regs_t *slots = get_scsp_slot(0);
    for (int i = 0; i < 3 * 4; i++)
    {
        /*
         [y].w=sinepatt shr 16+$130 > y=_+2   ; $00 main
         [y].w=sinepatt and $FFFF > y=_+2     ; $02 samp addr low word
         [y].w=0 > y=_+2                      ; $04 loop begin
         [y].w=$80 > y=_+2                      ; $06 loop end

         [y].w=$1F > y=_+2            ; $08 decay1/decay2/attack
         [y].w=$1F > y=_+2            ; $0A sustain/release

         [y].w=255 > y=_+2            ; $0C volume/etc
         [y].w=0 > y=_+2              ; $0E modulation
         [y].w=0 > y=_+2              ; $10 octave/freq
         [y].w=0 > y=_+2              ; $12 LFO
         [y].w=0 > y=_+2              ; $14 DSP mixing

        */
        slots[i].raw[0] = (addr << 16) | 0x130;
        slots[i].raw[1] = addr & 0xffff;
        slots[i].raw[2] = 0;
        slots[i].raw[3] = sizeof(rawData);

        slots[i].raw[4] = 0x1F;
        slots[i].raw[5] = 0x1F;

        slots[i].raw[6] = 255;
        slots[i].raw[7] = 0;
        slots[i].raw[8] = 0;
        slots[i].raw[9] = 0;
        slots[i].raw[10] = 0;

        slots[i].raw[11] = 0xA000;
        slots[i].raw[12] = 0;

        // slots[i].lpctl = 1;
        // slots[i].disdl = 7;
        slots[i].kyonb = 0;
        slots[i].kyonex = 0;
    }
}

/* http://www.vgmpf.com/Wiki/images/f/f5/YM2203_-_Manual.pdf **/
/* https://github.com/mkoloberdin/unrealspeccy/blob/master/emul_2203.cpp */
// http://nemesis.hacking-cult.org/MegaDrive/Documentation/YM2608J%20Translated.PDF page 25
// https://pages.mtu.edu/~suits/notefreqs.html
// math:
// cent = 1200*log2((1024+ fns )/1024)
//
// 1 channel => 4slots
//
// freq hz  - note 4 - fnumber
// 277.2    |  c4#   |  654
// 293.7    |  D4    |  692.8
// 311.1    |  D4#   |  734.0
// 329.6    |  E     |  777.7
// 349.2    |  F     |  823.9
// 370.0    |  F#    |  872.9
// 392.0    |  G     |  924.8
// 415.3    |  G#    |  979.8
// 440.0    |  A     |  1038.1
// 466.2    |  A#    |  1099.8
// 493.9    |  B     |  1165.2
// 523.3    |  C     |  1234.5
//
//

uint16_t fnum_to_idx[] = {
    617, 653, 692, 733, 777, 823, 872, 924, 1037, 1099, 1164, 1234};
uint16_t fncs_from_idx[] = {
    0x3d, 0x7d, 0xc2, 0x10a, 0x157, 0x1a8, 0x1fe, 0x25a, 0x2ba, 0x321, 0x38d};

#define OPN_SLOT(N) ((N >> 2) & 3)
void opn_write_mode(uint8_t r, uint8_t v)
{
    uint8_t chan_v = v & 0x03;
    uint8_t chan_r = r & 0x03;
    uint8_t op = OPN_SLOT(r);

    volatile scsp_slot_regs_t *slots_per_op = get_scsp_slot(chan_r * 4);
    volatile scsp_slot_regs_t *slots = get_scsp_slot(chan_r * 4);
    volatile scsp_slot_regs_t *slot0 = &slots[0];
    volatile scsp_slot_regs_t *slot1 = &slots[1];
    volatile scsp_slot_regs_t *slot2 = &slots[2];
    volatile scsp_slot_regs_t *slot3 = &slots[3];

    // return;
    //
    switch (r)
    {
    case 0x28: /* key on / off */
               // ifgreater y and 7,2,v100
        if (chan_v > 2)
            break;
        // sreg=y and 3 shl 7+$100000
        volatile scsp_slot_regs_t *slotschan = get_scsp_slot(chan_v * 4);
        // slotschan[0].sa = 0x1000;
        slotschan[0].kyonb = v & 0x10 ? 1 : 0;
        slotschan[1].kyonb = v & 0x20 ? 1 : 0;
        slotschan[2].kyonb = v & 0x30 ? 1 : 0;
        slotschan[3].kyonb = v & 0x40 ? 1 : 0;
        slotschan[3].kyonex = v & 0x40 ? 1 : 0;

        break;
    case 0x40 ... 0x4f: /* set total level */
        // return;
        slots_per_op[op].total_l = v << 1;
        break;
    case 0x50 ... 0x5f: /* set attack rate & key scale */
        // return;
        slots_per_op[op].attack_rate = v & 31;
        // slots_per_op[op].kr_scale = (v >> 6) & 3;
        break;
    case 0x60 ... 0x6f: /* set decay rate */
        // return;
        slots_per_op[op].d1r = v & 31;
        break;
    case 0x70 ... 0x7f: /* set sustain rate */
        // return;
        // return;
        slots_per_op[op].d2r = v & 31;
        break;
    case 0x80 ... 0x8f: /* set release rate  */
        return;

        // slots[sl].release_r = v & 31;
        //  sreg=_+$0A > [sreg].w=y and $F0 shl 1+(y and 15) shl 1
        // slots_per_op[op].raw[5] = ((v & 0xF0) << 1) | (v & 15) << 1;
        slots_per_op[op].release_r = v & 0x0f;
        slots_per_op[op].decay_l = (v >> 4) & 0x0f;
        break;
    case 0xa0 ... 0xaf:
        uint16_t opn_r_fnum = ((ym2203_regs[0xA4 + chan_r] & 7) << 8) | (ym2203_regs[0xA0 + chan_r]);
        uint16_t opn_r_block = ((ym2203_regs[0xA4 + chan_r] >> 3));

        uint32_t freq = opn_r_fnum * (1 << opn_r_block);

        // (((1.0 / (144.0 * ((1 << 20) / (cpuspeed)))) * (1 << 16)) * (fnum * (1<<blk)) )>>16

        uint32_t real_fq_16 = (freq * freq_ym2203_ratio) >> 8;
        uint32_t real_fq = (freq * freq_ym2203_ratio) >> 16;
        uint32_t tmp_fq = (freq * freq_ym2203_ratio) >> 16;

        uint32_t scsp_freq = real_fq;
        uint32_t scsp_oct = 0;
        uint32_t scsp_fns = 0;
        // 261.63 * Math.pow(2, -48/12)

        // find oct
        while (1)
        {
            if (oct_f[scsp_oct + 1] > real_fq_16)
            {
                break;
            }
            scsp_oct++;
            tmp_fq >>= 1;
        }

        // find fns

        if (1)
        { // tmp_fq devrait etre l'ocatve 0
            for (scsp_fns = 0; scsp_fns < 1200; scsp_fns++)
            {

                if ((zizi_0[scsp_fns + 1]>>8) > tmp_fq)
                {
                    break;
                }
            }
            scsp_freq = scsp_freq >> 1;
        }

        // dbgio_printf("freq_ym2203_ratio: %d\n", freq_ym2203_ratio);
        dbgio_printf("real_fq: %d - tmp_fq:%d\n", real_fq, tmp_fq);
        dbgio_printf("opn_r_fnum: %d   -- scsp fnum:%d\n", opn_r_fnum, scsp_fns);
        dbgio_printf("opn_r_block: %d  -- scsp oct: %d\n", opn_r_block, scsp_oct);

        slot0->oct = scsp_oct - 4;
        slot1->oct = scsp_oct - 4;
        slot2->oct = scsp_oct - 4;
        slot3->oct = scsp_oct - 4;

        slot0->fns = scsp_fns;
        slot1->fns = scsp_fns;
        slot2->fns = scsp_fns;
        slot3->fns = scsp_fns;
        break;

    case 0xb0 ... 0xbe:
        // return;

        // w=newreg($B0+chan) and 7 shl 2+chan shl 3+algotable.a
        uint8_t reg_c = ym2203_regs[0xb0 + chan_r] & 7;

        uint8_t algo = ((reg_c & 7) << 2) | (chan_r);
        slot0->raw[0xe >> 1] = algo_table[(algo * 4) + 0];
        slot1->raw[0xe >> 1] = algo_table[(algo * 4) + 1];
        slot2->raw[0xe >> 1] = algo_table[(algo * 4) + 2];
        slot3->raw[0xe >> 1] = algo_table[(algo * 4) + 3];

        break;
    default:
        break;
    }
}

void ym2203_w(uint8_t aa, uint8_t dd)
{
    uint8_t addr = aa;
    ym2203_regs[addr] = dd;
    opn_write_mode(addr, dd);
}
