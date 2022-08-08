#include <yaul.h>
#include <stdio.h>
#include <stdlib.h>
#include "fenrir/fenrir.h"
#include "vdp1.config.h"
#include "vdp2.config.h"
#include "font/font.h"
#include "screens/gamelist.h"
#include "../assets/bitmap.h"
#include "../assets/test.h"

#define RESOLUTION_WIDTH (352)
#define RESOLUTION_HEIGHT (224)

sd_dir_t *sd_dir;
status_sector_t *status_sector;
sd_dir_entry_t *sd_dir_entries;

void *zalloc(size_t l)
{
    void *ptr = malloc(l);
    memset(ptr, 0, l);
    return ptr;
}

static void copy_bitmap_data(const vdp2_scrn_bitmap_format_t *format)
{
    uint8_t *cpd = (uint8_t *)format->bitmap_pattern;
    vdp_dma_enqueue(cpd, bitmap, bitmapLength / 16);
}

static void copy_palette(const vdp2_scrn_bitmap_format_t *format)
{
    uint16_t *color_palette;
    color_palette = (uint16_t *)format->color_palette;
    vdp_dma_enqueue(color_palette, pal, palLength);
}

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

    vdp_dma_enqueue((void *)NBG0_COLOR_ADDR, pal, palLength);
    vdp_dma_enqueue((void *)NBG0_CELL_ADDR, cells, cellsLength);
    vdp_dma_enqueue((void *)NGB0_PATTERN_ADDR, pattern, patternLength);

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
#if 0
static void vdp2_rgb0_init()
{
#define RBG0_ROTATION_TABLE VDP2_VRAM_ADDR(3, 0x00000)
    const vdp2_scrn_bitmap_format_t format = {
        .scroll_screen = VDP2_SCRN_RBG0,
        .cc_count = VDP2_SCRN_CCC_RGB_32768,
        .bitmap_pattern = RBG0_BITMAP_ADDR,
        .bitmap_size = {.height = 256, .width = 512},
        .color_palette = 0,
        .rp_mode = 0,
        .sf_type = VDP2_SCRN_SF_TYPE_NONE,
        .sf_code = VDP2_SCRN_SF_CODE_A,
        .sf_mode = 0,
        .rotation_table_base = 0,
        .usage_banks = {
            .a0 = VDP2_VRAM_USAGE_TYPE_BPD,
            .a1 = VDP2_VRAM_USAGE_TYPE_BPD,
            .b0 = VDP2_VRAM_USAGE_TYPE_NONE,
            .b1 = VDP2_VRAM_USAGE_TYPE_NONE}};

    vdp2_scrn_rotation_table_t rotation_table = {
        /* Screen start coordinates */
.xst = 0,
    .yst = 0,
    .zst = 0,

    /* Screen vertical coordinate increments (per each line) */
    .delta_xst = FIX16(0.0f),
    .delta_yst = FIX16(1.0f),

    /* Screen horizontal coordinate increments (per each dot) */
    .delta_x = FIX16(1.0f),
    .delta_y = FIX16(0.0f),

    /* Rotation matrix */
    .matrix = {
        .param = {
            .a = FIX16(1.0f),
            .b = FIX16(0.0f),
            .c = FIX16(0.0f),
            .d = FIX16(0.0f),
            .e = FIX16(1.0f),
            .f = FIX16(0.0f)}},

    /* View point coordinates */
    .px = 0, .py = 0, .pz = 0,

    /* Center coordinates */
    .cx = 0, .cy = 0, .cz = 0,

    /* Amount of horizontal shifting */
    .mx = 0, .my = 0,

    /* Scaling coefficients */
    .kx = FIX16(1.0f), .ky = FIX16(1.0f),

    /* Coefficient table start address */
    .kast = 0,
    /* Addr. increment coeff. table (per line) */
    .delta_kast = 0,
    /* Addr. increment coeff. table (per dot) */
    .delta_kax = 0
}
;

vdp_dma_enqueue((void *)RBG0_ROTATION_TABLE,
                &rotation_table,
                sizeof(vdp2_scrn_rotation_table_t));

vdp2_scrn_bitmap_format_set(&format);
vdp2_scrn_priority_set(VDP2_SCRN_RBG0, 4);
}
#endif

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

    vdp2_scrn_display_set(VDP2_SCRN_NBG0_DISP | VDP2_SCRN_NBG1_DISP /* | VDP2_SCRN_RBG0_DISP*/);
}

int main(void)
{
    vdp1_vram_partitions_t vdp1_vram_partitions;
    vdp1_vram_partitions_get(&vdp1_vram_partitions);

    vdp1_init();
    font_to_vram();
    vdp2_init();

    // slave cpu setup
    cpu_dual_comm_mode_set(CPU_DUAL_ENTRY_ICI);

    sd_dir = (sd_dir_t *)zalloc(sizeof(sd_dir_t));
    status_sector = (status_sector_t *)zalloc(sizeof(status_sector_t));
    sd_dir_entries = (sd_dir_entry_t *)zalloc(sizeof(sd_dir_entry_t) * 2500);

    screen_t *screen = &gamelist_screen;
    screen->init();

    while (1)
    {
        screen->update();

        vdp1_sync_render();
        vdp1_sync();
        vdp2_sync();
        vdp1_sync_wait();
        //  vdp2_sync_wait();
    }
    screen->destroy();
}

static void _vblank_out_handler(void *work __unused)
{
    smpc_peripheral_intback_issue();
}

void user_init(void)
{
    static const struct vdp1_env env = {
        .bpp = VDP1_ENV_BPP_16,
        .rotation = VDP1_ENV_ROTATION_0,
        .color_mode = VDP1_ENV_COLOR_MODE_RGB_PALETTE,
        .sprite_type = 0,
        .erase_color = COLOR_RGB1555(0, 0, 0, 0),
        .erase_points = {
            {0, 0},
            {RESOLUTION_WIDTH, RESOLUTION_HEIGHT}}};

    smpc_peripheral_init();
    cd_block_init();

    vdp2_tvmd_display_res_set(VDP2_TVMD_INTERLACE_NONE, VDP2_TVMD_HORZ_NORMAL_A,
                              VDP2_TVMD_VERT_240);

    vdp2_scrn_back_color_set(VDP2_VRAM_ADDR(3, 0x01FFFE),
                             COLOR_RGB1555(0, 0, 0, 0));

    vdp2_sprite_priority_set(0, 6);
    vdp1_env_set(&env);
    vdp1_sync_interval_set(0);

    vdp_sync_vblank_out_set(_vblank_out_handler, NULL);

    cpu_intc_mask_set(0);
    vdp2_tvmd_display_set();
}
