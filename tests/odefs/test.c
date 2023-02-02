#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <dirent.h>
#include <string.h>
#include "odefs.h"
#include "utest.h"

#define ODEFS_ROOT "/tmp-ode-fs/"

//===================================================
// odefs context
//===================================================

typedef struct
{
    char cur_path[256];
    DIR *dir;
    DIR *root_dir;
    struct
    {
        FILE *fd;
        uint32_t size;
        uint32_t offset;

    } file;

} odefs_ctx_t;

odefs_ctx_t odefs_ctx;

void odefs_init(odefs_ctx_t *ctx)
{
    ctx->cur_path[0] = 0;
    ctx->dir = NULL;
    ctx->file.fd = NULL;
    ctx->file.size = 0;
    ctx->file.offset = 0;

    // root dir by default
    ctx->root_dir = ctx->dir = opendir(ODEFS_ROOT);
    strcpy(ctx->cur_path, ODEFS_ROOT);
}

/**
 * check if filename is ignore
 * - starting with .
 */
int odefs_ignore_filename(char *filename)
{
    if (filename[0] == '.')
    {
        return 1;
    }
    return 0;
}

static struct dirent *odefs_get_dir_entry(DIR *dir, uint32_t id)
{    
    assert(dir != NULL);
    // seekdir not impl
    rewinddir(dir);
    for (int i = 0; i < id; i++)
    {
        if (readdir(dir) == NULL)
        {
            // err..
            return NULL;
        }
    }
    struct dirent *dirent = readdir(dir);
    return dirent;
}

//===================================================
// ll file function
//===================================================
void odefs_read_file_open(odefs_ctx_t *ctx, uint32_t id)
{
    if (ctx->file.fd)
    {
        fclose(ctx->file.fd);
        ctx->file.fd = NULL;
    }

    if (ctx->dir)
    {
        // use real id
        id = id - 1;
        struct dirent *dirent = odefs_get_dir_entry(ctx->dir, id);
        if (dirent && dirent->d_type == DT_REG)
        {
            char *filename = (char *)malloc(512);
            sprintf(filename, "%s%s", ctx->cur_path, dirent->d_name);
            ctx->file.fd = fopen(filename, "rb");
            ctx->file.size = 0;
            ctx->file.offset = 0;

            // get filesz
            if (ctx->file.fd)
            {
                fseek(ctx->file.fd, 0, SEEK_END);
                ctx->file.size = ftell(ctx->file.fd);
                fseek(ctx->file.fd, 0, SEEK_SET);
            }
            free(filename);
        }
    }
}

void odefs_read_file_seek(odefs_ctx_t *ctx, size_t offset)
{
    if (ctx->file.fd)
    {
        fseek(ctx->file.fd, offset, SEEK_SET);
        ctx->file.offset = offset;
    }
}

void odefs_read_file_sector(odefs_ctx_t *ctx, uint8_t *data)
{
    if (ctx->file.fd && ctx->file.offset < ctx->file.size)
    {
        fread(data, 2048, 1, ctx->file.fd);
        ctx->file.offset += 2048;
    }
}

//===================================================
// ll dir function
//===================================================
void odefs_read_dir_open(odefs_ctx_t *ctx, uint32_t id)
{
    if (id == 0)
    {
        ctx->dir = ctx->root_dir;
        rewinddir(ctx->dir);
    }
    else
    {
        // use real id
        id = id - 1;
        struct dirent *dirent = odefs_get_dir_entry(ctx->dir, id);
        if (dirent && dirent->d_type == DT_DIR)
        {
            char *filename = (char *)malloc(512);
            sprintf(filename, "%s%s/", ctx->cur_path, dirent->d_name);
            strcpy(ctx->cur_path, filename);

            // never close rootdir
            if (ctx->dir && ctx->dir != ctx->root_dir)
            {
                closedir(ctx->dir);
                ctx->dir = NULL;
            }

            ctx->dir = opendir(filename);
            free(filename);
        }
        else
        {
            printf("error: dirent id:%d\n", id);
            printf("dirent %p\n", dirent);
            if (dirent)
            {
                printf("name %s\n", dirent->d_name);
            }
        }
    }
}

void odefs_read_dir_seek(odefs_ctx_t *ctx, size_t offset)
{
    assert(ctx->dir != NULL);
    rewinddir(ctx->dir);
    for (int i = 0; i < offset; i++)
    {
        if (readdir(ctx->dir) == NULL)
        {
            return;
        }
    }
}

void odefs_read_dir(odefs_ctx_t *ctx, uint8_t *data)
{
    size_t max_entries = 2048 / sizeof(odefs_entry_t);
    odefs_entry_t *odefs_entries = (odefs_entry_t *)data;
    struct dirent *dirent = NULL;
    int id = 1;
    while ((dirent = readdir(ctx->dir)) != NULL)
    {
        if (!odefs_ignore_filename(dirent->d_name))
        {
            odefs_entries->fad = id;
            odefs_entries->flag = dirent->d_type & DT_DIR ? ODEFS_ENTRY_TYPE_DIR : ODEFS_ENTRY_TYPE_FILE;
            strncpy(odefs_entries->name, dirent->d_name, ODEFS_FILENAME_LENGTH);
            odefs_entries->size = 0; // todo...
            odefs_entries++;
        }

        id++;
    }
}

void odefs_read_dir_info(odefs_ctx_t *ctx, uint8_t *data)
{
    odefs_path_info_t *info = (odefs_path_info_t *)data;
    info->number_of_entries = 0;
    struct dirent *dirent = NULL;
    if (ctx->dir)
    {
        rewinddir(ctx->dir);
        while ((dirent = readdir(ctx->dir)) != NULL)
            if (!odefs_ignore_filename(dirent->d_name))
                info->number_of_entries++;
        rewinddir(ctx->dir);
    }
}

//===================================================
// menu hook function
//===================================================
typedef void (*menu_read_callback_t)(uint32_t, uint8_t *);

typedef struct
{
    const char *name;
    uint32_t fad_start;
    uint32_t fad_end;
    menu_read_callback_t handler;
} menu_read_t;

void _odefs_select_dir(uint32_t sector, uint8_t *data)
{
    uint32_t id = sector - ODEFS_SELECT_PATH_FAD;
    odefs_read_dir_open(&odefs_ctx, id);
}

void _odefs_read_dirinfo(uint32_t sector, uint8_t *data)
{
    uint32_t id = sector - ODEFS_PATH_INFO_FAD;
    odefs_read_dir_seek(&odefs_ctx, 0);
    odefs_read_dir_info(&odefs_ctx, data);
}

void _odefs_read_dir(uint32_t sector, uint8_t *data)
{
    uint32_t id = sector - ODEFS_DIR_READ_FAD;
    odefs_read_dir_seek(&odefs_ctx, id);
    odefs_read_dir(&odefs_ctx, data);
}

void _odefs_read_file(uint32_t sector, uint8_t *data)
{
    uint32_t offset = (sector - ODEFS_FILE_READ_FAD) * 2048;
    odefs_read_file_seek(&odefs_ctx, offset);
    odefs_read_file_sector(&odefs_ctx, data);
}

void _odefs_select_file(uint32_t sector, uint8_t *data)
{
    uint32_t id = sector - ODEFS_SELECT_FILE_FAD;
    odefs_read_file_open(&odefs_ctx, id);
}

static menu_read_t read_lut[] = {
    // select
    {
        .name = "_odefs_select_dir",
        .fad_start = ODEFS_SELECT_PATH_FAD,
        .fad_end = ODEFS_SELECT_PATH_FAD_MAX,
        .handler = _odefs_select_dir,
    },
    {
        .name = "_odefs_select_file",
        .fad_start = ODEFS_SELECT_FILE_FAD,
        .fad_end = ODEFS_SELECT_FILE_FAD_MAX,
        .handler = _odefs_select_file,
    },

    // read
    {
        .name = "_odefs_read_dirinfo",
        .fad_start = ODEFS_PATH_INFO_FAD,
        .fad_end = ODEFS_PATH_INFO_FAD,
        .handler = _odefs_read_dirinfo,
    },
    {
        .name = "_odefs_read_dir",
        .fad_start = ODEFS_DIR_READ_FAD,
        .fad_end = ODEFS_DIR_READ_FAD_MAX,
        .handler = _odefs_read_dir,
    },
    {
        .name = "_odefs_read_file",
        .fad_start = ODEFS_FILE_READ_FAD,
        .fad_end = ODEFS_FILE_READ_FAD_MAX,
        .handler = _odefs_read_file,
    },

};

typedef uint32_t sector_t;
void cdfs_sector_read(sector_t fad, void *ptr)
{
    // simulate fenrir
    uint8_t *dst = (uint8_t *)ptr;
    memset(dst, 0, 2048);

    for (int i = 0; i < (sizeof(read_lut) / sizeof(read_lut[0])); i++)
    {
        if (fad >= read_lut[i].fad_start && fad <= read_lut[i].fad_end)
        {
            printf("execute %s(%d)\n", read_lut[i].name, fad);
            read_lut[i].handler(fad, dst);
            return;
        }
    }

    printf("No handler found at %d\n", fad);
}
//===================================================
// unit tests
//===================================================

/**
 * unit test script
mkdir -p /tmp-ode-fs
mkdir -p /tmp-ode-fs/dir1
mkdir -p /tmp-ode-fs/dir2/dir3/dir4/dir5/
mkdir -p /tmp-ode-fs/dirblabla
echo "some content in file 1" > /tmp-ode-fs/1_file
echo "some content in file 2" > /tmp-ode-fs/2_file
echo "some content in file 3" > /tmp-ode-fs/3_file
echo "some content in file 3" > /tmp-ode-fs/dir1/an_other_file
echo "some content in file an_other_file" > /tmp-ode-fs/dir2/dir3/dir4/dir5/an_other_file
echo "blabla" > /tmp-ode-fs/dir1/yet_an_other_file
*/

int test_find_file(odefs_entry_t *entries, int cnt, char *search, int search_type)
{
    for (int i = 0; i < cnt; i++)
    {
        if (search_type == entries[i].flag && strcmp(entries[i].name, search) == 0)
        {
            return 1;
        }
    }
    return -1;
}

UTEST(odefs, filelist_root_read)
{
    odefs_entry_t *entries = (odefs_entry_t *)malloc(ODEFS_ENTRIES_MAX * sizeof(odefs_entry_t));
    int cnt = 0;
    odefs_filelist_root_read(entries, &cnt);
    ASSERT_TRUE(cnt == 6);

    ASSERT_EQ(test_find_file(entries, cnt, "1_file", ODEFS_ENTRY_TYPE_FILE), 1);
    ASSERT_EQ(test_find_file(entries, cnt, "2_file", ODEFS_ENTRY_TYPE_FILE), 1);
    ASSERT_EQ(test_find_file(entries, cnt, "3_file", ODEFS_ENTRY_TYPE_FILE), 1);

    ASSERT_EQ(test_find_file(entries, cnt, "dir2", ODEFS_ENTRY_TYPE_DIR), 1);
    ASSERT_EQ(test_find_file(entries, cnt, "dir1", ODEFS_ENTRY_TYPE_DIR), 1);
    ASSERT_EQ(test_find_file(entries, cnt, "dirblabla", ODEFS_ENTRY_TYPE_DIR), 1);
}

UTEST(odefs_find_entry, exists_file)
{
    odefs_entry_t etr;
    odefs_entry_t *entry = odefs_find_entry("/tmp-ode-fs/1_file", &etr);
    ASSERT_NE(NULL, entry);
    ASSERT_STREQ("1_file", entry->name);
}

UTEST(odefs_find_entry, exists_dir)
{
    odefs_entry_t etr;
    odefs_entry_t *entry = odefs_find_entry("/tmp-ode-fs/dir2", &etr);
    ASSERT_NE(NULL, entry);
    ASSERT_STREQ("dir2", entry->name);
}

UTEST(odefs_find_entry, exists_subdir_file)
{
    odefs_entry_t etr;
    odefs_entry_t *entry = odefs_find_entry("/tmp-ode-fs/dir2/dir3/dir4/dir5/an_other_file", &etr);
    ASSERT_NE(NULL, entry);
    ASSERT_STREQ("an_other_file", entry->name);
    uint8_t sector[2048];
    cdfs_sector_read(ODEFS_SELECT_FILE_FAD + entry->fad, sector);
    cdfs_sector_read(ODEFS_FILE_READ_FAD, sector);
    ASSERT_STREQ("some content in file an_other_file\n", sector);
}
UTEST(odefs_find_entry, non_exists)
{
    odefs_entry_t etr;
    odefs_entry_t *entry = odefs_find_entry("/tmp-ode-fs/zboob", &etr);
    ASSERT_EQ(NULL, entry);
}

UTEST_STATE();
int main(int argc, const char *const argv[])
{
    odefs_init(&odefs_ctx);
    return utest_main(argc, argv);
}