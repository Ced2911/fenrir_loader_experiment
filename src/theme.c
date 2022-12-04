#include "yaul.h"
#include "theme.h"

static ui_config_t *ui_config;

typedef struct
{
    ui_config_background_t *background;
    ui_config_background_t *foreground;
} current_theme_config_t;

static current_theme_config_t current_theme_config = {
    .background = NULL,
    .foreground = NULL};

#define SET_CURRENT_SCREEN(x)                            \
    {                                                    \
        current_theme_config.foreground = &x.foreground; \
        current_theme_config.background = &x.background; \
        theme_background_config_set(&x.foreground);      \
        theme_background_config_set(&x.background);      \
    }

/*****************************************************
 * vdp init
 ****************************************************/
vdp2_scrn_cell_format_t format_nbg0 = {
    .scroll_screen = VDP2_SCRN_NBG0,
    .ccc = VDP2_SCRN_CCC_PALETTE_16,
    // .ccc = VDP2_SCRN_CCC_PALETTE_256,
    .char_size = VDP2_SCRN_CHAR_SIZE_1X1,
    .pnd_size = 2,
    .aux_mode = 1,
    .plane_size = VDP2_SCRN_PLANE_SIZE_1X1,
    //.cpd_base = NBG0_CELL_ADDR,
    //.palette_base = NBG0_COLOR_ADDR
};

vdp2_scrn_cell_format_t format_nbg2 = {
    .scroll_screen = VDP2_SCRN_NBG2,
    .ccc = VDP2_SCRN_CCC_PALETTE_16,
    //.ccc = VDP2_SCRN_CCC_PALETTE_256,
    .char_size = VDP2_SCRN_CHAR_SIZE_1X1,
    .pnd_size = 2,
    .aux_mode = 1,
    .plane_size = VDP2_SCRN_PLANE_SIZE_1X1,
    //.cpd_base = NBG2_CELL_ADDR,
    //.palette_base = NBG2_COLOR_ADDR
};

vdp2_scrn_cell_format_t format_nbg1 = {
    .scroll_screen = VDP2_SCRN_NBG1,
    .ccc = VDP2_SCRN_CCC_PALETTE_16,
    .char_size = VDP2_SCRN_CHAR_SIZE_1X1,
    .pnd_size = 1,
    .aux_mode = 0,
    .plane_size = VDP2_SCRN_PLANE_SIZE_1X1,
    //.cpd_base = NBG1_CELL_ADDR,
    //.palette_base = NBG1_COLOR_ADDR
};

vdp2_scrn_bitmap_format_t format_rbg0 = {
    .bitmap_size = VDP2_SCRN_BITMAP_SIZE_512X256,
    .bitmap_base = VDP2_VRAM_ADDR(3, 0x00000),
    .palette_base = VDP2_CRAM_ADDR(0),
    .ccc = VDP2_SCRN_CCC_PALETTE_256,
    .scroll_screen = VDP2_SCRN_RBG0};

static void vdp2_set_plane_addr(vdp2_scrn_normal_map_t *planes, uintptr_t plan_a_addr, size_t pattern_sz)
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
    vdp2_set_plane_addr(&nbg0_map,
                        ui_config->vdp2.nbg0.pal_addr,
                        0);

    format_nbg0.cpd_base = ui_config->vdp2.nbg0.cell_addr;
    format_nbg0.palette_base = ui_config->vdp2.nbg0.pal_addr << 1;

    vdp2_scrn_cell_format_set(&format_nbg0, &nbg0_map);
    vdp2_scrn_priority_set(VDP2_SCRN_NBG0, 2);
    vdp2_cram_offset_set(VDP2_SCRN_NBG0, format_nbg0.palette_base);
}

static void vdp2_ngb1_init()
{
    const vdp2_scrn_normal_map_t nbg1_map = {
        .plane_a = ui_config->vdp2.nbg1.pattern_addr,
        .plane_b = ui_config->vdp2.nbg1.pattern_addr,
        .plane_c = ui_config->vdp2.nbg1.pattern_addr,
        .plane_d = ui_config->vdp2.nbg1.pattern_addr};

    format_nbg1.cpd_base = ui_config->vdp2.nbg1.cell_addr;
    format_nbg1.palette_base = ui_config->vdp2.nbg1.pal_addr << 1;

    vdp2_scrn_cell_format_set(&format_nbg1, &nbg1_map);
    vdp2_scrn_priority_set(VDP2_SCRN_NBG1, 6);
    vdp2_cram_offset_set(VDP2_SCRN_NBG1, format_nbg1.palette_base);
}

static void vdp2_ngb2_init()
{

    vdp2_scrn_normal_map_t nbg2_map;
    vdp2_set_plane_addr(&nbg2_map,
                        ui_config->vdp2.nbg2.pal_addr,
                        0);

    format_nbg2.cpd_base = ui_config->vdp2.nbg2.cell_addr;
    format_nbg2.palette_base = ui_config->vdp2.nbg2.pal_addr << 1;

    vdp2_scrn_cell_format_set(&format_nbg2, &nbg2_map);
    vdp2_scrn_priority_set(VDP2_SCRN_NBG2, 4);
    vdp2_cram_offset_set(VDP2_SCRN_NBG2, 0);
    vdp2_cram_offset_set(VDP2_SCRN_NBG2, format_nbg2.palette_base);
}

static void vdp2_rbg0_init()
{

    vdp2_scrn_bitmap_format_set(&format_rbg0);
    vdp2_scrn_priority_set(VDP2_SCRN_RBG0, 1);
    vdp2_scrn_display_set(VDP2_SCRN_DISPTP_RBG0);

    vdp2_cram_offset_set(VDP2_SCRN_RBG0, 0);
}

static void vdp2_setup_vram()
{
    // __setup_vdp2_cycles();
    vdp2_vram_cycp_t vram_cycp;
    vram_cycp.pt[0].t0 = VDP2_VRAM_CYCP_CHPNDR_NBG0;
    vram_cycp.pt[0].t1 = VDP2_VRAM_CYCP_CHPNDR_NBG2;
    vram_cycp.pt[0].t2 = VDP2_VRAM_CYCP_CHPNDR_NBG1;
    vram_cycp.pt[0].t3 = VDP2_VRAM_CYCP_PNDR_NBG0;
    vram_cycp.pt[0].t4 = VDP2_VRAM_CYCP_PNDR_NBG2;
    vram_cycp.pt[0].t5 = VDP2_VRAM_CYCP_PNDR_NBG1;
    vram_cycp.pt[0].t6 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[0].t7 = VDP2_VRAM_CYCP_NO_ACCESS;

    vram_cycp.pt[1].t0 = VDP2_VRAM_CYCP_NO_ACCESS;
    vram_cycp.pt[1].t1 = VDP2_VRAM_CYCP_NO_ACCESS;
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

/*****************************************************
 * theme
 ****************************************************/
void theme_init_vdp()
{
    MEMORY_WRITE(16, VDP2(RAMCTL), 0x1301);
    vdp2_setup_vram();

    vdp2_rbg0_init();
    vdp2_ngb0_init();
    vdp2_ngb1_init();
    vdp2_ngb2_init();

    uint8_t *ressources = ((uint8_t *)ui_config) + 512;
    for (uint32_t i = 0; i < ui_config->ressource_count; i++)
    {
        if (ui_config->ressources[i].type == 1)
        {
            memcpy((char *)0x25E00000UL, ressources + ui_config->ressources[i].offset, ui_config->ressources[i].lenght);
        }
        if (ui_config->ressources[i].type == 2)
        {
            memcpy((char *)VDP2_CRAM_ADDR(0), ressources + ui_config->ressources[i].offset, ui_config->ressources[i].lenght);
        }
    }

    vdp2_scrn_display_set(VDP2_SCRN_DISPTP_RBG0 | VDP2_SCRN_DISPTP_NBG0 | VDP2_SCRN_DISPTP_NBG1 | VDP2_SCRN_DISPTP_NBG2);
   // vdp2_scrn_display_set(VDP2_SCRN_DISPTP_RBG0);
}

void theme_set_pattern(theme_scr_t scr, uint32_t pattern_offset)
{
    vdp2_scrn_normal_map_t map;
    ui_config_t *ui_config = theme_get_ui_config();

    switch (scr)
    {
    case THEME_SCR_NBG0:
        vdp2_set_plane_addr(&map,
                            pattern_offset + ui_config->vdp2.nbg0.pattern_addr,
                            0);
        vdp2_scrn_cell_format_set(&format_nbg0, &map);
        break;
    case THEME_SCR_NBG2:
        vdp2_set_plane_addr(&map,
                            pattern_offset + ui_config->vdp2.nbg2.pattern_addr,
                            0);
        vdp2_scrn_cell_format_set(&format_nbg2, &map);
        break;
    default:
        break;
    }
}

void theme_background_config_set(ui_config_background_t *b)
{
    theme_set_pattern(b->screen, b->pattern_offset);

    // dbgio_printf("unknow %d-%d\n", b->screen, b->pattern_offset);
}

void theme_set_background(screens_type_t scr)
{
    ui_config_t *ui_config = theme_get_ui_config();

    vdp2_scrn_scroll_x_set(VDP2_SCRN_NBG0, 0);
    vdp2_scrn_scroll_y_set(VDP2_SCRN_NBG0, 0);

    vdp2_scrn_scroll_x_set(VDP2_SCRN_NBG2, 0);
    vdp2_scrn_scroll_y_set(VDP2_SCRN_NBG2, 0);

    switch (scr)
    {
    case screen_error_no_sd:
        SET_CURRENT_SCREEN(ui_config->screens.error_no_sd);
        break;
    case screen_gamelist:
        SET_CURRENT_SCREEN(ui_config->screens.gamelist);
        break;
    case screen_error_bad_filesystem:
        SET_CURRENT_SCREEN(ui_config->screens.error_bad_filesystem);
        break;

    default:
        break;
    }
    dbgio_flush();
}

void theme_update()
{
    ui_config_t *ui_config = theme_get_ui_config();
    ui_config_background_t *background = current_theme_config.background;
    ui_config_background_t *foreground = current_theme_config.foreground;

    // animate bg
    if (background && background->velocity_x)
        vdp2_scrn_scroll_x_update(VDP2_SCRN_NBG0, background->velocity_x);
    if (background && background->velocity_y)
        vdp2_scrn_scroll_y_update(VDP2_SCRN_NBG0, background->velocity_y);

    if (foreground && foreground->velocity_x)
        vdp2_scrn_scroll_x_update(VDP2_SCRN_NBG2, foreground->velocity_x);
    if (foreground && foreground->velocity_y)
        vdp2_scrn_scroll_y_update(VDP2_SCRN_NBG2, foreground->velocity_y);
}

void theme_init_bgm()
{
}

/*****************************************************
 * theme loader
 ****************************************************/
#include "themes/demo/theme.demo.h"
static ui_config_t *ui_config = (ui_config_t *)theme;
ui_config_t *theme_get_ui_config()
{
    return ui_config;
}
