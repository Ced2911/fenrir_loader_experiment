#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <yaul.h>

// used by custom emulator
void emu_printf(const char *format, ...)
{
    static char emu_printf_buffer[256];
    char *s = emu_printf_buffer;
    volatile uint8_t *addr = (volatile uint8_t *)CS1(0x1000);
    va_list args;

    va_start(args, format);
    (void)vsnprintf(emu_printf_buffer, 256, format, args);
    va_end(args);

    while (*s)
        *addr = (uint8_t)*s++;
}