#pragma once
#ifdef UNIT_TEST
#include <assert.h>
#endif

#define ODEFS_FILENAME_LENGTH (32 - 2 - 2 - 1)

// max number of entries per dir
#define ODEFS_ENTRIES_MAX (4900)

#define ODEFS_FAD (20000)
#define ODEFS_PATH_INFO_FAD (20000)

// change entry (file/dir)
// 0 => root dir
#define ODEFS_SELECT_PATH_FAD (25000)
#define ODEFS_SELECT_PATH_FAD_MAX (ODEFS_SELECT_PATH_FAD + ODEFS_ENTRIES_MAX)
#define ODEFS_SELECT_FILE_FAD (30000)
#define ODEFS_SELECT_FILE_FAD_MAX (ODEFS_SELECT_FILE_FAD + ODEFS_ENTRIES_MAX)

//=== file read / max read size 64MB per file ===/
#define ODEFS_FILE_READ_FAD (40000)
#define ODEFS_FILE_READ_FAD_MAX (ODEFS_FILE_READ_FAD + 32768)
#define ODEFS_DIR_READ_FAD (50000)
#define ODEFS_DIR_READ_FAD_MAX (ODEFS_DIR_READ_FAD + 32768)

typedef enum odefs_entry_type
{
    ODEFS_ENTRY_TYPE_FILE,
    ODEFS_ENTRY_TYPE_DIR
} odefs_entry_type_t;

typedef struct __attribute__((__packed__))
{
    uint16_t size; // max 64mb files
    uint16_t fad;  // add ODEFS_FAD to get real offset
    uint8_t flag;  // odefs_entry_type
    char name[ODEFS_FILENAME_LENGTH];
} odefs_entry_t;

typedef struct __attribute__((__packed__))
{
    uint16_t number_of_entries;
    char name[255]; // fullpath
    char *padding;  // unused
} odefs_path_info_t;

#ifdef UNIT_TEST
static_assert(sizeof(odefs_entry_t) == 32, "Structure size error");
#endif

void odefs_filelist_root_read(odefs_entry_t *entries, int *entries_count);
void odefs_filelist_dir_read(odefs_entry_t *dir, odefs_entry_t *entries, int *entries_count);
void odefs_file_read(odefs_entry_t * entry);
odefs_entry_t * odefs_find_entry(const char * name, odefs_entry_t * entry);

// void odefs_filelist_read(odefs_filelist_t *filelist, const odefs_filelist_entry_t root_entry);
