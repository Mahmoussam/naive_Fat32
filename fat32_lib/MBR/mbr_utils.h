/*
* Holds MBR related functions and utilities 
*
*/

#ifndef MBR_UTILS_H
#define MBR_UTILS_H

#include "mbr_types.h"

/**
 * Goes to addr 0x00 where MBR is assumed to be
 * returns 0 at success
 */
static uint8_t go_to_mbr(FILE *fd);

/**
 * reads partition table from mbr LBA 0
 * returns 0 on success
 */
uint8_t read_partition_tables(MBR_PartitionTableEntry *table , uint8_t *read_cnt , FILE *fd);
/**
 * Validates the last 2 bytes as a signature 0x55AA
 */
static uint8_t validate_mbr_signature(FILE *fd);
#endif