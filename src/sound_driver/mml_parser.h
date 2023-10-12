#include <stdint.h>
#include <yaul.h>

#pragma once

#define NOTE_REST 0
#define NOTE_C 1
#define NOTE_CS 2
#define NOTE_D 3
#define NOTE_DS 4
#define NOTE_E 5
#define NOTE_F 6
#define NOTE_FS 7
#define NOTE_G 8
#define NOTE_GS 9
#define NOTE_A 10
#define NOTE_AS 11
#define NOTE_B 12

#define TRACK_N (4)

typedef struct
{
    uint32_t time_ms;
    uint32_t end_ms;
    uint32_t tempo;
    //
    fix16_t f_tempo;
    fix16_t f_duration;
    //
    uint8_t duration;
    uint8_t l_duration;
    uint8_t slot_n;
    uint8_t note;
    uint8_t oct;
    uint8_t vol;
} mml_track_t;

typedef struct
{
    void *ud;
    char *mml;
    char *p;

    uint32_t vsync_cnt;
    uint32_t time_ms;

    mml_track_t tracks[TRACK_N];

    void (*toneCallback)(void *ud, uint8_t note, uint8_t oct, uint8_t length, uint8_t trk, uint8_t vol);
} mml_parser_ctx_t;

int mml_parser_init(mml_parser_ctx_t *ctx, char *mml);
int mml_parser_play(mml_parser_ctx_t *ctx, int trk_n);
int mml_parser_reset(mml_parser_ctx_t *ctx);
void mml_parser_stop(mml_parser_ctx_t *ctx);
int mml_parser_is_playing(mml_parser_ctx_t *ctx);
