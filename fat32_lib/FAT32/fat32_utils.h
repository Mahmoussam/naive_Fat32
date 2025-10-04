#ifndef FAT32_UTILS_H
#define FAT32_UTILS_H

#include <stdio.h>
#include "fat32_types.h"
/**
 * 
 */
uint8_t read_fat32_BPB_header(FAT_All_BPB_Head *fat_head , uint64_t base_addr, FILE *fd);


#endif