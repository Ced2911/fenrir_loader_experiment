#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "vgm.h"
#include "trk.h"

// ffplay -f s16le -ar 44.1k -ac 2 pcm.s16

int main(int argc, char *argv[])
{
    FILE *fd = fopen("pcm.s16", "wb");

    vgm_player_t player;
    player.vgm = trk;
    player.samples = (int **)malloc(2 * sizeof(int *));
    player.samples[0] = (int *)malloc(2352 * sizeof(int));
    player.samples[1] = (int *)malloc(2352 * sizeof(int));

    uint16_t *pcm = (uint16_t *)malloc(1024 * 1024 * 1000);
    vgm_init(&player);

    uint16_t data[2352];
    int i = 1;
    // for (i = 0; i < 4000; i++)
#if 1
    while (player.vgmend == 0)
    {
        vgm_do_audio(&player, i * 2352, (uint8_t *)data);
        fwrite(data, 2352, 1, fd);
        i++;
    }
#else
    vgm_do_samples_full(&player);
#endif
    // fwrite(pcm, (player.pcm - pcm), 1, fd);

    fclose(fd);

    return 0;
}
