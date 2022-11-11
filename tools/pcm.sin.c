#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
// gcc pcm.sin.c -o pcm.sin -lm
// https://pages.mtu.edu/~suits/notefreqs.html

#if 0
#define DURATION_SEC 2
#define SAMPLE_RATE 44100
#define NOTE_FREQ 261.63 // c4

#define INCR ((NOTE_FREQ * 2 * M_PI) / SAMPLE_RATE)
#define LEN (2 * SAMPLE_RATE / NOTE_FREQ * sizeof(int16_t))
//#define LEN DURATION_SEC * SAMPLE_RATE * 2

void main()
{
    FILE *fd = fopen("sample.pcm", "wb");
    int16_t *samples = (int16_t *)malloc(LEN);
    double inc = INCR;
    double v = 0;
    printf("inc:%f\n", INCR);
    printf("len:%f\n", LEN);
    for (int i = 0; i < LEN / 2; i++)
    {
        samples[i] = __builtin_bswap16(sin(v) * 32767);
        printf("%d: %f\n", i, sin(v) * 32767);
        v += inc;
    }

    fwrite(samples, LEN, 1, fd);
    fclose(fd);
}
#else
#define DURATION_SEC 2
#define SAMPLE_RATE 44100
#define NOTE_FREQ ((2 * 44100) / 256)

#define INCR ((NOTE_FREQ * 2 * M_PI) / SAMPLE_RATE)
#define LEN (SAMPLE_RATE / NOTE_FREQ * sizeof(int8_t))
//#define LEN DURATION_SEC * SAMPLE_RATE * 2

void main()
{
    FILE *fd = fopen("sample.pcm", "wb");
    uint8_t *samples = (uint8_t *)malloc(LEN);
    double inc = INCR;
    double v = 0;
    printf("inc:%f\n", INCR);
    printf("len:%d\n", LEN);
    for (int i = 0; i < LEN; i++)
    {
        samples[i] = (sin(v) * 127);
        printf("%d: %04x\n", i, (int)(sin(v) * 127));
        v += inc;
    }

    fwrite(samples, LEN, 1, fd);
    fclose(fd);
}
#endif