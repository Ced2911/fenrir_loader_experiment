#include <yaul.h>
#include "lz4/lz4.h"
#include "theme.h"
//#include "./font.sfa.h"
#include "./vdp2.sfz3.h"
#include "theme.config.h"

#include "sound_driver/pcm.h"

vdp2_scrn_cell_format_t format_nbg0 = {
    .scroll_screen = VDP2_SCRN_NBG0,
    .ccc = VDP2_SCRN_CCC_PALETTE_16,
    //.ccc = VDP2_SCRN_CCC_PALETTE_256,
    .char_size = VDP2_SCRN_CHAR_SIZE_1X1,
    .pnd_size = 2,
    .aux_mode = 1,
    .plane_size = VDP2_SCRN_PLANE_SIZE_1X1,
    .cpd_base = NBG0_CELL_ADDR,
    .palette_base = NBG0_COLOR_ADDR};

vdp2_scrn_cell_format_t format_nbg2 = {
    .scroll_screen = VDP2_SCRN_NBG2,
    .ccc = VDP2_SCRN_CCC_PALETTE_16,
    .char_size = VDP2_SCRN_CHAR_SIZE_1X1,
    .pnd_size = 2,
    .aux_mode = 1,
    .plane_size = VDP2_SCRN_PLANE_SIZE_1X1,
    .cpd_base = NBG2_CELL_ADDR,
    .palette_base = NBG2_COLOR_ADDR};

vdp2_scrn_cell_format_t format_nbg1 = {
    .scroll_screen = VDP2_SCRN_NBG1,
    .ccc = VDP2_SCRN_CCC_PALETTE_16,
    .char_size = VDP2_SCRN_CHAR_SIZE_1X1,
    .pnd_size = 1,
    .aux_mode = 0,
    .plane_size = VDP2_SCRN_PLANE_SIZE_1X1,
    .cpd_base = NBG1_CELL_ADDR,
    .palette_base = NBG1_COLOR_ADDR};

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
                        NBG0_PATTERN_ADDR,
                        0);

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
    vdp2_set_plane_addr(&nbg2_map,
                        NBG2_PATTERN_ADDR + (0),
                        0);

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
    MEMORY_WRITE(16, VDP2(RAMCTL), 0x1301);
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
    return ui_config;
}

void theme_init_bgm()
{
}

// a bouger
void dbg_do()
{
    dbgio_printf("sizeof stuct: %d\n", sizeof(ui_config_t));

    // main font
    dbgio_printf("%s: %d\n", "ui_config->version", ui_config->version);
    dbgio_printf("%s: %d\n", "ui_config->size", ui_config->size);
#if 0
        // vdp2 cfg
        dbgio_printf("%s: %08x\n", "ui_config->vdp2.nbg0.cell_addr", ui_config->vdp2.nbg0.cell_addr);
        dbgio_printf("%s: %08x\n", "ui_config->vdp2.nbg1.cell_addr", ui_config->vdp2.nbg1.cell_addr);
        dbgio_printf("%s: %08x\n", "ui_config->vdp2.nbg2.cell_addr", ui_config->vdp2.nbg2.cell_addr);
        dbgio_printf("%s: %08x\n", "ui_config->vdp2.nbg3.cell_addr", ui_config->vdp2.nbg3.cell_addr);

        // dbgio_printf("%s: %d\n", "ui_config->main_font.char_spacing", ui_config->main_font.char_spacing);
        // dbgio_printf("%s: %d\n", "ui_config->main_font.data", ui_config->main_font.data);
#endif
#if 1
    // screen
    dbgio_printf("%s: %d\n", "ui_config->screens.gamelist.foreground.pattern_offset", ui_config->screens.gamelist.foreground.pattern_offset);

    dbgio_printf("%s: %08X\n", "ui_config->screens.error_bad_filesystem.background.pattern_offset", ui_config->screens.error_bad_filesystem.background.pattern_offset);
    dbgio_printf("%s: %08X\n", "ui_config->screens.error_bad_filesystem.foreground.pattern_offset", ui_config->screens.error_bad_filesystem.foreground.pattern_offset);
#endif
#if 0
    // gamelist browser
    dbgio_printf("%s: %d\n", "ui_config->screens.error_no_sd.background.velocity_x", ui_config->screens.error_no_sd.background.velocity_x);
    dbgio_printf("%s: %d\n", "ui_config->screens.error_no_sd.background.velocity_y", ui_config->screens.error_no_sd.background.velocity_y);
#endif
#if 0
        // gamelist browser
        dbgio_printf("%s: %d\n", "ui_config->screens.gamelist.browser.x", ui_config->screens.gamelist.browser.x);
        dbgio_printf("%s: %d\n", "ui_config->screens.gamelist.browser.y", ui_config->screens.gamelist.browser.y);
        dbgio_printf("%s: %d\n", "ui_config->screens.gamelist.browser.line_height", ui_config->screens.gamelist.browser.line_height);
        dbgio_printf("%s: %d\n", "ui_config->screens.gamelist.browser.position_bar.enabled", ui_config->screens.gamelist.browser.position_bar.enabled);
        dbgio_printf("%s: %d\n", "ui_config->screens.gamelist.browser.position_bar.w", ui_config->screens.gamelist.browser.position_bar.w);
        dbgio_printf("%s: %d\n", "ui_config->screens.gamelist.browser.position_bar.h", ui_config->screens.gamelist.browser.position_bar.h);
        dbgio_printf("%s: %d\n", "ui_config->screens.gamelist.browser.position_bar.color", ui_config->screens.gamelist.browser.position_bar.color);
        dbgio_printf("%s: %d\n", "ui_config->screens.gamelist.browser.position_bar.gouraud[0]", ui_config->screens.gamelist.browser.position_bar.gouraud[0]);
        dbgio_printf("%s: %d\n", "ui_config->screens.gamelist.browser.position_bar.gouraud[1]", ui_config->screens.gamelist.browser.position_bar.gouraud[1]);
        dbgio_printf("%s: %d\n", "ui_config->screens.gamelist.browser.position_bar.gouraud[2]", ui_config->screens.gamelist.browser.position_bar.gouraud[2]);
        dbgio_printf("%s: %d\n", "ui_config->screens.gamelist.browser.position_bar.gouraud[3]", ui_config->screens.gamelist.browser.position_bar.gouraud[3]);
#endif
#if 0
    dbgio_printf("%s: %d\n", "ui_config->screens.gamelist.background.velocity_x", ui_config->screens.gamelist.background.velocity_x);
    dbgio_printf("%s: %d\n", "ui_config->screens.gamelist.background.velocity_y", ui_config->screens.gamelist.background.velocity_y);
    dbgio_printf("%s: %d\n", "ui_config->screens.gamelist.foreground.velocity_x", ui_config->screens.gamelist.foreground.velocity_x);
    dbgio_printf("%s: %d\n", "ui_config->screens.gamelist.foreground.velocity_y", ui_config->screens.gamelist.foreground.velocity_y);
#endif
#if 0 // ok
    // gamelist cover
    dbgio_printf("%s: %d\n", "ui_config->screens.gamelist.cover.x", ui_config->screens.gamelist.cover.x);
    dbgio_printf("%s: %d\n", "ui_config->screens.gamelist.cover.y", ui_config->screens.gamelist.cover.y);
    dbgio_printf("%s: %d\n", "ui_config->screens.gamelist.cover.w", ui_config->screens.gamelist.cover.w);
    dbgio_printf("%s: %d\n", "ui_config->screens.gamelist.cover.h", ui_config->screens.gamelist.cover.h);
    dbgio_printf("%s: %d\n", "ui_config->screens.gamelist.cover.enabled", ui_config->screens.gamelist.cover.enabled);
    dbgio_printf("%s: %d\n", "ui_config->screens.gamelist.cover.noise_palettes[0]", ui_config->screens.gamelist.cover.noise_palettes[0]);
    dbgio_printf("%s: %d\n", "ui_config->screens.gamelist.cover.noise_palettes[5]", ui_config->screens.gamelist.cover.noise_palettes[5]);
    dbgio_printf("%s: %d\n", "ui_config->screens.gamelist.cover.noise_palettes[10]", ui_config->screens.gamelist.cover.noise_palettes[10]);
    dbgio_printf("%s: %d\n", "ui_config->screens.gamelist.cover.noise_palettes[15]", ui_config->screens.gamelist.cover.noise_palettes[15]);
#endif
#if 0
    // ressource
    dbgio_printf("%s: %d\n", "offsetof ui_config->ressource_count", offsetof(ui_config_t, ressource_count));
    dbgio_printf("%s: %d\n", "offsetof ui_config->ressources", offsetof(ui_config_t, ressources));
    dbgio_printf("%s: %d\n", "offsetof ui_config->ressources", offsetof(ui_config_ressources_t, type));

    dbgio_printf("%s: %d\n", "ui_config->ressource_count", ui_config->ressource_count);
#endif
#if 0
    for (int i = 0; i < ui_config->ressource_count; i++)
    {
        dbgio_printf("ui_config->ressources[%d].type: %d\n", i, ui_config->ressources[i].type);
        dbgio_printf("ui_config->ressources[%d].offset: %d\n", i, ui_config->ressources[i].offset);
        dbgio_printf("ui_config->ressources[%d].lenght: %d\n", i, ui_config->ressources[i].lenght);
    }
#endif
    dbgio_flush();
}