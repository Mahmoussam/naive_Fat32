#include "mbr_utils.h"


/**
 * Goes to addr 0x00 where MBR is assumed to be
 * returns 0 at success
 */
/*
static uint8_t go_to_mbr(FILE *fd){
    if(fd == NULL)return 1;
    fseek(fd , MBR_ADDR , SEEK_SET);
    return 0;
}   
*/
/**
 * reads partition table from mbr LBA 0
 * returns 0 on success
 */
uint8_t read_partition_tables(MBR_PartitionTableEntry *table , uint8_t *read_cnt , FILE *fd){
    if(fd == NULL)return 1;
    // //find mbr
    // if(go_to_mbr(fd))return 2;
    //go to partitions table
    fseek(fd , MBR_PARTITION_TABLE_ADDR , SEEK_SET);
    //read entries
    *read_cnt = (uint8_t)fread(table , sizeof(MBR_PartitionTableEntry) , MAX_PARTITIONS_ENTRIES , fd);
    if(*read_cnt == 0)return 3;
    //validate signature..
    if(validate_mbr_signature(fd))return 4;
    return 0;
}
/**
 * Validates the last 2 bytes as a signature 0x55AA
 */
static uint8_t validate_mbr_signature(FILE *fd){
    #warning shall we check for fd == null here ?! or save the hustle
    fseek(fd , MBR_SIGNATURE_ADDR , SEEK_SET);
    uint8_t signature[2];
    fread(signature , sizeof(uint8_t) , 2 , fd);
    if(signature[0] ==  VALID_SIGNATURE_BYTE0 && signature[1] ==  VALID_SIGNATURE_BYTE1)return 0;
    return 1;
}