#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include "mml_parser.h"

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

static void skip_space(mml_parser_ctx_t *ctx)
{
    while (isspace(*ctx->p))
        *ctx->p++;
}

static char get_next_ch(mml_parser_ctx_t *ctx)
{
    char c = *ctx->p;
    ctx->p++;
    return tolower(c);
}
static int get_next_nbr(mml_parser_ctx_t *ctx)
{
    char *next = NULL;
    int l = strtol(ctx->p, &next, 10);
    if (next)
    {
        ctx->p = next;
    }
    return l;
}

int mml_parser_init(mml_parser_ctx_t *ctx, char *mml)
{
    ctx->ud = NULL;
    ctx->mml = NULL;

    mml_parser_reset(ctx);
    ctx->mml = mml;
    ctx->p = ctx->mml;
}

int mml_parser_reset(mml_parser_ctx_t *ctx)
{
    ctx->p = ctx->mml;
    ctx->vsync_cnt = 0;
    ctx->time_ms = 0;

    for (int i = 0; i < TRACK_N; i++)
    {
        ctx->tracks[i].duration = 0;
        ctx->tracks[i].l_duration = 4;
        ctx->tracks[i].end_ms = 0;

        //
        ctx->tracks[i].oct = -1;
        ctx->tracks[i].tempo = 120;
        ctx->tracks[i].note = -1;
        ctx->tracks[i].oct = 0;
        ctx->tracks[i].vol = 7;
        ctx->tracks[i].slot_n = i;
    }
}

static int mml_inc_time(mml_parser_ctx_t *ctx, int trk_n)
{
    static int _i = 0;
    // called each vsync => 1.6ms

    ctx->vsync_cnt++;

    uint32_t ms = ctx->time_ms;
    ctx->time_ms += 16;

    if (ms > ctx->tracks[trk_n].end_ms)
    {

        return 0;
    }
    return -1;
}

static int mml_duration(fix16_t tempo, uint32_t d)
{
    fix16_t d4 = fix16_div(FIX16(4), fix16_int32_from(d));
    fix16_t p = fix16_mul(tempo, d4);

    return p;
}

static int mml_parse_note(char c, mml_parser_ctx_t *ctx, int trk_n)
{
    int note = -1;

    switch (c)
    {
    case 'a':
        note = NOTE_A;
        break;
    case 'b':
        note = NOTE_B;
        break;
    case 'c':
        note = NOTE_C;
        break;
    case 'd':
        note = NOTE_D;
        break;
    case 'e':
        note = NOTE_E;
        break;
    case 'f':
        note = NOTE_F;
        break;
    case 'g':
        note = NOTE_G;
        break;

    case 'p':
    case 'r':
        note = NOTE_REST;
        break;
    }

    switch (*ctx->p)
    {
    case '+':
    case '#':
        note++;
        ctx->p++;
        break;

    case '-':
        note--;
        ctx->p++;
        break;
    }

    // (60 / tempo) * (4 / duration)
    if (isdigit(*ctx->p))
    {
        ctx->tracks[trk_n].duration = get_next_nbr(ctx);
    }
    else
    {
        ctx->tracks[trk_n].duration = ctx->tracks[trk_n].l_duration;
    }

    ctx->tracks[trk_n].time_ms = ctx->time_ms;
    ctx->tracks[trk_n].end_ms = ctx->time_ms + mml_duration(ctx->tracks[trk_n].f_tempo, ctx->tracks[trk_n].duration);
    ctx->tracks[trk_n].note = note;

    return note;
}

int mml_parser_play(mml_parser_ctx_t *ctx, int trk_n)
{
    int note = -1;

    if (mml_inc_time(ctx, trk_n) == -1)
    {
        return 0; // nothing to do...
    }

    do
    {
        skip_space(ctx);
        char c = get_next_ch(ctx);

        switch (c)
        {
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'p':
        case 'r':
            note = mml_parse_note(c, ctx, trk_n);
            break;

        case 'l':
            if (isdigit(*ctx->p))
                ctx->tracks[trk_n].l_duration = get_next_nbr(ctx);

            break;
        case 'o':
            if (isdigit(*ctx->p))
                ctx->tracks[trk_n].oct = get_next_nbr(ctx);
            break;
        case 't':
            if (isdigit(*ctx->p))
            {
                // (6000 / 120) * (4 / 8)

                ctx->tracks[trk_n].tempo = get_next_nbr(ctx);
                fix16_t _1min = 60 * 1000;
                fix16_t _t = fix16_int32_from(ctx->tracks[trk_n].tempo);
                ctx->tracks[trk_n].f_tempo = fix16_div(_1min, _t);
            }
            break;
        case 'v':
            if (isdigit(*ctx->p))
                ctx->tracks[trk_n].vol = get_next_nbr(ctx);
            break;
        case 'm':
            switch (get_next_ch(ctx))
            {
            default:
                break;
                // case 'n':
                //     duration_ration = DEFAULT_TIMING;
                //     break;
                // case 'l':
                //     duration_ration = LEGATO_TIMING;
                //     break;
                // case 's':
                //     duration_ration = STACCATO_TIMING;
                //     break;
            }
            break;

        // case 'n':
        //     if (isdigit(ctx->p))
        //         freqIndex = get_next_nbr(ctx);
        //     ;
        //     break;
        case '<':
            ctx->tracks[trk_n].oct--;
            if (ctx->tracks[trk_n].oct < 0)
                ctx->tracks[trk_n].oct = 0;
            break;
        case '>':
            ctx->tracks[trk_n].oct++;
            if (ctx->tracks[trk_n].oct > 0xf)
                ctx->tracks[trk_n].oct = 0xf;
            break;

        case '\0':
        case ',': // track ends where new track starts
            //_isPlaying = false;
            return -1;
            break;
        }

        if (note != -1)
        {

            if (ctx->toneCallback)
                ctx->toneCallback(ctx->ud,
                                  note,
                                  ctx->tracks[trk_n].oct,
                                  0,
                                  0,
                                  ctx->tracks[trk_n].vol);

            return 0;
        }

        // return 0;
    } while (1);

    return -1;
}

void mml_parser_stop(mml_parser_ctx_t *ctx)
{
}
int mml_parser_is_playing(mml_parser_ctx_t *ctx)
{
}
