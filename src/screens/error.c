#include "screens.h"
#include "yaul.h"

static void _vbk()
{
    // smpc - input
    smpc_peripheral_intback_issue();
}
static void error_init()
{

    // setup vblank
    vdp_sync_vblank_out_set(_vbk, NULL);
}

static void error_update()
{
    smpc_peripheral_digital_t digital;
    smpc_peripheral_process();
    smpc_peripheral_digital_port(1, &digital);
    if (digital.released.button.a)
    {
        screens_select(screen_error_no_sd);
    }
    if (digital.released.button.b)
    {        
        screens_select(screen_error_bad_filesystem);
    }
}

static void error_destroy()
{
}

screen_t error_no_sd_screen = {
    .init = error_init,
    .update = error_update,
    .destroy = error_destroy,
};
screen_t error_bad_filesystem_screen = {
    .init = error_init,
    .update = error_update,
    .destroy = error_destroy,
};
