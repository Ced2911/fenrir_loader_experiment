#pragma once
#include <stdlib.h>
#include <ctype.h>
#include <yaul.h>

static inline void __always_inline
bios_cd_init(void)
{
    __register uint32_t *bios_address;
    bios_address = (uint32_t *)0x0600029c;

    ((void (*)(void)) * bios_address)();
}

static inline void __always_inline
bios_cd_read(void)
{
    __register uint32_t *bios_address;
    bios_address = (uint32_t *)0x060002cc;

    ((void (*)(void)) * bios_address)();
}

static inline int __always_inline
bios_cd_boot(void)
{
    __register uint32_t *bios_address;
    bios_address = (uint32_t *)0x06000288;

    return ((int (*)(void)) * bios_address)();
}


// smpc value cached by bios
#define bios_get_region_flag() (*(volatile uint32_t *)0x06000248)
#define bios_get_smpc_region() (bios_get_region_flag() >> 12)

