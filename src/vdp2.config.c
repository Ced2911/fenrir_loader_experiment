
#include <yaul.h>
#include "vdp2.config.h"
#include "vdp2.h"
#include "theme.h"

const vdp2_scrn_bitmap_format_t format_ngb0 = {
#ifdef FENRIR_480i
    .bitmap_size = VDP2_SCRN_BITMAP_SIZE_1024X512,
#else
    .bitmap_size = VDP2_SCRN_BITMAP_SIZE_512X256,
#endif

    .bitmap_base = NBG0_BITMAP_ADDR,
    .scroll_screen = VDP2_SCRN_NBG0,
    .palette_base = VDP2_CRAM_ADDR(0),
    .ccc = VDP2_SCRN_CCC_PALETTE_256,
};

const vdp2_scrn_bitmap_format_t format_nbg1 = {
    .bitmap_size = VDP2_SCRN_BITMAP_SIZE_512X256,
    .bitmap_base = NBG1_BITMAP_ADDR,
    .scroll_screen = VDP2_SCRN_NBG1,
    .palette_base = VDP2_CRAM_ADDR(0x100),
    .ccc = VDP2_SCRN_CCC_PALETTE_16,
};

const vdp2_scrn_bitmap_format_t format_nbg2 = {
    .bitmap_size = VDP2_SCRN_BITMAP_SIZE_512X256,
    .bitmap_base = NBG2_BITMAP_ADDR,
    .scroll_screen = VDP2_SCRN_NBG2,
    .palette_base = VDP2_CRAM_ADDR(0x200),
    .ccc = VDP2_SCRN_CCC_PALETTE_16,
};

static void vdp2_setup_vram()
{
    // __setup_vdp2_cycles();
    vdp2_vram_cycp_t vram_cycp;
    vram_cycp.pt[0].t0 = VDP2_VRAM_CYCP_CHPNDR_NBG0;
    vram_cycp.pt[0].t1 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[0].t2 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[0].t3 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[0].t4 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[0].t5 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[0].t6 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[0].t7 = VDP2_VRAM_CYCP_NO_ACCESS;

    vram_cycp.pt[1].t0 = VDP2_VRAM_CYCP_CHPNDR_NBG1;
    vram_cycp.pt[1].t1 = VDP2_VRAM_CYCP_CHPNDR_NBG2;
    vram_cycp.pt[1].t2 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[1].t3 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[1].t4 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[1].t5 = VDP2_VRAM_CYCP_NO_ACCESS;
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
    // vdp2 screens
    // nbg0 => ui screen
    // nbg1 => bg
    // nbg2 => fg

    vdp2_setup_vram();

    vdp2_scrn_bitmap_format_set(&format_ngb0);
    vdp2_scrn_priority_set(format_ngb0.scroll_screen, 4);
    vdp2_cram_offset_set(format_ngb0.scroll_screen, 0);

    vdp2_scrn_bitmap_format_set(&format_nbg1);
    vdp2_scrn_priority_set(format_nbg1.scroll_screen, 5);
    vdp2_cram_offset_set(format_nbg1.scroll_screen, 0);
    /*
    vdp2_scrn_bitmap_format_set(&format_nbg2);
    vdp2_scrn_priority_set(format_nbg2.scroll_screen, 3);
    vdp2_cram_offset_set(format_nbg2.scroll_screen, 0);
    */
}
