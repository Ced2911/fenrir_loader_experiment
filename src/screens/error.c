#include "screen.h"
#include "yaul.h"

extern void test_0();
extern void test_1();

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
        test_0();
    }
    if (digital.released.button.b)
    {
        test_1();
    }
}

static void error_destroy()
{
}

screen_t error_no_sd = {
    .init = error_init,
    .update = error_update,
    .destroy = error_destroy,
};
screen_t error_bad_filesystem = {
    .init = error_init,
    .update = error_update,
    .destroy = error_destroy,
};
