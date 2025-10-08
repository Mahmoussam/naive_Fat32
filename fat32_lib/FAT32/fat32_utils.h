#ifndef FAT32_UTILS_H
#define FAT32_UTILS_H

#include <stdio.h>
#include "fat32_types.h"


/**
 * 
 */
uint8_t read_fat32_BPB_header(FAT_All_BPB_Head *fat_head , uint64_t base_addr, FILE *fd);
/**
 * Gets next cluster in Fat32 system from FAT starting at specific address
 * returns 0 @ success
 */
uint8_t get_next_cluster( uint64_t cluster,  uint64_t *next_cluster ,FAT_All_BPB_Head *fat_head ,uint8_t FAT_Num );
#endif