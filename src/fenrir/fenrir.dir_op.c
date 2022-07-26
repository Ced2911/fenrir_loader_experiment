/*****************************************************
 * WIP
 * Abuse seek to transfer data to sd card
 *****************************************************/

#include <yaul.h>
#include "fenrir.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

static uint8_t tmp[2048];

int fenrir_select_direntry(int16_t id)
{
    if (id > FENRIR_MAX_IO)
    {
        // error... too many files
        return -1;
    }

    // fixed id
    if (id == DIR_ENTRY_ROOT)
    {
        cd_block_sector_read(FENRIR_IO_SELECT, (void *)tmp);
    }
    else if (id == PARENT_DIRECTORY)
    {
        cd_block_sector_read(FENRIR_IO_SELECT + PARENT_DIRECTORY, (void *)tmp);
    }
    // others
    else
    {
        cd_block_sector_read(FENRIR_IO_SELECT + id, (void *)tmp);
    }

    // wait a few...
    vdp2_sync();
    return 0;
}

int fenrir_readdir(fenrir_dir_data_t *fenrir_dir_data)
{
    memset(fenrir_dir_data, 0, sizeof(fenrir_dir_data_t));
    cd_block_sector_read(FENRIR_IO_READ, (void *)fenrir_dir_data);
    if (fenrir_dir_data->count == 0)
    {
        return -1;
    }
    return 0;
}

// Attention ne lit qu'une part !!
int fenrir_readfile_packet(void *buffer)
{
    return cd_block_sector_read(FENRIR_IO_READ, (void *)buffer);
}

fenrir_dir_entry_t *fenrir_find_entry(fenrir_dir_data_t *fenrir_dir_data, char *name)
{
    // remove /sdcard/
    char *d = strstr(name, "/sdcard/");
    if (d == NULL)
    {
        d = name;
    }
    else
    {
        d += strlen("/sdcard/");
    }

    // end of path
    char *slen = d + strlen(d) - 1;

    // find next '/'
    char *next = strchr(d, '/');
    next = MIN(next, slen);

    // select root directory /
    fenrir_select_direntry(DIR_ENTRY_ROOT);

    // root directory ...
    if (strncasecmp(name, "/sdcard/", 9) == 0 || strncasecmp(name, "/sdcard", 8) == 0)
    {
        return (fenrir_dir_entry_t *)(DIR_ENTRY_ROOT);
    }

    do
    {
        if (fenrir_readdir(fenrir_dir_data) == 0)
        {
            if (fenrir_dir_data->count == 0)
            {
                // no file found
                return NULL;
            }

            for (int i = 0; i < fenrir_dir_data->count; i++)
            {
                fenrir_dir_entry_t *entry = &fenrir_dir_data->entries[i];

                int strlen = next ? (next - d) : (slen - d);

                if (strncmp(d, entry->name, strlen) == 0)
                {
                    fenrir_select_direntry(entry->id);

                    // is it last segment ?
                    // int n = (slen - d);
                    if (next == 0)
                    {
                        return entry;
                    }

                    // update d & next
                    d = next + 1;
                    next = strchr(d, '/');
                    next = MIN(next, slen);

                    if (next == slen)
                    {
                        // end
                        next = 0;
                    }

                    break;
                }
            }
        }
        else
        {
            // read error
            return NULL;
        }
    } while (1);
    return NULL;
}