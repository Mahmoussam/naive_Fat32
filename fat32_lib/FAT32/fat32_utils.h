#ifndef FAT32_UTILS_H
#define FAT32_UTILS_H

#include <stdio.h>
#include <fcntl.h>
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
/**
 * gets the entries of the current cluster data.
 * moves `current_cluster` to the next cluster in turn.
 * according to the specs , having 0x00 as a prefix of an entry indicates its availability, and availability
 * of the following entries
 * ~~Hence, , `complete_read` indicates whether reading the complete cluster is required (when set to 1)or should halt once available entry
 * reached (parameter set to 0)~~ depracated `complete_read`
 */
uint8_t get_current_directory_cluster_entries(FAT32_Directory_Entry* data_result , FAT_All_BPB_Head *fat_head , FAT32_Directory_Entry *parent_directory , FILE *fd , uint32_t EntriesNums );
/**
 * writes complete file out to output stream 
 */
uint8_t write_complete_file_out(FAT32_Directory_Entry *directory , FILE *disk_fd , int out_fd , FAT_All_BPB_Head * fat_head);
/**
 * Checks if the entry refers to a subdirectory (returning 1)
 * else if file (returns 0)
 * takes the attributes of t he entry
 */
static inline uint8_t is_entry_a_subdirectory(uint8_t attrib){
    return attrib & ATTR_DIRECTORY;    
}
/**
 * checks if entry is just and LFN
 */
static inline uint8_t is_entry_LFN(uint8_t attrib){
    return (attrib & ATTR_LONG_NAME_MASK) == ATTR_LONG_NAME;    
}
/**
 * Writes `size` bytes from given cluster to outstream file descriptor
 * returns 0 on success
 */
uint8_t write_from_cluster_out(uint32_t cluster , uint32_t size , int out_fd , FAT_All_BPB_Head * fat_head , FILE *in_fd);
#endif