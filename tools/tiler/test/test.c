#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
typedef uint16_t fix16_t;
typedef uint16_t rgb1555_t;

#include "/workspaces/loader_yaul/src/ui.config.h"

#define D_U16(x)

void main()
{
    ui_config_t ui_config;
    FILE *fd = fopen("../theme.bin", "rb");
    printf("sizeof stuct: %d\n", sizeof(ui_config_t));
    fread(&ui_config, sizeof(ui_config_t), 1, fd);

    // main font
    printf("%s: %d\n", "ui_config.main_font.char_height", ui_config.main_font.char_height);
    printf("%s: %d\n", "ui_config.main_font.char_width", ui_config.main_font.char_width);
    printf("%s: %d\n", "ui_config.main_font.char_spacing", ui_config.main_font.char_spacing);
    printf("%s: %d\n", "ui_config.main_font.data", ui_config.main_font.data);

    // gamelist browser
    printf("%s: %d\n", "ui_config.screens.gamelist.browser.x", ui_config.screens.gamelist.browser.x);
    printf("%s: %d\n", "ui_config.screens.gamelist.browser.y", ui_config.screens.gamelist.browser.y);

    // gamelist cover
    printf("%s: %d\n", "ui_config.screens.gamelist.cover.x", ui_config.screens.gamelist.cover.x);
    printf("%s: %d\n", "ui_config.screens.gamelist.cover.y", ui_config.screens.gamelist.cover.y);
    printf("%s: %d\n", "ui_config.screens.gamelist.cover.w", ui_config.screens.gamelist.cover.w);
    printf("%s: %d\n", "ui_config.screens.gamelist.cover.h", ui_config.screens.gamelist.cover.h);

    fclose(fd);
}
