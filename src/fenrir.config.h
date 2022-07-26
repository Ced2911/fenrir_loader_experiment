#pragma once
#include <stdint.h>

/*****************************************************
 * [seek] Configuration events
 * 500 to 1000 
 ****************************************************/
#define FENRIR_EVENT_REFRESH_SD_ROOT_FAD (500)
#define FENRIR_EVENT_SET_REGION_MIN_FAD (520)
#define FENRIR_EVENT_SET_REGION_MAX_FAD (FENRIR_EVENT_SET_REGION_MIN_FAD + (16 * 8)) // end at 648
#define FENRIR_EVENT_TOGGLE_AUTO_RELOAD (700)

/*****************************************************
 * [seek] launch games, big area !!!
 * 10000 to 30000
 ****************************************************/
#define FENRIR_EVENT_LAUNCH_ID_START_FAD (10000)
#define FENRIR_EVENT_LAUNCH_ID_END_FAD (30000)

/*****************************************************
 * [read] configuration area
 * 2000
 ****************************************************/
#define FENRIR_READ_CONFIGURATION_FAD (2000)

/*****************************************************
 * [read] status sector area
 * 2010
 ****************************************************/
#define FENRIR_READ_STATUS_FAD (2010)

/*****************************************************
 * [read] Filebrowser streaming
 * 3000 to 4000
 ****************************************************/
#define FENRIR_READ_FILEBROWSER_START_FAD (3000)
#define FENRIR_READ_FILEBROWSER_END_FAD (4000)

/*****************************************************
 * Extended area - allow 2500 games to be launched
 ****************************************************/
// todo
/*****************************************************
 * Extended area - reserve an area for multiples saturn parsed header
 * Each ip header size is 0xFF
 * It need about (2500 * 256 / 2048) ~= 313, we will use 500
 ****************************************************/
// todo

#define FENRIR_FILEBROWSER_MIN (500)
#define FENRIR_FILEBROWSER_MAX (40000)

#define STATUS_SECTOR_SIZE 2048

enum
{
    FENRIR_STATUS_READY = 0,
    FENRIR_STATUS_BUSY = 1,
    FENRIR_STATUS_ERROR = 2,
};

enum
{
    FENRIR_STATUS_TYPE_UNKNOWN = 0,
    FENRIR_STATUS_TYPE_REFRESH = 1,
    FENRIR_STATUS_TYPE_WRITE_FILE = 2,
};

enum
{
    FENRIR_WRITE_NOTHING = 0,
    FENRIR_WRITE_CHUNK = 1,
    FENRIR_WRITE_CHUNK_ERROR = 2,
};

typedef union
{
    uint8_t raw[STATUS_SECTOR_SIZE];

    // common
    struct
    {
        uint8_t status;
        uint8_t type;
        uint8_t data[];
    };

    // refresh scan
    struct
    {
        uint8_t _scan_status;
        uint8_t _scan_type;
        uint16_t progress;
    };
    // write file
    struct
    {
        uint8_t _save_status;
        uint8_t _save_type;
        uint8_t write_op;
        uint8_t write_id;
        uint8_t chunk_checksum;
    };
} status_sector_t;

static inline uint8_t chunk_checksum(uint8_t *data, uint32_t len)
{
    uint8_t checksum = 0x00;
    for (int i = 0; i < len; i++)
    {
        checksum ^= data[i];
    }
    return checksum;
}
