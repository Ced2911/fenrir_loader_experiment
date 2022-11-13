#include "screen.h"

static void error_init()
{
    /*****************************************************
     * themes
     ****************************************************/
}

static void error_update()
{
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
