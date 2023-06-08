
#include <yaul.h>
#include "vdp2.config.h"
#include "vdp2.h"
#include "theme.h"

const vdp2_scrn_bitmap_format_t format_ngb0 = {
#if 0 && defined(FENRIR_480i)
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

const vdp2_scrn_cell_format_t format_cell_nbg1 = {
	.scroll_screen = VDP2_SCRN_NBG1,
	.ccc = VDP2_SCRN_CCC_PALETTE_16,
	.plane_size = VDP2_SCRN_PLANE_SIZE_1X1,
	.char_size = VDP2_SCRN_CHAR_SIZE_1X1,
	.aux_mode = VDP2_SCRN_AUX_MODE_0,
	.pnd_size = 1,

	.palette_base = VDP2_CRAM_ADDR(0x000),
	.cpd_base = VDP2_VRAM_ADDR(0, 0x00000),
};

const vdp2_scrn_normal_map_t normal_cell_nbg1 = {
	.plane_a = NBG1_BITMAP_ADDR,
	.plane_b = NBG1_BITMAP_ADDR,
	.plane_c = NBG1_BITMAP_ADDR,
	.plane_d = NBG1_BITMAP_ADDR,
};

const vdp2_scrn_cell_format_t format_cell_nbg2 = {
	.scroll_screen = VDP2_SCRN_NBG2,
	.ccc = VDP2_SCRN_CCC_PALETTE_16,
	.plane_size = VDP2_SCRN_PLANE_SIZE_1X1,
	.char_size = VDP2_SCRN_CHAR_SIZE_1X1,
	.aux_mode = VDP2_SCRN_AUX_MODE_0,
	.pnd_size = 1,

	.palette_base = VDP2_CRAM_ADDR(0x100),
	.cpd_base = VDP2_VRAM_ADDR(0, 0x10000),
};

const vdp2_scrn_normal_map_t normal_cell_nbg2 = {
	.plane_a = NBG2_BITMAP_ADDR,
	.plane_b = NBG2_BITMAP_ADDR,
	.plane_c = NBG2_BITMAP_ADDR,
	.plane_d = NBG2_BITMAP_ADDR,
};

extern unsigned char theme_bin[];

void vdp2_tx_cell(uint8_t *vdp2_dat, vdp2_scrn_cell_format_t *cell, vdp2_scrn_normal_map_t *screen_map)
{
	uint8_t map_sz = vdp2_dat[0];
	uint8_t plane = vdp2_dat[1];
	uint8_t page = vdp2_dat[2];
	uint8_t tile = vdp2_dat[3];
	uint8_t bpp = vdp2_dat[4];
	uint8_t __ = vdp2_dat[5];
	uint8_t tile_cnt_h = vdp2_dat[6];
	uint8_t tile_cnt_l = vdp2_dat[7];

	uint16_t tile_cnt = tile_cnt_h << 8 | tile_cnt_l;

	uint16_t *ptr = (uint16_t *)vdp2_dat;
	uint16_t *cram = (uint16_t *)cell->palette_base;
	uint16_t *map = (uint16_t *)screen_map->plane_a;

	uint8_t *tiles = cell->cpd_base;

	{
		uint16_t *ptr = (uint16_t *)(vdp2_dat + 0x80);
		// upload palettes
		for (int i = 0; i < 16; i++)
		{
			cram[i] = *ptr++;
		}
	}
	{
		uint16_t *ptr = (uint16_t *)(vdp2_dat + 0x100);
		// upload map
		for (int i = 0; i < VDP2_SCRN_PAGE_SIZE_CALCULATE(cell); i++)
		{
			map[i] = *ptr++;
			// map[i] = i * 4;
		}
	}
	{
		// upload tiles
		uint8_t *ptr = (uint8_t *)(vdp2_dat + 0x100 + (VDP2_SCRN_PAGE_SIZE_CALCULATE(cell)));
		uint8_t *p_tile = tiles;
		for (int i = 0; i < tile_cnt * tile * tile; i++)
		{
			*p_tile++ = *ptr++;
		}
	}

	vdp2_scrn_cell_format_set(cell, screen_map);
	vdp2_cram_offset_set(cell->scroll_screen, 0);
}

void vdp2_bg_cell()
{
	uint32_t sz;
	uint8_t *vdp2_dat = theme_get_ressource(theme_bin, THEME_ID_VDP2_BG, &sz);
	if (vdp2_dat == NULL)
	{
		return;
	}

	vdp2_tx_cell(vdp2_dat, &format_cell_nbg1, &normal_cell_nbg1);
	vdp2_scrn_priority_set(format_cell_nbg2.scroll_screen, 6);
}

void vdp2_fg_cell()
{
	uint32_t sz;
	uint8_t *vdp2_dat = theme_get_ressource(theme_bin, THEME_ID_VDP2_FG, &sz);
	if (vdp2_dat == NULL)
	{
		return;
	}

	vdp2_tx_cell(vdp2_dat, &format_cell_nbg2, &normal_cell_nbg2);
	vdp2_scrn_priority_set(format_cell_nbg2.scroll_screen, 7);
}

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

	vdp2_scrn_cell_format_set(&format_cell_nbg1, &normal_cell_nbg1);
	vdp2_scrn_priority_set(format_cell_nbg1.scroll_screen, 2);
	vdp2_cram_offset_set(format_cell_nbg1.scroll_screen, 0);

	vdp2_scrn_cell_format_set(&format_cell_nbg2, &normal_cell_nbg2);
	vdp2_scrn_priority_set(format_cell_nbg2.scroll_screen, 3);
	vdp2_cram_offset_set(format_cell_nbg2.scroll_screen, 0);
}
