#include "vdp1.config.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

static vdp1_cmdt_t *cmdt = NULL;
vdp1_cmdt_list_t *cmdt_list = NULL;

void vdp1_init()
{
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
}