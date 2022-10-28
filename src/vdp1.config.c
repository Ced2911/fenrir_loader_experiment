#include "vdp1.config.h"

#define C_RGB(r, g, b) (((b)&0x1F) << 10 | ((g)&0x1F) << 5 | ((r)&0x1F) | 0x8000)

static const uint16_t color_[] = {
    C_RGB(31, 0, 0),
    C_RGB(31, 31, 0),
    C_RGB(31, 0, 31),
    C_RGB(0, 0, 31),
    //
    C_RGB(31, 1, 31),
    C_RGB(31, 1, 31),
    C_RGB(31, 1, 31),
    C_RGB(31, 1, 31),
    //
    C_RGB(2, 2, 2),
    C_RGB(3, 3, 3),
    C_RGB(4, 4, 4),
    C_RGB(5, 5, 5),
    C_RGB(6, 6, 6),
    C_RGB(7, 7, 7),
    C_RGB(8, 8, 8),
    C_RGB(9, 9, 9),
    C_RGB(10, 10, 10),
    C_RGB(11, 11, 11),
    C_RGB(12, 12, 12),
    C_RGB(13, 13, 13),
    C_RGB(14, 14, 14),
    C_RGB(15, 15, 15),
    C_RGB(16, 16, 16),
    C_RGB(17, 17, 17),
    C_RGB(18, 18, 18),
    C_RGB(19, 19, 19),
    C_RGB(20, 20, 20),
    C_RGB(21, 21, 21),
    C_RGB(22, 22, 22),
    C_RGB(23, 23, 23),
    C_RGB(24, 24, 24),
    C_RGB(25, 25, 25),
    C_RGB(26, 26, 26),
    C_RGB(27, 27, 27),
    C_RGB(28, 28, 28),
    C_RGB(29, 29, 29),
    C_RGB(30, 30, 30),
    C_RGB(31, 31, 31),
};

static const color_rgb1555_t palettes[] = {
    COLOR_RGB1555(1, 0x0F, 0, 0x1F),
    COLOR_RGB1555(1, 0x0F, 0, 0x1F),
    COLOR_RGB1555(1, 0x0F, 0, 0x1F),
    COLOR_RGB1555(1, 0x0F, 0, 0x1F),

    COLOR_RGB1555(1, 0, 0x0F, 0x1F),
    COLOR_RGB1555(1, 0, 0x0F, 0x1F),
    COLOR_RGB1555(1, 0, 0x0F, 0x1F),
    COLOR_RGB1555(1, 0, 0x0F, 0x1F),

    COLOR_RGB1555(1, 0x1F, 0x0F, 0),
    COLOR_RGB1555(1, 0x1F, 0x0F, 0),
    COLOR_RGB1555(1, 0x1F, 0x0F, 0),
    COLOR_RGB1555(1, 0x1F, 0x0F, 0),

    COLOR_RGB1555(1, 0x0F, 0x1F, 0),
    COLOR_RGB1555(1, 0x0F, 0x1F, 0),
    COLOR_RGB1555(1, 0x0F, 0x1F, 0),
    COLOR_RGB1555(1, 0x0F, 0x1F, 0),
};

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

static vdp1_cmdt_t *cmdt = NULL;
vdp1_cmdt_list_t *cmdt_list = NULL;

void vdp1_init()
{
    // init command tables
    cmdt_list = vdp1_cmdt_list_alloc(ORDER_COUNT);
    memset(&cmdt_list->cmdts[0], 0x00, sizeof(vdp1_cmdt_t) * ORDER_COUNT);

    vdp1_cmdt_t *cmdts;
    cmdts = &cmdt_list->cmdts[0];

    const int16_vec2_t system_clip_coord =
        INT16_VEC2_INITIALIZER(SCREEN_WIDTH - 1,
                               SCREEN_HEIGHT - 1);

    const int16_vec2_t local_coord_center =
        INT16_VEC2_INITIALIZER(0, 0);

    vdp1_cmdt_system_clip_coord_set(&cmdts[ORDER_SYSTEM_CLIP_COORDS_INDEX]);
    vdp1_cmdt_param_vertex_set(&cmdts[ORDER_SYSTEM_CLIP_COORDS_INDEX],
                               CMDT_VTX_SYSTEM_CLIP, &system_clip_coord);

    vdp1_cmdt_local_coord_set(&cmdts[ORDER_CLEAR_LOCAL_COORDS_INDEX]);
    vdp1_cmdt_param_vertex_set(&cmdts[ORDER_CLEAR_LOCAL_COORDS_INDEX],
                               CMDT_VTX_LOCAL_COORD, &local_coord_center);

    // upload colors

    vdp1_vram_partitions_t vdp1_vram_partitions;
    vdp1_vram_partitions_get(&vdp1_vram_partitions);
    
    vdp_dma_enqueue(vdp1_vram_partitions.gouraud_base, (void *)color_, sizeof(color_) * sizeof(uint16_t));
    vdp_dma_enqueue(vdp1_vram_partitions.clut_base, (void *)palettes, sizeof(palettes) * sizeof(uint16_t));

    
        vdp2_sprite_priority_set(0, 2);
}