
#ifndef GPT_UTILS_H
#define GPT_UTILS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gpt_types.h"
#include "../CRC/crc.h"

/**
 * Validates the integrity of GPT Header 
 * uses CRC32 / IEEE standard
 * given GPT_Header structure
 * returns 0 on valid integrity
 */
uint8_t validate_GPT_Header_CRC(GPT_Header *header ,FILE *fd);
/**
 * Validates the ingegrity of the entry array
 * uses CRC32 / IEEE standard
 * given GPT_Header
 * returns 0 on valid integrity
 */
uint8_t validate_GUID_Partition_Entry_Array_CRC(GPT_Header *header ,FILE *fd);

/**
 * initializes the GPT header with necessary data from a file descriptor , at specific LBA 
 * returns 0 on success
 */
uint8_t init_GPT_Header(GPT_Header *header , uint64_t starting_LBA , FILE *fd);

#endif