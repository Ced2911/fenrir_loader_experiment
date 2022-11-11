#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <yaul.h>
#include "vgm.h"
#include "sinepatt.h"
#include "../scsp.h"
#include "tr.h"
#include "empty_drv.h"

void snd_init()
{
   *(volatile uint8_t *)(0x25B00400) = 0x02;
   // Turn off Sound CPU
   smpc_smc_sndoff_call();

   // Make sure SCSP is set to 512k mode
   *(volatile uint8_t *)(0x25B00400) = 0x02;

   // Clear Sound Ram
   for (int i = 0; i < 0x80000; i += 4)
      *(volatile uint32_t *)(0x25A00000 + i) = 0x00000000;

   // Copy driver over
   for (int i = 0; i < sound_driver_size; i++)
      *(volatile uint8_t *)(0x25A00000 + i) = sound_driver[i];

   // Turn on Sound CPU again
   smpc_smc_sndon_call();
}

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

void ym2203_w(uint8_t aa, uint8_t dd);

#define SAMPLING_RATE 44100

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

uint16_t vgm_parse(vgm_player_t *vgm_player)
{
   uint8_t command;
   uint16_t wait = 0;
   uint8_t reg;
   uint8_t dat;

   command = get_vgm_ui8(vgm_player);
   switch (command)
   {
      /*
      AY8910 stereo mask, dd is a bit mask of i y r3 l3 r2 l2 r1 l1 (bit 7 ... 0)
      i	chip instance (0 or 1)
      y	set stereo mask for YM2203 SSG (1) or AY8910 (0)
      l1/l2/l3	enable channel 1/2/3 on left speaker
      r1/r2/r3	enable channel 1/2/3 on right speaker
      */
   case 0x31:
      dat = get_vgm_ui8(vgm_player);
      // ignored
      break;

      // SN76489/SN76496
   case 0x50:
      dat = get_vgm_ui8(vgm_player);
      break;
      // YM2612 port 0 - 1
   case 0x52:
   case 0x53:
      reg = get_vgm_ui8(vgm_player);
      dat = get_vgm_ui8(vgm_player);
      break;
      // 0x55	aa dd	YM2203, write value dd to register aa
   case 0x55:
      reg = get_vgm_ui8(vgm_player);
      dat = get_vgm_ui8(vgm_player);
      ym2203_w(reg, dat);
      break;

      // wait
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
         vgm_player->vgmpos++;
      }
      break;
   case 0x67:
      get_vgm_ui8(vgm_player); // 0x66
      get_vgm_ui8(vgm_player); // 0x00 data type
      vgm_player->datpos = vgm_player->vgmpos + 4;
      vgm_player->vgmpos += get_vgm_ui32(vgm_player); // size of data, in bytes
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
   default:
      dbgio_printf("unknown cmd at 0x%x: 0x%x\n", vgm_player->vgmpos, command);
      vgm_player->vgmpos++;
      break;
   }

   // dbgio_printf("command: %x\nwait:%d\n", command, wait);

   if (vgm_player->vgmend)
   {
      vgm_player->pcmpos = 0x34;
      vgm_player->vgmpos = 0x34 + get_vgm_ui32(vgm_player);
      vgm_player->vgmend = 0;
   }
   vgm_player->cycles += wait;
   return wait;
}

static void vgm_parse_header(vgm_player_t *vgm_player)
{
   // vgm_player->vgmpos = 0x40;
   vgm_player->vgmend = 0;

   // parse header
   vgm_player->vgmpos = 0x44;
   vgm_player->clock_ym2203 = get_vgm_ui32(vgm_player);

   vgm_player->vgmpos = 0x1c;
   vgm_player->vgmloopoffset = get_vgm_ui32(vgm_player);

   vgm_player->vgmpos = 0x34;
   vgm_player->vgmpos = 0x34 + get_vgm_ui32(vgm_player);

   if (vgm_player->clock_ym2203 == 0)
      vgm_player->clock_ym2203 = 3000000; // 4000000;
}

int vgm_init(vgm_player_t *vgm_player)
{
   dbgio_init();
   dbgio_dev_default_init(DBGIO_DEV_VDP2_ASYNC);
   dbgio_dev_font_load();

   smpc_smc_sndon_call();

   vgm_player->vgm = rawData;
   vgm_parse_header(vgm_player);

   vgm_player->sampled = 0;
   vgm_player->frame_size = -1;
   vgm_player->sample_count = 0;
   vgm_player->cycles = 0;
   vgm_player->cycles_played = 0;

   dbgio_printf("vgm_player->vgmloopoffset 0x%x\n", vgm_player->vgmloopoffset);

   return 0;
}

/**
 * from https://antime.kapsi.fi/sega/files/ST-077-R2-052594.pdf
 * see Table 4.20
 */
#define SCSP_FREQ_R ((int)(((440.0f - 261.63f) / 698.2f) * 65536.f))

// calcule frequence ym
// example:
// * ratio (~2.359296)
//       ratio = (Math.pow(2, 20))/(8*Math.pow(10,6))/Math.pow(2, 3)*144
// *  freq vers Fnumber
//       Fnumber = 440 * r
//       * 440 => notes A4
// * Fnumber vers freq
//       freq = FNumber/ratio
#define YM_FREQ_R ((int)(65536.f/2.359296f)

uint8_t ym2203_regs[256];

void ym2203_init()
{
   snd_init();
   // copy sinetable
   uint32_t addr = (0x2000);
   // 5a02000
   memcpy((void *)(SCSP_RAM + addr), SINEPATT_BIN, SINEPATT_BIN_len);
   memcpy((void *)(SCSP_RAM + addr + SINEPATT_BIN_len), SINEPATT_BIN, SINEPATT_BIN_len);
   memcpy((void *)(SCSP_RAM + addr + SINEPATT_BIN_len + SINEPATT_BIN_len), SINEPATT_BIN, SINEPATT_BIN_len);

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
      slots[i].raw[3] = 0x80;

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
      if (chan_v > 3)
         break;
#if 1
      // sreg=y and 3 shl 7+$100000
      volatile scsp_slot_regs_t *slotschan = get_scsp_slot(chan_v * 4);
      // slotschan[0].sa = 0x1000;

#if 0
      slotschan[0].lpctl = 1;
      slotschan[1].lpctl = 1;
      slotschan[2].lpctl = 1;
      slotschan[3].lpctl = 1;
#endif
#if 1
      slotschan[0].kyonb = v & 0x10 ? 1 : 0;
      slotschan[1].kyonb = v & 0x20 ? 1 : 0;
      slotschan[2].kyonb = v & 0x30 ? 1 : 0;
      slotschan[3].kyonb = v & 0x40 ? 1 : 0;
#endif
#if 1
      slotschan[0].kyonex = v & 0x10 ? 1 : 0;
      slotschan[1].kyonex = v & 0x20 ? 1 : 0;
      slotschan[2].kyonex = v & 0x30 ? 1 : 0;
      slotschan[3].kyonex = v & 0x40 ? 1 : 0;
#endif
#else

#endif

      // dbgio_printf("vgm_player key ch:%d v:0x%x\n", chan_v, v >> 4);

      //         [$1003E0].b=$10                     ; let 'er rip
      uint8_t *u0 = (uint8_t *)0x25b003e0;
      *u0 = 0x10;

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
      // return;
      uint16_t opn_r_fnum = ((ym2203_regs[0xA4 + chan_r] & 7) << 8) | (ym2203_regs[0xA0 + chan_r]);
      uint16_t opn_r_block = ((ym2203_regs[0xA4 + chan_r] >> 3));

      for (int i = 0; i < 4; i++)
      {

         // scsp
         // fns = Math.pow(2,10)*(Math.pow(2,cent/1200)-1)
         uint32_t freq_hz = (opn_r_fnum * YM_FREQ_R) >> 16;
         uint32_t freq_scsp = ((440 - freq_hz) * SCSP_FREQ_R) >> 16;

#if 0
         uint16_t opn_r_multi = (ym2203_regs[0x30 + chan_r + i] & 0xf);

         uint16_t opn_f = opn_r_fnum;
         uint16_t opn_b = 1 << opn_r_block;
         uint16_t opn_m = opn_r_multi << 1;

         uint32_t f = opn_f * opn_b * opn_m;

         uint16_t octave = 0;
         if (f > 100)
         {
            while (f > 44098)
            {
               octave += 1;
               f = f >> 1;
            }
            while (f < 22050)
            {
               octave -= 1;
               f = f << 1;
            }
            f = (f - (22050 * 190)) >> 12;

            slots[i].fns = f;
            slots[i].oct = octave;
         }
#else

         slots[i].oct = opn_r_block;
         slots[i].fns = opn_r_fnum;
#endif
      }
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
