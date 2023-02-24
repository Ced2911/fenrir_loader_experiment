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

//
void emu_printf(const char *format, ...)
{
   static char emu_printf_buffer[256];
   char *s = emu_printf_buffer;
   volatile uint8_t *addr = (volatile uint8_t *)CS1(0x1000);
   va_list args;

   va_start(args, format);
   (void)vsnprintf(emu_printf_buffer, 256, format, args);
   va_end(args);

   while (*s)
      *addr = (uint8_t)*s++;
}

#include "notes.h"

void snd_init();

#include "./coincoin.h"
#include "./atomic-robot-kid-bgma.h"
#include "./debug_vgm.h"
#include "./ys.h"
#include "./sf2_title.h"
#include "./sf2_ryu.h"
#include "./sn76496_sonic.h"
#include "./alex_kidd.h"
#include "./mj.smooth.h"

// chips
#include "./ym2151.h"
#include "./ym2203.h"
#include "./sn76496.h"

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

void vgm_restart(vgm_player_t *vgm_player)
{
   vgm_player->vgmpos = 0x34;
   uint32_t offset = get_vgm_ui32(vgm_player);
   if (offset == 0)
   {
      vgm_player->vgmpos = 0x40;
   }
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
   case 0x4f: // Game Gear PSG stereo, write dd to port 0x06
   case 0x50: // 0x50	dd	PSG (SN76489/SN76496) write value dd
      dat = get_vgm_ui8(vgm_player);
      sn76496_w(dat);
      break;
      // YM2612 port 0 - 1
   case 0x52:
   case 0x53:
      reg = get_vgm_ui8(vgm_player);
      dat = get_vgm_ui8(vgm_player);
      break;
      // 0x54	aa dd YM2151, write value dd to register aa
   case 0x54:
      reg = get_vgm_ui8(vgm_player);
      dat = get_vgm_ui8(vgm_player);
      ym2151_w(reg, dat);
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
   case 0x67: //  0x67 0x66 tt ss ss ss ss (data)

   {
      get_vgm_ui8(vgm_player); // 0x66
      uint8_t tt = get_vgm_ui8(vgm_player);
      uint32_t size = get_vgm_ui32(vgm_player);
      uint8_t *data_start = vgm_player->vgm + vgm_player->vgmpos;

      uint32_t chunk_pos = vgm_player->vgmpos;
      switch (tt)
      {
      case 0x8B:
         uint32_t rom_size = get_vgm_ui32(vgm_player);
         uint32_t rom_off = get_vgm_ui32(vgm_player);
         uint32_t rom_len = size - 8;
         dbgio_printf("oki m6295 data %04x %04x %04x\n", rom_size, rom_off, rom_len);
         break;
      default:
         break;
      }

      vgm_player->datpos = vgm_player->vgmpos;
      vgm_player->vgmpos = chunk_pos + size;
   }
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

   case 0xB8:                  // 0xB8	aa dd	OKIM6295, write value dd to register aa
      get_vgm_ui8(vgm_player); // 0x66
      get_vgm_ui8(vgm_player); // 0x00 data type
      break;
   case 0xe0:
      vgm_player->pcmpos = get_vgm_ui32(vgm_player);
      vgm_player->pcmoffset = 0;
      break;
   default:
      emu_printf("unknown cmd at 0x%x: 0x%x\n", vgm_player->vgmpos, command);
      vgm_player->vgmpos++;
      vgm_restart(vgm_player);
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

   vgm_player->vgmpos = 0x30;
   vgm_player->clock_ym2151 = get_vgm_ui32(vgm_player);

   vgm_player->vgmpos = 0x0C;
   vgm_player->clock_sn76 = get_vgm_ui32(vgm_player);

   vgm_player->vgmpos = 0x1c;
   vgm_player->vgmloopoffset = get_vgm_ui32(vgm_player);

   vgm_player->vgmpos = 0x34;
   uint32_t offset = get_vgm_ui32(vgm_player);
   if (offset == 0)
   {
      vgm_player->vgmpos = 0x40;
   }
   else
   {
      vgm_player->vgmpos = 0x34 + offset;
   }

   if (vgm_player->clock_ym2151)
      ym2151_init();

   if (vgm_player->clock_ym2203)
      ym2203_init();

   if (vgm_player->clock_sn76)
      sn76496_init();

   if (vgm_player->clock_ym2203 == 0)
      vgm_player->clock_ym2203 = 3000000; // 4000000;
}

/* C:\Users\cc\Downloads\DefleMask_Legacy_Windows (1)\a4_sms.vgm (05/02/2023 12:43:21)
   DébutPosition(h): 00000000, FinPosition(h): 000000E7, Longueur(h): 000000E8 */

unsigned char a4_sms[232] = {
    0x56, 0x67, 0x6D, 0x20, 0xE4, 0x00, 0x00, 0x00, 0x50, 0x01, 0x00, 0x00,
    0x99, 0x9E, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00,
    0x40, 0x27, 0x02, 0x00, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3C, 0x00, 0x00, 0x00, 0x09, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x50, 0x9F, 0x50, 0xBF, 0x50, 0xDF, 0x50, 0xFF,
    0x50, 0x90, 0x50, 0x80, 0x50, 0x04, 0x61, 0xFF, 0xFF, 0x61, 0xFF, 0xFF,
    0x61, 0x42, 0x27, 0x66, 0x47, 0x64, 0x33, 0x20, 0x00, 0x01, 0x00, 0x00,
    0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x53, 0x00, 0x65, 0x00, 0x67, 0x00, 0x61, 0x00, 0x20, 0x00, 0x4D, 0x00,
    0x61, 0x00, 0x73, 0x00, 0x74, 0x00, 0x65, 0x00, 0x72, 0x00, 0x20, 0x00,
    0x53, 0x00, 0x79, 0x00, 0x73, 0x00, 0x74, 0x00, 0x65, 0x00, 0x6D, 0x00,
    0x20, 0x00, 0x2F, 0x00, 0x20, 0x00, 0x47, 0x00, 0x61, 0x00, 0x6D, 0x00,
    0x65, 0x00, 0x20, 0x00, 0x47, 0x00, 0x65, 0x00, 0x61, 0x00, 0x72, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x00,
    0x65, 0x00, 0x6C, 0x00, 0x65, 0x00, 0x6B, 0x00, 0x27, 0x00, 0x73, 0x00,
    0x20, 0x00, 0x44, 0x00, 0x65, 0x00, 0x66, 0x00, 0x6C, 0x00, 0x65, 0x00,
    0x4D, 0x00, 0x61, 0x00, 0x73, 0x00, 0x6B, 0x00, 0x20, 0x00, 0x54, 0x00,
    0x72, 0x00, 0x61, 0x00, 0x63, 0x00, 0x6B, 0x00, 0x65, 0x00, 0x72, 0x00,
    0x00, 0x00, 0x00, 0x00};

int vgm_init(vgm_player_t *vgm_player)
{
   dbgio_init();
   dbgio_dev_default_init(DBGIO_DEV_VDP2_ASYNC);
   dbgio_dev_font_load();

   smpc_smc_sndon_call();

   // vgm_player->vgm = debug_2151_vgm;
   // vgm_player->vgm = atomic_kid;
   // vgm_player->vgm = ys_vgm;
   // vgm_player->vgm = sf2_ryu;
   // vgm_player->vgm = sf2_title;
   vgm_player->vgm = mj_smooth_sn;
   vgm_player->vgm = sonic_sn76496;
   // vgm_player->vgm = alexkidd_sn;
   // vgm_player->vgm = a4_sms;
   vgm_parse_header(vgm_player);

   vgm_player->sampled = 0;
   vgm_player->frame_size = -1;
   vgm_player->sample_count = 0;
   vgm_player->cycles = 0;
   vgm_player->cycles_played = 0;

   emu_printf("vgm_player->vgmloopoffset 0x%x\n", vgm_player->vgmloopoffset);
   dbgio_printf("vgm_player->vgmloopoffset 0x%x\n", vgm_player->vgmloopoffset);

   return 0;
}
