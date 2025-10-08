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
uint8_t get_next_cluster_fat32( uint32_t cluster,  uint32_t *next_cluster ,FAT_All_BPB_Head *fat_head  , FILE * fd);
/**
 * Calculates the cluster address 
 */
uint8_t get_cluster_address(uint32_t cluster,uint64_t *result_address, FAT_All_BPB_Head * fat_head );
/**
 * Reads the current 32 bytes directory entry
 * the cursor of file stream input must point to offset of the entry
 */
uint8_t read_directory_entry_at_cursor(FAT32_Directory_Entry *entry , FILE * fd);
#endif