#include <stdint.h>

#define BRAM_ADDR ((volatile uint16_t *)(0x00180000))

void bram_write_u8(uint8_t *src, uint32_t len, uint32_t offset)
{
    volatile uint16_t *bram = BRAM_ADDR + offset;

    for (int i = 0; i < len; i++)
    {
        *bram++ = *src++ | 0xFF00;
    }
}

void bram_read_u8(uint8_t *dst, uint32_t len, uint32_t offset)
{
    volatile uint16_t *bram = BRAM_ADDR + offset;

    // take only odd bytes, even are 0xFF
    for (int i = 0; i < len; i++)
    {
        *dst++ = ((*bram++)) & 0xFF;
    }
}
