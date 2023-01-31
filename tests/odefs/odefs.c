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
