/**
 * Holds types and definitions for Fat32 stuff
 * Read: Microsoft FAT Specification paper
 */
#ifndef FAT32_TYPES_H
#define FAT32_TYPES_H

#include <stdint.h>


// VALUES
/******************************************** */

// ADDRs
/******************************************** */

// OFFSETS
/******************************************** */

// for details refer to the specs as same naming is used...
// FAT
#define BPB_BytsPerSec_OFFSET (11)
#define BPB_SecPerClus_OFFSET (13)
#define BPB_NumFATs_OFFSET    (16)
#define BPB_TotSec32_OFFSET   (32)

//FAT32 specfic exented stuff
#define BPB_FATSz32_OFFSET    (36)
#define BPB_RootClus_OFFSET   (44)

#define BS_VOLID_OFFSET       (67)
#define BS_VOLLAB_OFFSET      (71)
#define BS_FilSysType_OFFSET  (82)
#define SIGNATURE_WORD_OFFSET (510)

// Data types
/******************************************** */


/**
 * Structure to hold necessary FAT BPB data
 * only essentials are stored to save the memory (in the case of embedded mcu)
 * compatible with Fat12/16/32
 */
typedef struct{//refers to logical sector 0 !!
    uint64_t base_addr;             //compatible with Fat12/16/32
    
    uint32_t FAT_size_in_sectors;// differs being Fat32 or Fat12/16..
    uint32_t total_sectors;
    uint32_t root_cluster;

    uint16_t sector_size;           //compatible with Fat12/16/32
    
    uint8_t sectors_per_cluster;    //compatible with Fat12/16/32
    uint8_t NumFATs;                //compatible with Fat12/16/32
    
} FAT_All_BPB_Head;

// /**
//  * Extended BPB struct for FAT32
//  * necessary data , used beside `FAT_All_BPB_Head`
//  */
// typedef struct{

// } FAT32_Extended_BPB_Head;


#endif