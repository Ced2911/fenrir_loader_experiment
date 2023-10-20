#include <stdint.h>
#include "../include/mikmod.h"

extern uint32_t const _stack_start;
extern uint32_t const _stack_end;

volatile uint32_t *r = 0x1000;

void _entry_point(void)
{
    malloc_init();

    uint8_t *d = malloc(0x1000);

    d += 0x1000;
    *d = 1;

    uint32_t *t = 0x2000;
    *t = d;
}

__attribute__((externally_visible)) void intCB()
{
}