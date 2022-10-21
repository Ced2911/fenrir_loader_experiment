
#include <yaul.h>
#include "assets/vdp2.h"
#include "ui.config.h"
#include "vdp2.config.h"

static vdp2_scrn_cell_format_t format_nbg0 = {
    .scroll_screen = VDP2_SCRN_NBG0,
    .cc_count = VDP2_SCRN_CCC_PALETTE_256,
    .character_size = 1 * 1,
    .pnd_size = 1,
    .auxiliary_mode = 0,
    .plane_size = 1 * 1,
    .cp_table = NBG0_CELL_ADDR,
    .color_palette = VDP2_CRAM_ADDR(0x200)};

static vdp2_scrn_cell_format_t format_nbg1 = {
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
            NGB1_PATTERN_ADDR,
            NGB1_PATTERN_ADDR,
            NGB1_PATTERN_ADDR,
            NGB1_PATTERN_ADDR,
        }}};

static vdp2_scrn_cell_format_t format_nbg2 = {
    .scroll_screen = VDP2_SCRN_NBG2,
    .cc_count = VDP2_SCRN_CCC_PALETTE_256,
    .character_size = 1 * 1,
    .pnd_size = 1,
    .auxiliary_mode = 1,
    .plane_size = 1 * 1,
    .cp_table = NBG2_CELL_ADDR,
    .color_palette = NBG2_COLOR_ADDR};

static void upload_cells(ui_config_background_t *bg, const vdp2_scrn_cell_format_t *format)
{
    if (bg->cell_addr)
    {
        vdp_dma_enqueue((void *)format->cp_table,
                        (void *)bg->cell_addr,
                        bg->cell_sz);
    }
    if (bg->pal_addr)
    {
        vdp_dma_enqueue((void *)format->color_palette,
                        (void *)bg->pal_addr,
                        bg->pal_sz);
    }
    if (bg->pattern_addr)
    {
        vdp_dma_enqueue((void *)format->map_bases.planes[0],
                        (void *)bg->pattern_addr,
                        bg->pattern_sz);
    }
}

static void set_plane_addr(vdp2_vram_t *planes, uintptr_t plan_a_addr, size_t pattern_sz)
{
    switch (pattern_sz)
    {
    case (64 * 64) * sizeof(short):
        planes[0] = plan_a_addr;
        planes[1] = plan_a_addr;
        planes[2] = plan_a_addr;
        planes[3] = plan_a_addr;
        break;
    case (128 * 64) * sizeof(short):
        planes[0] = plan_a_addr + 0x00000;
        planes[1] = plan_a_addr + 0x02000;
        planes[2] = plan_a_addr + 0x00000;
        planes[3] = plan_a_addr + 0x02000;
        break;
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
                   NGB0_PATTERN_ADDR,
                   ui_config.screens.gamelist.background.pattern_sz);

    upload_cells(&ui_config.screens.gamelist.background, &format_nbg0);

    vdp2_scrn_cell_format_set(&format_nbg0);
    vdp2_scrn_priority_set(VDP2_SCRN_NBG0, 2);
    vdp2_cram_offset_set(VDP2_SCRN_NBG0, VDP2_CRAM_ADDR(0x200));
}

static void vdp2_ngb1_init()
{
    vdp2_scrn_cell_format_set(&format_nbg1);
    vdp2_scrn_priority_set(VDP2_SCRN_NBG1, 3);
    vdp2_cram_offset_set(VDP2_SCRN_NBG1, VDP2_CRAM_ADDR(0x100));
}

static void vdp2_ngb2_init()
{

    set_plane_addr(format_nbg2.map_bases.planes,
                   NGB2_PATTERN_ADDR,
                   ui_config.screens.gamelist.fg.pattern_sz);

    upload_cells(&ui_config.screens.gamelist.fg, &format_nbg2);

    vdp2_scrn_cell_format_set(&format_nbg2);
    vdp2_scrn_priority_set(VDP2_SCRN_NBG2, 4);
    vdp2_cram_offset_set(VDP2_SCRN_NBG2, 0);

    
    vdp2_scrn_scroll_x_set(VDP2_SCRN_NBG2, FIX16(12));
    vdp2_scrn_scroll_y_set(VDP2_SCRN_NBG2, FIX16(0.0f));
}

static void vdp2_setup_vram()
{
    vdp2_vram_cycp_t vram_cycp;

    vram_cycp.pt[0].t0 = VDP2_VRAM_CYCP_PNDR_NBG0;
    vram_cycp.pt[0].t1 = VDP2_VRAM_CYCP_PNDR_NBG1;
    vram_cycp.pt[0].t2 = VDP2_VRAM_CYCP_CHPNDR_NBG1;
    vram_cycp.pt[0].t3 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[0].t4 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[0].t5 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[0].t6 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[0].t7 = VDP2_VRAM_CYCP_NO_ACCESS;

    vram_cycp.pt[1].t0 = VDP2_VRAM_CYCP_CHPNDR_NBG0;
    vram_cycp.pt[1].t1 = VDP2_VRAM_CYCP_CHPNDR_NBG0;
    vram_cycp.pt[1].t2 = VDP2_VRAM_CYCP_CHPNDR_NBG2;
    vram_cycp.pt[1].t3 = VDP2_VRAM_CYCP_CHPNDR_NBG2;
    vram_cycp.pt[1].t4 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[1].t5 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[1].t6 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[1].t7 = VDP2_VRAM_CYCP_NO_ACCESS;

    vram_cycp.pt[2].t0 = VDP2_VRAM_CYCP_PNDR_NBG2;
    vram_cycp.pt[2].t1 = VDP2_VRAM_CYCP_PNDR_NBG2;
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
}

void vdp2_init()
{
    vdp2_setup_vram();

    vdp2_ngb2_init();
    vdp2_ngb0_init();
    vdp2_ngb1_init();

    // vdp2_registers_t * r = vdp2_regs_get();
    //  r->pncn0 = r->pncn0 & 0xFFFB;

    vdp2_scrn_display_set(VDP2_SCRN_NBG0_DISP | VDP2_SCRN_NBG1_DISP | VDP2_SCRN_NBG2_DISP);
}