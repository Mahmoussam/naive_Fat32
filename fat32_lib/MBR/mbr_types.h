
#ifndef MBR_TYPES
#define MBR_TYPES


#include <stdint.h>
#include <stdio.h>

// VALUES
#define MAX_PARTITIONS_ENTRIES 4
#define VALID_SIGNATURE_BYTE0 0X55
#define VALID_SIGNATURE_BYTE1 0XAA
// OFFSETS and ADDR
#define MBR_ADDR (0)
#define MBR_PARTITION_TABLE_OFFSET (0x1BE)
#define MBR_SIGNATURE_OFFSET (0x1FE)

#define MBR_PARTITION_TABLE_ADDR (MBR_ADDR + MBR_PARTITION_TABLE_OFFSET)
#define MBR_SIGNATURE_ADDR (MBR_ADDR + MBR_SIGNATURE_OFFSET)
/*
 MBR partition entry wrapper , 16 bytes
 for details regarding partition types: https://en.wikipedia.org/wiki/Partition_type
*/
#pragma pack(push, 1)
typedef struct {
    uint8_t drive_atrributes;
    uint8_t CHS_start[3];
    uint8_t parition_type;
    uint8_t CHS_end[3];
    uint32_t start_sector;
    uint32_t size_in_sectors;
}  MBR_PartitionTableEntry;
#pragma pack(pop)
#endif
/**
 * Notes
 * MBR Always assumes secotr size to be 512 Bytes
 */