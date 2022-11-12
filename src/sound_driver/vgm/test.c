#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct
{
    int32_t basefreq;
    int32_t approxtype;
    int32_t slope;
} freqtable_t;

static const freqtable_t pg_freqtable[64] = {
    {1299, 1, 19},
    {1318, 1, 19},
    {1337, 1, 19},
    {1356, 1, 20},
    {1376, 1, 20},
    {1396, 1, 20},
    {1416, 1, 21},
    {1437, 1, 20},
    {1458, 1, 21},
    {1479, 1, 21},
    {1501, 1, 22},
    {1523, 1, 22},
    {0, 0, 16},
    {0, 0, 16},
    {0, 0, 16},
    {0, 0, 16},
    {1545, 1, 22},
    {1567, 1, 22},
    {1590, 1, 23},
    {1613, 1, 23},
    {1637, 1, 23},
    {1660, 1, 24},
    {1685, 1, 24},
    {1709, 1, 24},
    {1734, 1, 25},
    {1759, 1, 25},
    {1785, 1, 26},
    {1811, 1, 26},
    {0, 0, 16},
    {0, 0, 16},
    {0, 0, 16},
    {0, 0, 16},
    {1837, 1, 26},
    {1864, 1, 27},
    {1891, 1, 27},
    {1918, 1, 28},
    {1946, 1, 28},
    {1975, 1, 28},
    {2003, 1, 29},
    {2032, 1, 30},
    {2062, 1, 30},
    {2092, 1, 30},
    {2122, 1, 31},
    {2153, 1, 31},
    {0, 0, 16},
    {0, 0, 16},
    {0, 0, 16},
    {0, 0, 16},
    {2185, 1, 31},
    {2216, 0, 31},
    {2249, 0, 31},
    {2281, 0, 31},
    {2315, 0, 31},
    {2348, 0, 31},
    {2382, 0, 30},
    {2417, 0, 30},
    {2452, 0, 30},
    {2488, 0, 30},
    {2524, 0, 30},
    {2561, 0, 30},
    {0, 0, 16},
    {0, 0, 16},
    {0, 0, 16},
    {0, 0, 16}};

static int32_t OPM_CalcKCode(int32_t kcf, int32_t lfo, int32_t lfo_sign, int32_t dt)
{
    int32_t t2, t3, b0, b1, b2, b3, w2, w3, w6;
    int32_t overflow1 = 0;
    int32_t overflow2 = 0;
    int32_t negoverflow = 0;
    int32_t sum, cr;
    if (!lfo_sign)
    {
        lfo = ~lfo;
    }
    sum = (kcf & 8191) + (lfo & 8191) + (!lfo_sign);
    cr = ((kcf & 255) + (lfo & 255) + (!lfo_sign)) >> 8;
    if (sum & (1 << 13))
    {
        overflow1 = 1;
    }
    sum &= 8191;
    if (lfo_sign && ((((sum >> 6) & 3) == 3) || cr))
    {
        sum += 64;
    }
    if (!lfo_sign && !cr)
    {
        sum += (-64) & 8191;
        negoverflow = 1;
    }
    if (sum & (1 << 13))
    {
        overflow2 = 1;
    }
    sum &= 8191;
    if ((!lfo_sign && !overflow1) || (negoverflow && !overflow2))
    {
        sum = 0;
    }
    if (lfo_sign && (overflow1 || overflow2))
    {
        sum = 8127;
    }

    t2 = sum & 63;
    if (dt == 2)
        t2 += 20;
    if (dt == 2 || dt == 3)
        t2 += 32;

    b0 = (t2 >> 6) & 1;
    b1 = dt == 2;
    b2 = ((sum >> 6) & 1);
    b3 = ((sum >> 7) & 1);

    w2 = (b0 && b1 && b2);
    w3 = (b0 && b3);
    w6 = (b0 && !w2 && !w3) || (b3 && !b0 && b1);

    t2 &= 63;

    t3 = (sum >> 6) + w6 + b1 + (w2 || w3) * 2 + (dt == 3) * 4 + (dt != 0) * 8;
    if (t3 & 128)
    {
        t2 = 63;
        t3 = 126;
    }
    sum = t3 * 64 + t2;
    return sum;
}

static int32_t OPM_KCToFNum(int32_t kcode)
{
    int32_t kcode_h = (kcode >> 4) & 63;
    int32_t kcode_l = kcode & 15;
    int32_t i, slope, sum = 0;
    if (pg_freqtable[kcode_h].approxtype)
    {
        for (i = 0; i < 4; i++)
        {
            if (kcode_l & (1 << i))
            {
                sum += (pg_freqtable[kcode_h].slope >> (3 - i));
            }
        }
    }
    else
    {
        slope = pg_freqtable[kcode_h].slope | 1;
        if (kcode_l & 1)
        {
            sum += (slope >> 3) + 2;
        }
        if (kcode_l & 2)
        {
            sum += 8;
        }
        if (kcode_l & 4)
        {
            sum += slope >> 1;
        }
        if (kcode_l & 8)
        {
            sum += slope;
            sum++;
        }
        if ((kcode_l & 12) == 12 && (pg_freqtable[kcode_h].slope & 1) == 0)
        {
            sum += 4;
        }
    }
    return pg_freqtable[kcode_h].basefreq + (sum >> 1);
}

void main()
{
    int32_t kcode = OPM_CalcKCode(0x4a << 6, 0, 0, 0);
    int32_t fnum = OPM_KCToFNum(kcode);
    uint32_t kcode_h = kcode >> 8;
    uint32_t block = kcode_h >> 2;
    uint32_t basefreq = (fnum << block) >> 2;
    printf("%d\n", kcode);
    printf("%d\n", fnum);
    printf("%d\n", basefreq);
}