
#include <yaul.h>
#include "assets/vdp2.h"
#include "ui.config.h"
#include "vdp2.config.h"
#include "vdp2.h"

static vdp2_scrn_cell_format_t format_nbg0 = {
    .scroll_screen = VDP2_SCRN_NBG0,
    .cc_count = VDP2_SCRN_CCC_PALETTE_256,
    .character_size = 1 * 1,
    .pnd_size = 2,
    .auxiliary_mode = 0,
    .plane_size = 1 * 1,
    .cp_table = NBG0_CELL_ADDR,
    .color_palette = NBG0_COLOR_ADDR};

static vdp2_scrn_cell_format_t format_nbg2 = {
    .scroll_screen = VDP2_SCRN_NBG2,
    .cc_count = VDP2_SCRN_CCC_PALETTE_256,
    .character_size = 1 * 1,
    .pnd_size = 2,
    .auxiliary_mode = 1,
    .plane_size = 1 * 1,
    .cp_table = NBG2_CELL_ADDR,
    .color_palette = NBG2_COLOR_ADDR};

static vdp2_scrn_cell_format_t format_nbg1 = {
    .scroll_screen = VDP2_SCRN_NBG1,
    .cc_count = VDP2_SCRN_CCC_PALETTE_256,
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
    vdp2_vram_cycp_t vram_cycp;

    vram_cycp.pt[0].t0 = VDP2_VRAM_CYCP_PNDR_NBG0;
    vram_cycp.pt[0].t1 = VDP2_VRAM_CYCP_PNDR_NBG1;
    vram_cycp.pt[0].t2 = VDP2_VRAM_CYCP_PNDR_NBG2;
    vram_cycp.pt[0].t3 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[0].t4 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[0].t5 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[0].t6 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[0].t7 = VDP2_VRAM_CYCP_NO_ACCESS;

    vram_cycp.pt[1].t0 = VDP2_VRAM_CYCP_CHPNDR_NBG0;
    vram_cycp.pt[1].t1 = VDP2_VRAM_CYCP_CHPNDR_NBG0;
    vram_cycp.pt[1].t2 = VDP2_VRAM_CYCP_CHPNDR_NBG1;
    vram_cycp.pt[1].t3 = VDP2_VRAM_CYCP_CHPNDR_NBG1;
    vram_cycp.pt[1].t4 = VDP2_VRAM_CYCP_CHPNDR_NBG2;
    vram_cycp.pt[1].t5 = VDP2_VRAM_CYCP_CHPNDR_NBG2;
    vram_cycp.pt[1].t6 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[1].t7 = VDP2_VRAM_CYCP_NO_ACCESS;

    vram_cycp.pt[2].t0 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[2].t1 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[2].t2 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[2].t3 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[2].t4 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[2].t5 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[2].t6 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[2].t7 = VDP2_VRAM_CYCP_NO_ACCESS;

    vram_cycp.pt[3].t0 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[3].t1 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[3].t2 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[3].t3 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[3].t4 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[3].t5 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[3].t6 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[3].t7 = VDP2_VRAM_CYCP_NO_ACCESS;

    vdp2_vram_cycp_set(&vram_cycp);
}

void vdp2_init()
{
    vdp2_setup_vram();

    vdp2_ngb0_init();
    vdp2_ngb1_init();
    vdp2_ngb2_init();

    vdp_dma_enqueue((void*)0x25E00000UL, vmem, vmem_sz);
    vdp_dma_enqueue(VDP2_CRAM_ADDR(0), cmem, cmem_sz);

    vdp2_scrn_display_set(VDP2_SCRN_NBG0_DISP | VDP2_SCRN_NBG1_DISP | VDP2_SCRN_NBG2_DISP);
}