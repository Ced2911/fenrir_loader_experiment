#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef UNIT_TEST
// yaul compat
typedef uint32_t sector_t;
extern void cdfs_sector_read(sector_t sector, void *ptr);
#endif

#include "odefs.h"

#define ODEFS_ENTRIES_PER_SECTOR (2048 / sizeof(odefs_entry_t))

static uint8_t sector[2048];

static void dir_walk(sector_t starting_fad, odefs_entry_t *entries, int *entries_count)
{
    int count = 0;
    // get path info
    cdfs_sector_read(ODEFS_PATH_INFO_FAD, sector);
    odefs_path_info_t *odefs_path_info = (odefs_path_info_t *)sector;
    *entries_count = odefs_path_info->number_of_entries;
    count = *entries_count;

    sector_t fad = starting_fad;
    while (count > 0)
    {
        cdfs_sector_read(fad++, entries);
        count -= ODEFS_ENTRIES_PER_SECTOR;
    }
}

void odefs_filelist_root_read(odefs_entry_t *entries, int *entries_count)
{
    *entries_count = 0;
    if (entries != NULL)
    {
        dir_walk(ODEFS_DIR_READ_FAD + 0, entries, entries_count);
    }
}

void odefs_filelist_dir_read(odefs_entry_t *dir, odefs_entry_t *entries, int *entries_count)
{
    *entries_count = 0;
    if (entries != NULL && dir != NULL)
    {
        dir_walk(ODEFS_DIR_READ_FAD + dir->fad, entries, entries_count);
    }
}

static odefs_entry_t *odefs_find_entry_in_dir(sector_t dir_fad,
                                              const char *path,
                                              const char *next_dir,
                                              odefs_entry_t *entry)
{
    int count = 0;

    // change path
    cdfs_sector_read(ODEFS_SELECT_PATH_FAD + dir_fad, sector);


    // get path info
    cdfs_sector_read(ODEFS_PATH_INFO_FAD, sector);
    odefs_path_info_t *odefs_path_info = (odefs_path_info_t *)sector;
    odefs_entry_t *entries = (odefs_entry_t *)sector;
    count = odefs_path_info->number_of_entries;

    sector_t fad = ODEFS_DIR_READ_FAD ;

    //    *next_dir = strchr(path + 1, '/') + 1;

    printf("next dir: %s\n", next_dir);
    printf("count %d\n", count);
    while (count > 0)
    {
        cdfs_sector_read(fad++, sector);
        // parse each entry...
        for (int i = 0; i < ODEFS_ENTRIES_PER_SECTOR; i++)
        {
            if (entries[i].name[0] == 0)
            {
                break;
            }
            else
            {
                printf("test %s %s %s %d\n", path, entries[i].name, next_dir, next_dir - path);

                // if next_dir look in subfolder
                // if not look only in current folder
                if (next_dir)
                {
                    if (strncmp(path, entries[i].name, next_dir - path) == 0)
                    {

                        char *n_path = strchr(path + 1, '/') + 1;
                        next_dir = strchr(n_path, '/');

                        printf("look in subfolder... %s\n", n_path);
                        return odefs_find_entry_in_dir(entries[i].fad, n_path, next_dir, entry);
                    }
                }
                else if (strcmp(path, entries[i].name) == 0)
                {
                    memcpy(entry, &entries[i], sizeof(odefs_entry_t));
                    return entry;
                }
            }
        }
        count -= ODEFS_ENTRIES_PER_SECTOR;
    }

    return NULL;
}

odefs_entry_t *odefs_find_entry(const char *name, odefs_entry_t *entry)
{
    // start from root dir
    int count = 0;

    // skip /sdcard/
    char *path = strchr(name + 1, '/') + 1;
    // find next /
    char *next_dir = strchr(path, '/');
    if (next_dir)
    {
        // next_dir++; // skip last /
    }

printf("find entry: %s\n", name);
    odefs_entry_t *ret = odefs_find_entry_in_dir(0, path, next_dir, entry);

    return ret;
}