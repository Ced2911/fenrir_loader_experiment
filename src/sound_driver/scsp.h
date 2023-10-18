#pragma once
#include <stdint.h>

#define SCSP_RAM (0x25a00000)
#define SCSP_SLOT_ADDR (0x25b00000)

#define SCSP_COMMON_ADDR (0x25b00400)

typedef union scsp_common_reg
{
    uint16_t raw[1];
    uint16_t raw8[2];
} scsp_common_reg_t;

typedef union scsp_slot_regs
{
    uint8_t b[32];
    uint16_t raw[16];
    uint32_t raw32[8];
    struct
    {
        /*
        // 0x00
        uint16_t  _ : 1;
        uint16_t  __ : 1;
        uint16_t  ___ : 1;

        uint16_t  kx : 1;
        uint16_t  kb : 1;
        uint16_t  sbctl : 2;
        uint16_t  ssctl : 2;
        uint16_t  lpctl : 2;
        uint16_t  pcm8b : 1;
        uint16_t  sah : 4;

        // 0x02
        uint16_t sal;
        */
        // 0x00
        uint32_t _ : 1;
        uint32_t __ : 1;
        uint32_t ___ : 1;

        uint32_t kyonex : 1; // 0x10
        uint32_t kyonb : 1;  // 0x08
        uint32_t sbctl : 2;  // 0x06
        uint32_t ssctl : 2;
        uint32_t lpctl : 2;
        uint32_t pcm8b : 1;
        uint32_t sa : 20;

        // 0x04
        uint16_t lsa;

        // 0x06
        uint16_t lea;

        // 0x08
        uint16_t d2r : 5;
        uint16_t d1r : 5;
        uint16_t eghold : 1;
        uint16_t attack_rate : 5;

        // 0x0a
        uint16_t _____ : 1;
        uint16_t loop_start : 1;
        uint16_t kr_scale : 4;
        uint16_t decay_l : 5;
        uint16_t release_r : 5;

        // 0x0c
        uint16_t ______ : 6;
        uint16_t stwinh : 1;
        uint16_t sdir : 1;
        uint16_t total_l : 8;

        // 0x0e
        uint16_t modulation_l : 4;
        uint16_t md_xsl : 6;
        uint16_t md_ysl : 6;

        // 0x10
        uint16_t _______ : 1;
        uint16_t oct : 4;
        uint16_t ________ : 1;
        uint16_t fns : 10;

        // 0x12
        uint16_t lfo_re : 1;
        uint16_t lfo_fq : 5;
        uint16_t plfows : 2;
        uint16_t plfos : 3;
        uint16_t alfows : 2;
        uint16_t alfos : 3;

        // 0x14
        uint16_t _________ : 9;

        uint16_t isel : 4;
        uint16_t imxl : 3;
        // 0x16
        uint16_t disdl : 3;
        uint16_t dipan : 5;
        uint16_t efsdl : 3;
        uint16_t efpan : 5;

        // 0x18 pad
        uint8_t _end[8];
    } __attribute__((packed));
}
__attribute__((aligned(16)))
scsp_slot_regs_t;

#define get_scsp_slot(x) ((volatile scsp_slot_regs_t *)(SCSP_SLOT_ADDR + (x * 0x20)))
#define get_scsp_reg() ((volatile scsp_common_reg_t *)SCSP_COMMON_ADDR)

static inline void scsp_slot_exec(scsp_slot_regs_t *s, uint8_t slot_nb)
{
    volatile scsp_slot_regs_t *slot = get_scsp_slot(slot_nb);

    slot->raw[0] = (s->raw[0] & 0x7FF);
    slot->raw[1] = s->raw[1];
    slot->raw[2] = s->raw[2];
    slot->raw[3] = s->raw[3];
    slot->raw[4] = s->raw[4];
    slot->raw[5] = s->raw[5];
    slot->raw[6] = s->raw[6];
    slot->raw[7] = s->raw[7];
    slot->raw[8] = s->raw[8];
    slot->raw[9] = s->raw[9];
    slot->raw[10] = s->raw[10];
    slot->raw[11] = s->raw[11];
    slot->raw[12] = s->raw[12];
    slot->raw[13] = s->raw[13];
    slot->raw[14] = s->raw[14];
    slot->raw[15] = s->raw[15];

    slot->raw[0] |= (1 << 12) | (1 << 11); // force keyex
}

static inline void scsp_slot_set(scsp_slot_regs_t *s, uint8_t slot_nb)
{
    volatile scsp_slot_regs_t *slot = get_scsp_slot(slot_nb);

    slot->raw[1] = s->raw[1];
    slot->raw[2] = s->raw[2];
    slot->raw[3] = s->raw[3];
    slot->raw[4] = s->raw[4];
    slot->raw[5] = s->raw[5];
    slot->raw[6] = s->raw[6];
    slot->raw[7] = s->raw[7];
    slot->raw[8] = s->raw[8];
    slot->raw[9] = s->raw[9];
    slot->raw[10] = s->raw[10];
    slot->raw[11] = s->raw[11];
    slot->raw[12] = s->raw[12];
    slot->raw[13] = s->raw[13];
    slot->raw[14] = s->raw[14];
    slot->raw[15] = s->raw[15];
    slot->raw[0] = (s->raw[0] & 0xFFF); // disable keyex
}

static inline void scsp_slot_key_off(uint8_t slot_nb)
{
    volatile scsp_slot_regs_t *slot = get_scsp_slot(slot_nb);
    slot->raw[0] = (slot->raw[0] & 0x7FF); // remove keyb
}

static inline void scsp_slot_set_dipan(uint8_t slot_nb, uint8_t pan)
{
    volatile scsp_slot_regs_t *slot = get_scsp_slot(slot_nb);
    slot->dipan = pan;
}

static inline void scsp_slot_set_disdl(uint8_t slot_nb, uint8_t v)
{
    volatile scsp_slot_regs_t *slot = get_scsp_slot(slot_nb);
    slot->disdl = v;
}

static inline void scsp_slot_set_fns_oct(uint8_t slot_nb, uint8_t fns, uint8_t oct)
{
    volatile scsp_slot_regs_t *slot = get_scsp_slot(slot_nb);
    slot->fns = fns;
    slot->oct = oct;
}
