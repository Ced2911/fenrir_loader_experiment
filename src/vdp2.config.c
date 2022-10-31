
#include <yaul.h>
#include "assets/vdp2.h"
#include "ui.config.h"
#include "vdp2.config.h"
#include "vdp2.h"
#include "lz4/lz4.h"

static vdp2_scrn_cell_format_t format_nbg0 = {
    .scroll_screen = VDP2_SCRN_NBG0,
    .cc_count = VDP2_SCRN_CCC_PALETTE_16,
    //.cc_count = VDP2_SCRN_CCC_PALETTE_256,
    .character_size = 1 * 1,
    .pnd_size = 2,
    .auxiliary_mode = 1,
    .plane_size = 1 * 1,
    .cp_table = NBG0_CELL_ADDR,
    /*
    .usage_banks = {
        .a0 = VDP2_VRAM_USAGE_TYPE_COEFF_TBL,
        .a1 = VDP2_VRAM_USAGE_TYPE_NONE,
        .b0 = VDP2_VRAM_USAGE_TYPE_NONE,
        .b1 = VDP2_VRAM_USAGE_TYPE_NONE},
        */
    .color_palette = NBG0_COLOR_ADDR};

static vdp2_scrn_cell_format_t format_nbg2 = {
    .scroll_screen = VDP2_SCRN_NBG2,
    .cc_count = VDP2_SCRN_CCC_PALETTE_16,
    //.cc_count = VDP2_SCRN_CCC_PALETTE_256,
    .character_size = 1 * 1,
    .pnd_size = 2,
    .auxiliary_mode = 1,
    .plane_size = 1 * 1,
    .cp_table = NBG2_CELL_ADDR,
    .color_palette = NBG2_COLOR_ADDR};

static vdp2_scrn_cell_format_t format_nbg1 = {
    .scroll_screen = VDP2_SCRN_NBG1,
    .cc_count = VDP2_SCRN_CCC_PALETTE_16,
    .character_size = 1 * 1,
    .pnd_size = 1,
    .auxiliary_mode = 0,
    .plane_size = 1 * 1,
    .cp_table = NBG1_CELL_ADDR,
    .color_palette = NBG1_COLOR_ADDR,
    .map_bases = {
        .planes = {
            NBG1_PATTERN_ADDR,
            NBG1_PATTERN_ADDR,
            NBG1_PATTERN_ADDR,
            NBG1_PATTERN_ADDR,
        }}};

static void set_plane_addr(vdp2_vram_t *planes, uintptr_t plan_a_addr, size_t pattern_sz)
{
    switch (pattern_sz)
    {
    case 16384:
    default:
        planes[0] = plan_a_addr;
        planes[1] = plan_a_addr;
        planes[2] = plan_a_addr;
        planes[3] = plan_a_addr;
        break;
    }
}

static void vdp2_ngb0_init()
{

    set_plane_addr(format_nbg0.map_bases.planes,
                   NBG0_PATTERN_ADDR,
                   ui_config.screens.gamelist.background.pattern_sz);

    vdp2_scrn_cell_format_set(&format_nbg0);
    vdp2_scrn_priority_set(VDP2_SCRN_NBG0, 2);
    vdp2_cram_offset_set(VDP2_SCRN_NBG0, format_nbg0.color_palette);
}

static void vdp2_ngb1_init()
{
    vdp2_scrn_cell_format_set(&format_nbg1);
    vdp2_scrn_priority_set(VDP2_SCRN_NBG1, 3);
    vdp2_cram_offset_set(VDP2_SCRN_NBG1, format_nbg1.color_palette);
}

static void vdp2_ngb2_init()
{

    set_plane_addr(format_nbg2.map_bases.planes,
                   NBG2_PATTERN_ADDR,
                   ui_config.screens.gamelist.fg.pattern_sz);

    vdp2_scrn_cell_format_set(&format_nbg2);
    vdp2_scrn_priority_set(VDP2_SCRN_NBG2, 4);
    vdp2_cram_offset_set(VDP2_SCRN_NBG2, 0);
    vdp2_cram_offset_set(VDP2_SCRN_NBG2, format_nbg2.color_palette);
}

static void vdp2_setup_vram()
{
    __setup_vdp2_cycles();
}

void vdp2_init()
{
    // vdp2_vram_ctl_t vdp2_vram_ctl = {.vram_mode = VDP2_VRAM_CTL_MODE_NO_PART_BANK_A | VDP2_VRAM_CTL_MODE_PART_BANK_B};
    // vdp2_vram_control_set(&vdp2_vram_ctl);
    MEMORY_WRITE(16, VDP2(RAMCTL), 0x1301);
    /*
    const vdp2_vram_usage_t vram_usage = {
                .a0 = VDP2_VRAM_USAGE_TYPE_CPD,
                .a1 = VDP2_VRAM_USAGE_TYPE_PND,
                .b0 = VDP2_VRAM_USAGE_TYPE_NONE,
                .b1 = VDP2_VRAM_USAGE_TYPE_NONE
        };

        vdp2_vram_usage_set(&vram_usage);
        */
    vdp2_setup_vram();

    vdp2_ngb0_init();
    vdp2_ngb1_init();
    vdp2_ngb2_init();
    
    LZ4_decompress_safe((char *)vmemz, (char *)0x25E00000UL, vmemz_sz, 512 * 1024);
    LZ4_decompress_safe((char *)cmemz, (char *)VDP2_CRAM_ADDR(0), cmemz_sz, 4 * 1024);

    vdp2_scrn_display_set(VDP2_SCRN_NBG0_DISP | VDP2_SCRN_NBG1_DISP | VDP2_SCRN_NBG2_DISP);
}