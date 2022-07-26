#pragma once
#include <stdint.h>

#define SD_MENU_FILENAME_LENGTH 58

#define SD_DIR_FLAG_DIRECTORY    (1<<31)

/*****************************************************
 * sd_dir
 ****************************************************/

// 64bytes
typedef struct
{
    uint16_t id;
    uint32_t flag;
    char filename[SD_MENU_FILENAME_LENGTH];
} __attribute__((packed)) sd_dir_entry_t;

typedef union
{
    struct
    {
        uint32_t magic;
        uint16_t count;
        char firmware_version[10];
        uint8_t hw_rev;
        uint8_t region;
        uint8_t auto_reload;
        uint8_t sd_card_status;
        char build_date[20];
        // June 2021
        uint8_t use_cover;
        // July 2021        
        uint8_t have_wifi;
        // August 2021        
        uint8_t wifi_state; 
        uint8_t local_ip[4];
        // Dec 2021
        char sd_name[10];
        char sd_type[10];
        uint32_t sd_size;
        // Fev 2022
        uint32_t card_cpld_id;
        uint8_t card_uid[4];        
    } hdr;
    uint8_t byte[2048];
} __attribute__((packed)) sd_dir_t;
