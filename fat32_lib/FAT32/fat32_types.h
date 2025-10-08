/**
 * Holds types and definitions for Fat32 stuff
 * Read: Microsoft FAT Specification paper
 */
#ifndef FAT32_TYPES_H
#define FAT32_TYPES_H

#include <stdint.h>


// VALUES
#define ATTR_READ_ONLY   0x01
#define ATTR_HIDDEN      0x02
#define ATTR_SYSTEM      0x04
#define ATTR_VOLUME_ID   0x08
#define ATTR_DIRECTORY   0x10
#define ATTR_ARCHIVE     0x20

#define ATTR_LONG_NAME        (ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID) // 0x0F
#define ATTR_LONG_NAME_MASK   (ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID | ATTR_DIRECTORY | ATTR_ARCHIVE) // 0x3F
/******************************************** */

// ADDRs
/******************************************** */

// OFFSETS
/******************************************** */

// for details refer to the specs as same naming is used...
// FAT
#define BPB_BytsPerSec_OFFSET (11)
#define BPB_SecPerClus_OFFSET (13)
#define BPB_RsvdSecCnt        (14)
#define BPB_NumFATs_OFFSET    (16)
#define BPB_TotSec32_OFFSET   (32)

//FAT32 specfic exented stuff
#define BPB_FATSz32_OFFSET    (36)
#define BPB_RootClus_OFFSET   (44)

#define BS_VOLID_OFFSET       (67)
#define BS_VOLLAB_OFFSET      (71)
#define BS_FilSysType_OFFSET  (82)
#define SIGNATURE_WORD_OFFSET (510)
// directory structure
#define DIR_CrtTime_OFFSET    (14)
#define DIR_FstClusHI_OFFSET  (20)
#define DIR_FstClusLO_OFFSET  (26)
#define DIR_FileSize_OFFSET   (28)
// Data types
/******************************************** */


/**
 * Structure to hold necessary FAT BPB data
 * only essentials are stored to save the memory (in the case of embedded mcu)
 * compatible with Fat12/16/32
 */
typedef struct{//refers to logical sector 0 !!
    uint64_t base_addr;             //address of the start of Fat32 volume
                                    // BPB address..

    uint64_t data_addr;             // address of the data (Cluster 2)

    uint64_t current_FAT_ADDR;      // address of the current chosen/used FAT table
                                    // default is to initialize it to the first Fat..


    uint32_t FAT_size_in_sectors;   // differs being Fat32 or Fat12/16..
    uint32_t total_sectors;
    uint32_t total_data_clusters;   // total number of clusters used to store actual data
                                    
    uint32_t root_cluster;

    uint16_t sector_size;           //compatible with Fat12/16/32
    uint16_t RsvdSecCnt;

    uint8_t sectors_per_cluster;    //compatible with Fat12/16/32
    uint8_t NumFATs;                //compatible with Fat12/16/32
    
} FAT_All_BPB_Head;


/**
 * Fat32 directory structure
 * Could be a directory , File, or Even just a LFN entry
 * refer to the specs for more details..
 */
typedef struct{

    uint64_t entry_address;     // Address of the Entry for further info fetching
    
    
    uint32_t DIR_FstClus;       // Combines the low and high words
                                // First cluster of this element

    uint32_t current_cluster;   // pointer to the current cluster to be read
                                // on demand , initialized to DIR_FstClus
    uint32_t DIR_FileSize;
    
    
    uint16_t DIR_CrtTime;
    uint16_t DIR_CrtDate;
    uint16_t DIR_LstAccDate;

    uint8_t DIR_Attr;
    char DIR_Name[11];
    
}FAT32_Directory_Entry;

#endif
/**
 * Notes
 * data addr = volume addr + rsv sectors count * sector size + Fat nums * Fat size
 * Fat entry is 32 bits for FAT32 sys
 * FSI structure exists @ logical(not physical) sector 1 
 * max file size is ~4GB on FAT32
 */