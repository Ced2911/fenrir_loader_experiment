#pragma once
#include <stdint.h>

/*****************************************************
 * [seek] Configuration events
 * 500 to 1000 
 ****************************************************/
#define FENRIR_EVENT_SET_REGION_MIN_FAD (520)
#define FENRIR_EVENT_SET_REGION_MAX_FAD (FENRIR_EVENT_SET_REGION_MIN_FAD + (16 * 8)) // end at 648
#define FENRIR_EVENT_TOGGLE_AUTO_RELOAD (700)
#define FENRIR_EVENT_TOGGLE_COVER (710)
#define FENRIR_EVENT_CHOOSE_SOURCE (720)
#define FENRIR_EVENT_CHOOSE_SOURCE_SD (720)
#define FENRIR_EVENT_CHOOSE_SOURCE_HTTP (721)

/*****************************************************
 * @deprecated
 * [seek] launch games, big area !!!
 * 20000 to 30000
 ****************************************************/
#define FENRIR_EVENT_LAUNCH_ID_START_FAD (20000)
#define FENRIR_EVENT_LAUNCH_ID_END_FAD (30000)
/*****************************************************
 * [seek] launch with wifi
 * 9000
 ****************************************************/
#define FENRIR_EVENT_LAUNCH_WIFI (9000)

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
 * [read/select] FILE IO Reading func
 * 5000 to 8101
 ****************************************************/
#define FENRIR_IO_SELECT (5000)
#define FENRIR_IO_READ (8001)
#define FENRIR_IO_DELETE (8101)
#define FENRIR_MAX_READ_IO (8)
#define FENRIR_MAX_IO (3000)

/*****************************************************
 * [read] screenshot area
 * 10000 to 20000
 ****************************************************/
#define FENRIR_SCREENSHOT_ID_START_FAD (10000)
#define FENRIR_SCREENSHOT_ID_END_FAD (13000)

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
    FENRIR_STATUS_BUSY,
    FENRIR_STATUS_ERROR,
};

enum
{
    FENRIR_STATUS_TYPE_UNKNOWN = 0,
    FENRIR_STATUS_TYPE_REFRESH,
    FENRIR_STATUS_TYPE_WRITE_FILE,
    FENRIR_STATUS_TYPE_WIFI
};

enum
{
    FENRIR_WRITE_NOTHING = 0,
    FENRIR_WRITE_PENDING,
    FENRIR_WRITE_CHUNK,
    FENRIR_WRITE_CHUNK_ERROR,    
};

enum
{
    FENRIR_WIFI_DISCONNECTED = 0,
    FENRIR_WIFI_CONNECTING,
    FENRIR_WIFI_CONNECTED
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
        uint32_t full_checksum;
    };
    // Wifi
    struct {
        uint8_t _wifi_status;
        uint8_t _wifi_type;
        uint8_t wifi_state;
    };
} status_sector_t;

static inline uint8_t chunk_checksum(uint8_t *data, uint32_t len)
{
    uint8_t checksum = 0x00;
    for (uint32_t i = 0; i < len; i++)
    {
        checksum ^= data[i];
    }
    return checksum;
}
