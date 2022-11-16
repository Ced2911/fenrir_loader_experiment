#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
typedef uint32_t fix16_t;
typedef uint16_t rgb1555_t;

#include "/workspaces/loader_yaul/src/ui.config.h"

#define D_U16(x)

void main()
{
    ui_config_t * ui_config = (ui_config_t *)malloc(512);
    FILE *fd = fopen("../theme.bin", "rb");
    printf("sizeof stuct: %d\n", sizeof(ui_config_t));
    fread(ui_config, 512, 1, fd);

    // main font
    printf("%s: %d\n", "ui_config->version", ui_config->version);
    printf("%s: %d\n", "ui_config->size", ui_config->size);

    // vdp2 cfg
    printf("%s: %08x\n", "ui_config->vdp2.nbg0.cell_addr", ui_config->vdp2.nbg0.cell_addr);
    printf("%s: %08x\n", "ui_config->vdp2.nbg1.cell_addr", ui_config->vdp2.nbg1.cell_addr);
    printf("%s: %08x\n", "ui_config->vdp2.nbg2.cell_addr", ui_config->vdp2.nbg2.cell_addr);
    printf("%s: %08x\n", "ui_config->vdp2.nbg3.cell_addr", ui_config->vdp2.nbg3.cell_addr);

    // printf("%s: %d\n", "ui_config->main_font.char_spacing", ui_config->main_font.char_spacing);
    // printf("%s: %d\n", "ui_config->main_font.data", ui_config->main_font.data);

    // screen
    printf("%s: %d\n", "ui_config->screens.gamelist.foreground.pattern_offset", ui_config->screens.gamelist.foreground.pattern_offset);

    printf("%s: %d\n", "ui_config->screens.error_bad_filesystem.foreground.pattern_offset", ui_config->screens.error_bad_filesystem.foreground.pattern_offset);

    // gamelist browser
    printf("%s: %d\n", "ui_config->screens.error_no_sd.background.velocity_x", ui_config->screens.error_no_sd.background.velocity_x);
    printf("%s: %d\n", "ui_config->screens.error_no_sd.background.velocity_y", ui_config->screens.error_no_sd.background.velocity_y);

    // gamelist browser
    printf("%s: %d\n", "ui_config->screens.gamelist.browser.x", ui_config->screens.gamelist.browser.x);
    printf("%s: %d\n", "ui_config->screens.gamelist.browser.y", ui_config->screens.gamelist.browser.y);
    printf("%s: %d\n", "ui_config->screens.gamelist.browser.line_height", ui_config->screens.gamelist.browser.line_height);
    printf("%s: %d\n", "ui_config->screens.gamelist.browser.position_bar.enabled", ui_config->screens.gamelist.browser.position_bar.enabled);
    printf("%s: %d\n", "ui_config->screens.gamelist.browser.position_bar.w", ui_config->screens.gamelist.browser.position_bar.w);
    printf("%s: %d\n", "ui_config->screens.gamelist.browser.position_bar.h", ui_config->screens.gamelist.browser.position_bar.h);
    printf("%s: %d\n", "ui_config->screens.gamelist.browser.position_bar.color", ui_config->screens.gamelist.browser.position_bar.color);
    printf("%s: %d\n", "ui_config->screens.gamelist.browser.position_bar.gouraud[0]", ui_config->screens.gamelist.browser.position_bar.gouraud[0]);
    printf("%s: %d\n", "ui_config->screens.gamelist.browser.position_bar.gouraud[1]", ui_config->screens.gamelist.browser.position_bar.gouraud[1]);
    printf("%s: %d\n", "ui_config->screens.gamelist.browser.position_bar.gouraud[2]", ui_config->screens.gamelist.browser.position_bar.gouraud[2]);
    printf("%s: %d\n", "ui_config->screens.gamelist.browser.position_bar.gouraud[3]", ui_config->screens.gamelist.browser.position_bar.gouraud[3]);

    printf("%s: %d\n", "ui_config->screens.gamelist.background.velocity_x", ui_config->screens.gamelist.background.velocity_x);
    printf("%s: %d\n", "ui_config->screens.gamelist.background.velocity_y", ui_config->screens.gamelist.background.velocity_y);
    printf("%s: %d\n", "ui_config->screens.gamelist.foreground.velocity_x", ui_config->screens.gamelist.foreground.velocity_x);
    printf("%s: %d\n", "ui_config->screens.gamelist.foreground.velocity_y", ui_config->screens.gamelist.foreground.velocity_y);

    // gamelist cover
    printf("%s: %d\n", "ui_config->screens.gamelist.cover.x", ui_config->screens.gamelist.cover.x);
    printf("%s: %d\n", "ui_config->screens.gamelist.cover.y", ui_config->screens.gamelist.cover.y);
    printf("%s: %d\n", "ui_config->screens.gamelist.cover.w", ui_config->screens.gamelist.cover.w);
    printf("%s: %d\n", "ui_config->screens.gamelist.cover.h", ui_config->screens.gamelist.cover.h);
    printf("%s: %d\n", "ui_config->screens.gamelist.cover.enabled", ui_config->screens.gamelist.cover.enabled);
    printf("%s: %d\n", "ui_config->screens.gamelist.cover.noise_palettes[0]", ui_config->screens.gamelist.cover.noise_palettes[0]);
    printf("%s: %d\n", "ui_config->screens.gamelist.cover.noise_palettes[5]", ui_config->screens.gamelist.cover.noise_palettes[5]);
    printf("%s: %d\n", "ui_config->screens.gamelist.cover.noise_palettes[10]", ui_config->screens.gamelist.cover.noise_palettes[10]);
    printf("%s: %d\n", "ui_config->screens.gamelist.cover.noise_palettes[15]", ui_config->screens.gamelist.cover.noise_palettes[15]);

    // ressource
    printf("%s: %d\n", "offsetof ui_config->ressource_count", offsetof(ui_config_t, ressource_count));
    printf("%s: %d\n", "offsetof ui_config->ressources", offsetof(ui_config_t, ressources));
    printf("%s: %d\n", "offsetof ui_config->ressources", offsetof(ui_config_ressources_t, type));

    printf("%s: %d\n", "ui_config->ressource_count", ui_config->ressource_count);
    for (int i = 0; i < ui_config->ressource_count; i++)
    {
        printf("ui_config->ressources[%d].type: %d\n", i, ui_config->ressources[i].type);
        printf("ui_config->ressources[%d].offset: %d\n", i, ui_config->ressources[i].offset);
        printf("ui_config->ressources[%d].lenght: %d\n", i, ui_config->ressources[i].lenght);
    }

    fclose(fd);
}
