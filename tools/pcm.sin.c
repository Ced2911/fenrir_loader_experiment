#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
// gcc pcm.sin.c -o pcm.sin -lm
// https://pages.mtu.edu/~suits/notefreqs.html

#if 1
#define DURATION_SEC 4
#define SAMPLE_RATE 44100.0
#define NOTE_FREQ 440.0 // a4

#define INCR ((NOTE_FREQ * 2.0 * M_PI) / SAMPLE_RATE)
#define LEN (2 * SAMPLE_RATE / NOTE_FREQ * sizeof(int16_t))
// #define LEN DURATION_SEC *SAMPLE_RATE * 2

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
        double tonePeriodSeconds = 1.0 / NOTE_FREQ;
        double radians = ((double)(i / SAMPLE_RATE)) / tonePeriodSeconds * (2 * M_PI);
        double result = sin(radians);

        // uint16_t s = result * (32767.0 / 2.0);
        int16_t s = result < 0 ? 32767 : -32768;
        samples[i] = __builtin_bswap16(s);
        printf("%d: %04x\n",i,  (uint16_t)s);
    }

    fwrite(samples, LEN, 1, fd);
    fclose(fd);
}
#elif 0
#define DURATION_SEC 2
#define SAMPLE_RATE 44100
#define NOTE_FREQ ((2 * 44100) / 256)

#define INCR ((NOTE_FREQ * 2 * M_PI) / SAMPLE_RATE)
#define LEN (SAMPLE_RATE / NOTE_FREQ * sizeof(int8_t))
// #define LEN DURATION_SEC * SAMPLE_RATE * 2

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
#elif 1

#define DURATION_SEC 2
#define SAMPLE_RATE 44100.0
// #define NOTE_FREQ (277.18)
#define NOTE_FREQ (440.0)

#define INCR ((NOTE_FREQ * 2 * M_PI) / SAMPLE_RATE)
#define LEN ((int)(50 * (SAMPLE_RATE / NOTE_FREQ) * sizeof(int8_t)))
// #define LEN DURATION_SEC * SAMPLE_RATE * 2

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

void main()
{
    FILE *fd = fopen("sample.pcm", "wb");
    uint8_t *samples = (uint8_t *)malloc(LEN);
    double inc = INCR;

    double cy = floor(SAMPLE_RATE / NOTE_FREQ);
    printf("inc:%f\n", INCR);
    printf("len:%d\n", LEN);
    printf("cy:%f\n", cy);
    for (int k = 0; k < 10; k++)
        for (int i = 0; i < cy; i++)
        {
            double v = MIN(128 * sin(((double)i / cy) * M_PI * 2.0), 127);
            uint8_t s = round(v) + 128;
            samples[i + k * (int)cy] = s;
        }

    fwrite(samples, cy * 10, 1, fd);
    fclose(fd);
}

#elif 1

#define DURATION_SEC 2
#define SAMPLE_RATE 44100
#define NOTE_FREQ (440)

#define INCR ((NOTE_FREQ * 2 * M_PI) / SAMPLE_RATE)
#define LEN (2 * SAMPLE_RATE / NOTE_FREQ * sizeof(int8_t))
// #define LEN DURATION_SEC * SAMPLE_RATE * 2

void main()
{
    FILE *fd = fopen("sample.square.pcm", "wb");
    uint8_t *samples = (uint8_t *)malloc(LEN);
    double inc = INCR;
    double v = 0;
    printf("inc:%f\n", INCR);
    printf("len:%d\n", LEN);
    for (int i = 0; i < LEN; i++)
    {
        uint8_t s = sin(v) >= 0 ? 0x7f : 0xff;
        samples[i] = s;
        printf("%d: %04x\n", i, s);
        v += inc;
    }

    fwrite(samples, LEN, 1, fd);
    fclose(fd);
}
#else
#define DURATION_SEC 2
#define SAMPLE_RATE 44100
#define NOTE_FREQ (440)

#define INCR ((NOTE_FREQ * 2 * M_PI) / SAMPLE_RATE)
#define LEN (256)

unsigned char rawData[256] = {
    0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01,
    0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00,
    0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x00, 0x01, 0x00, 0x01, 0x01,
    0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01,
    0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00,
    0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01,
    0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01,
    0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00,
    0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
    0x01, 0x01, 0x01, 0x00};

void main()
{
    FILE *fd = fopen("sample.periodic.pcm", "wb");
    uint8_t *samples = (uint8_t *)malloc(LEN);
    uint8_t v = 0;
    printf("inc:%f\n", INCR);
    printf("len:%d\n", LEN);
    uint16_t rng = 0x8000;
    for (int i = 0; i < LEN; i++)
    {
        uint8_t s = rawData[i];

        samples[i] = s ? 0x7f : 0xff;
        printf("%d: %04x\n", i, s);
    }

    fwrite(samples, LEN, 1, fd);
    fclose(fd);
}
#endif