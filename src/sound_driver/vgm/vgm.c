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

#include "notes.h"

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

#include "./ym2151.h"
#include "./ym2203.h"

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

   case 0xB8: // 0xB8	aa dd	OKIM6295, write value dd to register aa

      get_vgm_ui8(vgm_player); // 0x66
      get_vgm_ui8(vgm_player); // 0x00 data type
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

   vgm_player->vgmpos = 0x30;
   vgm_player->clock_ym2151 = get_vgm_ui32(vgm_player);

   vgm_player->vgmpos = 0x1c;
   vgm_player->vgmloopoffset = get_vgm_ui32(vgm_player);

   vgm_player->vgmpos = 0x34;
   vgm_player->vgmpos = 0x34 + get_vgm_ui32(vgm_player);

   if (vgm_player->clock_ym2151)
      ym2151_init();

   if (vgm_player->clock_ym2203)
      ym2203_init();

   if (vgm_player->clock_ym2203 == 0)
      vgm_player->clock_ym2203 = 3000000; // 4000000;
}

#include "./coincoin.h"
#include "./atomic-robot-kid-bgma.h"
#include "./debug_vgm.h"
#include "./ys.h"
#include "./sf2_title.h"
#include "./sf2_ryu.h"

int vgm_init(vgm_player_t *vgm_player)
{
   dbgio_init();
   dbgio_dev_default_init(DBGIO_DEV_VDP2_ASYNC);
   dbgio_dev_font_load();

   smpc_smc_sndon_call();

   // vgm_player->vgm = debug_2151_vgm;
   // vgm_player->vgm = atomic_kid;
   // vgm_player->vgm = ys_vgm;
   //vgm_player->vgm = sf2_ryu;
   vgm_player->vgm = sf2_title;
   vgm_parse_header(vgm_player);

   vgm_player->sampled = 0;
   vgm_player->frame_size = -1;
   vgm_player->sample_count = 0;
   vgm_player->cycles = 0;
   vgm_player->cycles_played = 0;

   dbgio_printf("vgm_player->vgmloopoffset 0x%x\n", vgm_player->vgmloopoffset);

   return 0;
}
