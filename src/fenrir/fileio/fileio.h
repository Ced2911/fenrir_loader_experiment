#pragma once
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// init & end internal timer
void fenrir_special_cmd_start();
void fenrir_special_cmd_end();

// low level cmd
int fenrir_special_cmd(uint8_t subcmd, uint8_t arg);
int fenrir_special_cmd_continue(uint8_t arg_0, uint8_t arg_1);
int fenrir_special_cmd_continue_mem(void *data, int len);

/**
 * Todo handle some error ?
 */
static inline int fenrir_openfile(const char *filename)
{
    fenrir_special_cmd(sp_cmd_write_open, strlen(filename));
    fenrir_special_cmd_continue_mem((void *)filename, strlen(filename));
    return 0;
}

/**
 * Todo handle some error ?
 */
int fenrir_writefile(void *data, uint16_t len);

/**
 * Todo handle some error ?
 */
static inline int fenrir_closefile()
{
    fenrir_special_cmd(sp_cmd_write_close, 0);
    return 0;
}

