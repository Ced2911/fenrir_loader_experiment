#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "vgm.h"
#include "sinepatt.h"
#include "scsp.h"

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
        // printf("unknown cmd at 0x%x: 0x%x\n", vgm_player->vgmpos, vgm_player->vgm[vgm_player->vgmpos]);
        vgm_player->vgmpos++;
        break;
    }

    // printf("command: %x\nwait:%d\n", command, wait);

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
    vgm_parse_header(vgm_player);

    vgm_player->sampled = 0;
    vgm_player->frame_size = -1;
    vgm_player->sample_count = 0;

    return 0;
}

uint8_t ym2203_regs[256];

void ym2203_init()
{
    // copy sinetable
    uint32_t addr = (0x1000);
    memcpy((void *)(SCSP_RAM + addr), SINEPATT_BIN, SINEPATT_BIN_len);
    memcpy((void *)(SCSP_RAM + addr + SINEPATT_BIN_len), SINEPATT_BIN, SINEPATT_BIN_len);
    memcpy((void *)(SCSP_RAM + addr + SINEPATT_BIN_len + SINEPATT_BIN_len), SINEPATT_BIN, SINEPATT_BIN_len);

    memset(ym2203_regs, 0, 256);
    // init slots...
    scsp_slot_regs_t *slots = get_scsp_slot(0);
    for (int i = 0; i < 32; i++)
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
        slots[i].raw[0] = addr << 16 + 0x130;
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
    }
}

/* http://www.vgmpf.com/Wiki/images/f/f5/YM2203_-_Manual.pdf **/
#define OPN_SLOT(N) ((N >> 2) & 3)
void opn_write_mode(uint8_t r, uint8_t v)
{
    uint8_t chan;
    uint8_t slot;
    uint8_t op;
    chan = v & 0x03;
    int sl = OPN_SLOT(r);

    scsp_slot_regs_t *slots = get_scsp_slot(chan);
    scsp_slot_regs_t *slot0 = &slots[0];
    scsp_slot_regs_t *slot1 = &slots[1];
    scsp_slot_regs_t *slot2 = &slots[2];
    scsp_slot_regs_t *slot3 = &slots[3];

    //
    switch (r)
    {
    case 0x28: /* key on / off */
        if (chan > 2)
            break;

        slot0->kyonex = v & 0x10 ? 1 : 0;
        slot1->kyonex = v & 0x20 ? 1 : 0;
        slot2->kyonex = v & 0x30 ? 1 : 0;
        slot3->kyonex = v & 0x40 ? 1 : 0;

        //         [$1003E0].b=$10                     ; let 'er rip
        break;
    case 0x40 ... 0x4f: /* set total level */
        if (ym2203_regs[r] == v)
            break;
        slots[sl].disdl = v << 1;
        break;
    case 0x50 ... 0x5f: /* set attack rate & key scale */
        if (ym2203_regs[r] == v)
            break;
        slots[sl].attack_rate = v & 31;
        break;
    case 0x60 ... 0x6f: /* set decay rate */
        if (ym2203_regs[r] == v)
            break;
        slots[sl].d1r = v & 31;
        break;
    case 0x70 ... 0x7f: /* set sustain rate */
        if (ym2203_regs[r] == v)
            break;
        slots[sl].d2r = v & 31;
        break;
    case 0x80 ... 0x8f: /* set release rate  */
        if (ym2203_regs[r] == v)
            break;

        // slots[sl].release_r = v & 31;
        //  sreg=_+$0A > [sreg].w=y and $F0 shl 1+(y and 15) shl 1
        slots[sl].raw[5] = ((r & 0xF0) << 1) + ((r & 15) << 1);
        break;
    case 0xa0 ... 0xaf:
        if (ym2203_regs[r] == v)
            break;

    case 0xb0 ... 0xbf:
        break;
    default:
        break;
    }
}

void ym2203_w(uint8_t aa, uint8_t dd)
{
    if (aa & 1)
    {
    }
    else
    {
        opn_write_mode(aa, dd);
    }
    ym2203_regs[aa] = dd;
}
