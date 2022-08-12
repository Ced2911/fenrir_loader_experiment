
#include <yaul.h>
#include "ui.config.h"
#include "vdp2.config.h"

static void vdp2_ngb0_init()
{
    const vdp2_scrn_cell_format_t format = {
        .scroll_screen = VDP2_SCRN_NBG0,
        .cc_count = VDP2_SCRN_CCC_PALETTE_256,
        .character_size = 1 * 1,
        .pnd_size = 1,
        .auxiliary_mode = 1,
        .plane_size = 1 * 1,
        .cp_table = NBG0_CELL_ADDR,
        .color_palette = NBG0_COLOR_ADDR,
        .map_bases = {
            .planes = {
                NGB0_PATTERN_ADDR, NGB0_PATTERN_ADDR + 0x02000, NGB0_PATTERN_ADDR + 0x00000, NGB0_PATTERN_ADDR + 0x02000}}};

    vdp_dma_enqueue((void *)NBG0_COLOR_ADDR,
                    (void *)ui_config.screens.gamelist.background.pal_addr,
                    ui_config.screens.gamelist.background.pal_sz);

    vdp_dma_enqueue((void *)NBG0_CELL_ADDR,
                    (void *)ui_config.screens.gamelist.background.cell_addr,
                    ui_config.screens.gamelist.background.cell_sz);

    vdp_dma_enqueue((void *)NGB0_PATTERN_ADDR,
                    (void *)ui_config.screens.gamelist.background.pattern_addr,
                    ui_config.screens.gamelist.background.pattern_sz);

    vdp2_scrn_cell_format_set(&format);
    vdp2_scrn_priority_set(VDP2_SCRN_NBG0, 2);
}

static void vdp2_ngb1_init()
{
    const vdp2_scrn_cell_format_t format = {
        .scroll_screen = VDP2_SCRN_NBG1,
        .cc_count = VDP2_SCRN_CCC_PALETTE_256,
        .character_size = 1 * 1,
        .pnd_size = 1,
        .auxiliary_mode = 0,
        .plane_size = 1 * 1,
        .cp_table = NBG1_CELL_ADDR,
        .color_palette = 0,
        .map_bases = {
            .planes = {
                NGB1_PATTERN_ADDR, NGB1_PATTERN_ADDR, NGB1_PATTERN_ADDR, NGB1_PATTERN_ADDR}}};

    vdp2_scrn_cell_format_set(&format);
    vdp2_scrn_priority_set(VDP2_SCRN_NBG1, 3);
    vdp2_cram_offset_set(VDP2_SCRN_NBG1, VDP2_CRAM_ADDR(0x100));
}

void vdp2_init()
{
    vdp2_vram_cycp_t vram_cycp;

    vram_cycp.pt[0].t0 = VDP2_VRAM_CYCP_PNDR_NBG0;
    vram_cycp.pt[0].t1 = VDP2_VRAM_CYCP_PNDR_NBG0;
    vram_cycp.pt[0].t2 = VDP2_VRAM_CYCP_PNDR_NBG1;
    vram_cycp.pt[0].t3 = VDP2_VRAM_CYCP_PNDR_NBG1;
    vram_cycp.pt[0].t4 = VDP2_VRAM_CYCP_CHPNDR_NBG1;
    vram_cycp.pt[0].t5 = VDP2_VRAM_CYCP_CHPNDR_NBG1;
    vram_cycp.pt[0].t6 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[0].t7 = VDP2_VRAM_CYCP_NO_ACCESS;

    vram_cycp.pt[1].t0 = VDP2_VRAM_CYCP_CHPNDR_NBG0;
    vram_cycp.pt[1].t1 = VDP2_VRAM_CYCP_CHPNDR_NBG0;
    vram_cycp.pt[1].t2 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[1].t3 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[1].t4 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[1].t5 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[1].t6 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[1].t7 = VDP2_VRAM_CYCP_NO_ACCESS;

    vram_cycp.pt[2].t0 = VDP2_VRAM_CYCP_CPU_RW;
    vram_cycp.pt[2].t1 = VDP2_VRAM_CYCP_CPU_RW;
    vram_cycp.pt[2].t2 = VDP2_VRAM_CYCP_CPU_RW;
    vram_cycp.pt[2].t3 = VDP2_VRAM_CYCP_CPU_RW;
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

    vdp2_ngb0_init();
    vdp2_ngb1_init();
    // vdp2_rgb0_init();

    vdp2_scrn_display_set(VDP2_SCRN_NBG0_DISP | VDP2_SCRN_NBG1_DISP);
}