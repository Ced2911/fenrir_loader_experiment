#include <yaul.h>
#include "lz4/lz4.h"
#include "theme.h"
#include "./font.sfa.h"
#include "./vdp2.sfz3.h"
#include "theme.config.h"

#include "sound_driver/pcm.h"

static vdp2_scrn_cell_format_t format_nbg0 = {
    .scroll_screen = VDP2_SCRN_NBG0,
    .ccc = VDP2_SCRN_CCC_PALETTE_16,
    //.ccc = VDP2_SCRN_CCC_PALETTE_256,
    .char_size = VDP2_SCRN_CHAR_SIZE_1X1,
    .pnd_size = 2,
    .aux_mode = 1,
    .plane_size = VDP2_SCRN_PLANE_SIZE_1X1,
    .cpd_base = NBG0_CELL_ADDR,
    /*
    .usage_banks = {
        .a0 = VDP2_VRAM_USAGE_TYPE_COEFF_TBL,
        .a1 = VDP2_VRAM_USAGE_TYPE_NONE,
        .b0 = VDP2_VRAM_USAGE_TYPE_NONE,
        .b1 = VDP2_VRAM_USAGE_TYPE_NONE},
        */
    .palette_base = NBG0_COLOR_ADDR};

static vdp2_scrn_cell_format_t format_nbg2 = {
    .scroll_screen = VDP2_SCRN_NBG2,
    .ccc = VDP2_SCRN_CCC_PALETTE_16,
    //.ccc = VDP2_SCRN_CCC_PALETTE_256,
    .char_size = VDP2_SCRN_CHAR_SIZE_1X1,
    .pnd_size = 2,
    .aux_mode = 1,
    .plane_size = VDP2_SCRN_PLANE_SIZE_1X1,
    .cpd_base = NBG2_CELL_ADDR,
    .palette_base = NBG2_COLOR_ADDR};

static vdp2_scrn_cell_format_t format_nbg1 = {
    .scroll_screen = VDP2_SCRN_NBG1,
    .ccc = VDP2_SCRN_CCC_PALETTE_16,
    .char_size = VDP2_SCRN_CHAR_SIZE_1X1,
    .pnd_size = 1,
    .aux_mode = 0,
    .plane_size = VDP2_SCRN_PLANE_SIZE_1X1,
    .cpd_base = NBG1_CELL_ADDR,
    .palette_base = NBG1_COLOR_ADDR};

static void set_plane_addr(vdp2_scrn_normal_map_t *planes, uintptr_t plan_a_addr, size_t pattern_sz)
{

    switch (pattern_sz)
    {
    case 16384:
    default:
        planes->plane_a = plan_a_addr;
        planes->plane_b = plan_a_addr;
        planes->plane_c = plan_a_addr;
        planes->plane_d = plan_a_addr;
        break;
    }
}

static void vdp2_ngb0_init()
{
    vdp2_scrn_normal_map_t nbg0_map;
    set_plane_addr(&nbg0_map,
                   NBG0_PATTERN_ADDR,
                   ui_config.background.pattern_sz);

    vdp2_scrn_cell_format_set(&format_nbg0, &nbg0_map);
    vdp2_scrn_priority_set(VDP2_SCRN_NBG0, 2);
    vdp2_cram_offset_set(VDP2_SCRN_NBG0, format_nbg0.palette_base);
}

static void vdp2_ngb1_init()
{
    const vdp2_scrn_normal_map_t nbg1_map = {
        .plane_a = NBG1_PATTERN_ADDR,
        .plane_b = NBG1_PATTERN_ADDR,
        .plane_c = NBG1_PATTERN_ADDR,
        .plane_d = NBG1_PATTERN_ADDR};
    vdp2_scrn_cell_format_set(&format_nbg1, &nbg1_map);
    vdp2_scrn_priority_set(VDP2_SCRN_NBG1, 3);
    vdp2_cram_offset_set(VDP2_SCRN_NBG1, format_nbg1.palette_base);
}

static void vdp2_ngb2_init()
{

    vdp2_scrn_normal_map_t nbg2_map;
    set_plane_addr(&nbg2_map,
                   NBG2_PATTERN_ADDR,
                   ui_config.foreground.pattern_sz);

    vdp2_scrn_cell_format_set(&format_nbg2, &nbg2_map);
    vdp2_scrn_priority_set(VDP2_SCRN_NBG2, 4);
    vdp2_cram_offset_set(VDP2_SCRN_NBG2, 0);
    vdp2_cram_offset_set(VDP2_SCRN_NBG2, format_nbg2.palette_base);
}

static void vdp2_setup_vram()
{
    __setup_vdp2_cycles();
}

void theme_init_vdp()
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

    vdp2_scrn_display_set(VDP2_SCRN_DISPTP_NBG0 | VDP2_SCRN_DISPTP_NBG1 | VDP2_SCRN_DISPTP_NBG2);
}

ui_config_t *theme_get_ui_config()
{
    return &ui_config;
}

void theme_init_bgm()
{
}
