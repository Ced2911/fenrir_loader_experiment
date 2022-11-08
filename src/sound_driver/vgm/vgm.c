#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <yaul.h>
#include "vgm.h"
#include "sinepatt.h"
#include "../scsp.h"
#include "ys.h"
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
      dbgio_printf("unknown cmd at 0x%x: 0x%x\n", vgm_player->vgmpos, vgm_player->vgm[vgm_player->vgmpos]);
      vgm_player->vgmpos++;
      break;
   }

   // dbgio_printf("command: %x\nwait:%d\n", command, wait);

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

   vgm_player->vgm = ys_vgm;
   vgm_parse_header(vgm_player);

   vgm_player->sampled = 0;
   vgm_player->frame_size = -1;
   vgm_player->sample_count = 0;

   return 0;
}

uint8_t ym2203_regs[256];

static const uint8_t note_c4[674] = {
    0x00, 0x00, 0x04, 0xC5, 0x09, 0x88, 0x0E, 0x48, 0x13, 0x03, 0x17, 0xB7,
    0x1C, 0x63, 0x21, 0x05, 0x25, 0x9B, 0x2A, 0x23, 0x2E, 0x9D, 0x33, 0x06,
    0x37, 0x5D, 0x3B, 0xA0, 0x3F, 0xCE, 0x43, 0xE5, 0x47, 0xE4, 0x4B, 0xCA,
    0x4F, 0x94, 0x53, 0x42, 0x56, 0xD3, 0x5A, 0x45, 0x5D, 0x97, 0x60, 0xC7,
    0x63, 0xD5, 0x66, 0xBF, 0x69, 0x85, 0x6C, 0x26, 0x6E, 0xA0, 0x70, 0xF2,
    0x73, 0x1C, 0x75, 0x1E, 0x76, 0xF6, 0x78, 0xA3, 0x7A, 0x26, 0x7B, 0x7D,
    0x7C, 0xA8, 0x7D, 0xA7, 0x7E, 0x79, 0x7F, 0x1E, 0x7F, 0x96, 0x7F, 0xE1,
    0x7F, 0xFE, 0x7F, 0xEE, 0x7F, 0xB0, 0x7F, 0x44, 0x7E, 0xAC, 0x7D, 0xE6,
    0x7C, 0xF4, 0x7B, 0xD5, 0x7A, 0x8A, 0x79, 0x14, 0x77, 0x72, 0x75, 0xA6,
    0x73, 0xB0, 0x71, 0x91, 0x6F, 0x4A, 0x6C, 0xDB, 0x6A, 0x45, 0x67, 0x89,
    0x64, 0xA9, 0x61, 0xA5, 0x5E, 0x7E, 0x5B, 0x35, 0x57, 0xCD, 0x54, 0x44,
    0x50, 0x9E, 0x4C, 0xDB, 0x48, 0xFD, 0x45, 0x05, 0x40, 0xF5, 0x3C, 0xCD,
    0x38, 0x8F, 0x34, 0x3E, 0x2F, 0xDA, 0x2B, 0x65, 0x26, 0xE0, 0x22, 0x4E,
    0x1D, 0xAF, 0x19, 0x06, 0x14, 0x54, 0x0F, 0x9B, 0x0A, 0xDC, 0x06, 0x1A,
    0x01, 0x55, 0xFC, 0x90, 0xF7, 0xCC, 0xF3, 0x0B, 0xEE, 0x4F, 0xE9, 0x98,
    0xE4, 0xEA, 0xE0, 0x45, 0xDB, 0xAC, 0xD7, 0x1F, 0xD2, 0xA2, 0xCE, 0x34,
    0xC9, 0xD8, 0xC5, 0x8F, 0xC1, 0x5B, 0xBD, 0x3D, 0xB9, 0x37, 0xB5, 0x4A,
    0xB1, 0x78, 0xAD, 0xC2, 0xAA, 0x29, 0xA6, 0xAE, 0xA3, 0x53, 0xA0, 0x1A,
    0x9D, 0x02, 0x9A, 0x0E, 0x97, 0x3D, 0x94, 0x92, 0x92, 0x0E, 0x8F, 0xB0,
    0x8D, 0x7A, 0x8B, 0x6D, 0x89, 0x8A, 0x87, 0xD0, 0x86, 0x42, 0x84, 0xDF,
    0x83, 0xA7, 0x82, 0x9C, 0x81, 0xBD, 0x81, 0x0B, 0x80, 0x87, 0x80, 0x2F,
    0x80, 0x05, 0x80, 0x09, 0x80, 0x3A, 0x80, 0x99, 0x81, 0x25, 0x81, 0xDE,
    0x82, 0xC4, 0x83, 0xD7, 0x85, 0x15, 0x86, 0x80, 0x88, 0x15, 0x89, 0xD5,
    0x8B, 0xC0, 0x8D, 0xD3, 0x90, 0x0F, 0x92, 0x73, 0x94, 0xFE, 0x97, 0xB0,
    0x9A, 0x86, 0x9D, 0x80, 0xA0, 0x9D, 0xA3, 0xDC, 0xA7, 0x3C, 0xAA, 0xBC,
    0xAE, 0x5A, 0xB2, 0x15, 0xB5, 0xEB, 0xB9, 0xDC, 0xBD, 0xE6, 0xC2, 0x08,
    0xC6, 0x3F, 0xCA, 0x8B, 0xCE, 0xEA, 0xD3, 0x5B, 0xD7, 0xDB, 0xDC, 0x6A,
    0xE1, 0x05, 0xE5, 0xAC, 0xEA, 0x5B, 0xEF, 0x13, 0xF3, 0xD0, 0xF8, 0x92,
    0xFD, 0x56, 0x02, 0x1B, 0x06, 0xDF, 0x0B, 0xA1, 0x10, 0x5F, 0x15, 0x18,
    0x19, 0xC8, 0x1E, 0x70, 0x23, 0x0C, 0x27, 0x9D, 0x2C, 0x1F, 0x30, 0x91,
    0x34, 0xF2, 0x39, 0x41, 0x3D, 0x7B, 0x41, 0x9F, 0x45, 0xAC, 0x49, 0xA0,
    0x4D, 0x79, 0x51, 0x38, 0x54, 0xD9, 0x58, 0x5C, 0x5B, 0xC0, 0x5F, 0x03,
    0x62, 0x24, 0x65, 0x23, 0x67, 0xFD, 0x6A, 0xB3, 0x6D, 0x42, 0x6F, 0xAB,
    0x71, 0xEC, 0x74, 0x04, 0x75, 0xF3, 0x77, 0xB9, 0x79, 0x53, 0x7A, 0xC3,
    0x7C, 0x06, 0x7D, 0x1E, 0x7E, 0x09, 0x7E, 0xC8, 0x7F, 0x59, 0x7F, 0xBD,
    0x7F, 0xF3, 0x7F, 0xFC, 0x7F, 0xD8, 0x7F, 0x86, 0x7F, 0x07, 0x7E, 0x5A,
    0x7D, 0x81, 0x7C, 0x7B, 0x7B, 0x48, 0x79, 0xEA, 0x78, 0x61, 0x76, 0xAC,
    0x74, 0xCE, 0x72, 0xC5, 0x70, 0x94, 0x6E, 0x3B, 0x6B, 0xBB, 0x69, 0x15,
    0x66, 0x49, 0x63, 0x59, 0x60, 0x45, 0x5D, 0x0F, 0x59, 0xB8, 0x56, 0x41,
    0x52, 0xAC, 0x4E, 0xF9, 0x4B, 0x2A, 0x47, 0x40, 0x43, 0x3D, 0x3F, 0x22,
    0x3A, 0xF0, 0x36, 0xAA, 0x32, 0x50, 0x2D, 0xE4, 0x29, 0x68, 0x24, 0xDD,
    0x20, 0x45, 0x1B, 0xA2, 0x16, 0xF5, 0x12, 0x3F, 0x0D, 0x83, 0x08, 0xC3,
    0x03, 0xFF, 0xFF, 0x3B, 0xFA, 0x76, 0xF5, 0xB3, 0xF0, 0xF3, 0xEC, 0x39,
    0xE7, 0x86, 0xE2, 0xDC, 0xDE, 0x3C, 0xD9, 0xA8, 0xD5, 0x22, 0xD0, 0xAB,
    0xCC, 0x45, 0xC7, 0xF1, 0xC3, 0xB1, 0xBF, 0x87, 0xBB, 0x74, 0xB7, 0x79,
    0xB3, 0x97, 0xAF, 0xD1, 0xAC, 0x28, 0xA8, 0x9C, 0xA5, 0x2F, 0xA1, 0xE3,
    0x9E, 0xB8, 0x9B, 0xB0, 0x98, 0xCB, 0x96, 0x0B, 0x93, 0x71, 0x90, 0xFD,
    0x8E, 0xB1, 0x8C, 0x8E, 0x8A, 0x93, 0x88, 0xC2, 0x87, 0x1B, 0x85, 0xA0,
    0x84, 0x50, 0x83, 0x2C, 0x82, 0x34, 0x81, 0x69, 0x80, 0xCB, 0x80, 0x5A,
    0x80, 0x17, 0x80, 0x02, 0x80, 0x19, 0x80, 0x5F, 0x80, 0xD1, 0x81, 0x71,
    0x82, 0x3E, 0x83, 0x38, 0x84, 0x5E, 0x85, 0xB0, 0x87, 0x2D, 0x88, 0xD6,
    0x8A, 0xA9, 0x8C, 0xA5, 0x8E, 0xCB, 0x91, 0x19, 0x93, 0x8E, 0x96, 0x2A,
    0x98, 0xEB, 0x9B, 0xD2, 0x9E, 0xDC, 0xA2, 0x08, 0xA5, 0x56, 0xA8, 0xC4,
    0xAC, 0x51, 0xAF, 0xFC, 0xB3, 0xC3, 0xB7, 0xA6, 0xBB, 0xA2, 0xBF, 0xB6,
    0xC3, 0xE2, 0xC8, 0x22, 0xCC, 0x77, 0xD0, 0xDE, 0xD5, 0x56, 0xD9, 0xDC,
    0xDE, 0x71, 0xE3, 0x11, 0xE7, 0xBC, 0xEC, 0x6F, 0xF1, 0x2A, 0xF5, 0xE9,
    0xFA, 0xAC};

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
      slots[i].raw[11] = 0;
      slots[i].raw[12] = 0;

      slots[i].lpctl = 1;
      slots[i].disdl = 7;
      slots[i].kyonb = 1;
      slots[i].kyonex = 1;
   }
}

/* http://www.vgmpf.com/Wiki/images/f/f5/YM2203_-_Manual.pdf **/
/* https://github.com/mkoloberdin/unrealspeccy/blob/master/emul_2203.cpp */
// 1 channel => 4slots
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

      slotschan[0].lpctl = 1;
      slotschan[1].lpctl = 1;
      slotschan[2].lpctl = 1;
      slotschan[3].lpctl = 1;

      slotschan[0].kyonb = v & 0x10 ? 1 : 0;
      slotschan[1].kyonb = v & 0x20 ? 1 : 0;
      slotschan[2].kyonb = v & 0x30 ? 1 : 0;
      slotschan[3].kyonb = v & 0x40 ? 1 : 0;

      slotschan[0].kyonex = v & 0x10 ? 1 : 0;
      slotschan[1].kyonex = v & 0x20 ? 1 : 0;
      slotschan[2].kyonex = v & 0x30 ? 1 : 0;
      slotschan[3].kyonex = v & 0x40 ? 1 : 0;
#else

#endif

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
      // return;

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
         uint16_t opn_r_multi = (ym2203_regs[0x30 + chan_r + i] & 15);

         uint16_t opn_f = opn_r_fnum;
         uint16_t opn_b = 1 << opn_r_block;
         uint16_t opn_m = opn_r_multi << 1;

         uint32_t f = opn_f * opn_b * opn_m;

         uint16_t octave = 0;
         if (f != 0)
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
      }
      break;

   case 0xb0 ... 0xbe:
      return;

      // w=newreg($B0+chan) and 7 shl 2+chan shl 3+algotable.a

      uint8_t algo = ((v & 7) << 2) | (chan_r << 3);
      slot0->raw[0xe >> 1] = algo_table[(algo * 1) + 0];
      slot1->raw[0xe >> 1] = algo_table[(algo * 1) + 1];
      slot2->raw[0xe >> 1] = algo_table[(algo * 1) + 2];
      slot3->raw[0xe >> 1] = algo_table[(algo * 1) + 3];

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
