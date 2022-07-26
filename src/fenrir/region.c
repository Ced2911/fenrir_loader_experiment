#include <stdint.h>
#include <stdio.h>
#include "fenrir.h"

// https://github.com/mamedev/mame/blob/c90a16481de1d1c946e7e3e7d814028cd416d49d/src/mame/sega/saturn.cpp#L481

typedef struct
{
    uint8_t smpc_id;
    uint8_t fenrir_id;
    const char *str;
} regions_map_t;

typedef enum regions
{
    region_japan,
    region_taiwan,
    region_usa,
    region_brazil,
    region_korea,
    region_asiapal,
    region_europe,
    region_latin_america
} regions_t;

static regions_map_t regions[] = {
    {0b0000, region_japan, "Unk.0"},
    {0b1000, region_japan, "Japan"},
    {0b0100, region_korea, "Asia (NTSC)"},
    {0b1100, region_japan, "Unk.3"},
    {0b0010, region_usa, "Usa"},
    {0b1010, region_brazil, "South America (NTSC)"},
    {0b0110, region_taiwan, "Taiwan"},
    {0b1110, region_usa, "Unk.7"},
    {0b0001, region_asiapal, "Unk.8"},
    {0b1001, region_asiapal, "Unk.9"},
    {0b0101, region_asiapal, "Asia (Pal)"},
    {0b1101, region_asiapal, "Unk.B"},
    {0b0011, region_europe, "Europe"},
    {0b1011, region_latin_america, "South America (PAL)"},
    {0b0111, region_europe, "Unk.E"},
    {0b1111, region_europe, "Unk.F"},
    {0, 0, NULL}};

const char *region_get_smpc_string()
{
    uint8_t system_region = bios_get_smpc_region();
    for (regions_map_t *reg = regions; reg->str != NULL; reg++)
    {
        if (reg->smpc_id == system_region)
        {
            return reg->str;
        }
    }

    return regions[0].str;
}

uint8_t region_get_fenrir_id()
{
    uint8_t system_region = bios_get_smpc_region();
    for (regions_map_t *reg = regions; reg->str != NULL; reg++)
    {
        if (reg->smpc_id == system_region)
        {
            return reg->fenrir_id;
        }
    }

    return regions[0].fenrir_id;
}
