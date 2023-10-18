#include <stdint.h>

extern uint32_t const _stack_start;
extern uint32_t const _stack_end;
extern uint32_t const _entry_point;
void lead_function(void) // Link start to main
{
    // note that _start is the ASM label that equates to
    // the lead function in this compiler. In a normal
    // compiler, it would be "main".
    __asm__("jmp _start");
}

void _start(void)
{
    int i = 0;
    while (1)
    {
        i++;
    }
}