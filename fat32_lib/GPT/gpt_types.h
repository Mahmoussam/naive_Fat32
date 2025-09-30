
#ifndef GPT_TYPES
#define GPT_TYPES


#include <stdint.h>
#include <stdio.h>

// VALUES
extern const char SIGNATURE_STRING[];
//assume it to be 512 for now ..
#define GPT_SECTOR_SIZE 512
#define DISK_GUID_SIZE_IN_BYTES 16
// OFFSETS and ADDR
#define HEADER_SIZE_OFFSET (12)
#define MY_LBA_OFFSET (24)
#define ALTERNATE_LBA_OFFSET (32)
#define DISK_GUID_OFFSET (56)
#define PARTITION_ENTRY_ARRAY_LBA_OFFSET (72)

//Structs

/**
 * a representation of necessary data about the GPT Partitioning
 * Header.
 * Note that GPT supports 64-bit LBA unlike the MBR supporting 
 * only 32-bit LBA
 */
typedef struct {
    uint64_t MyLBA;
    uint64_t AlternateLBA;
    uint64_t PartitionEntryArrayLBA;
    
    uint32_t header_size;
    uint32_t NumberOfPartitionEntries;
    uint32_t sizeOfPartitionEntry;
    uint32_t partitionArrayCRC32;
    uint8_t DiskGUID[16];
}  GPT_Header;

#endif


/* 
# Notes
GPT block/sector size could be 512B or 4k B
The following test must be performed to determine if a GPT is valid:

    - Check the Signature   done in `init_GPT_Header`

    - Check the Header CRC done in `validate_GPT_Header_CRC`

    - Check that the MyLBA entry points to the LBA that contains the GUID Partition Table done in `init_GPT_Header`

    - Check the CRC of the GUID Partition Entry Array

*/
