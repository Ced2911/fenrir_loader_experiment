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