#pragma once

void bram_write_u8(uint8_t *src, uint32_t len, uint32_t offset);
void bram_read_u8(uint8_t *dst, uint32_t len, uint32_t offset);