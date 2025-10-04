#include "fat32_utils.h"


uint8_t read_fat32_BPB_header(FAT_All_BPB_Head *fat_head , uint64_t base_addr, FILE *fd){
    if(fat_head == NULL)return 1;
    if(fd == NULL)return 2;
    fat_head->base_addr = base_addr;
    
    
    //read bytes per sector
    fseek(fd , base_addr + BPB_BytsPerSec_OFFSET , SEEK_SET);
    fread(&(fat_head->sector_size) , sizeof(fat_head->sector_size) , 1 , fd);
    //read sector per cluster
    fseek(fd , base_addr + BPB_SecPerClus_OFFSET , SEEK_SET);
    fread(&(fat_head->sectors_per_cluster) , sizeof(fat_head->sectors_per_cluster) , 1 , fd);
    //read number of FATs
    fseek(fd , base_addr + BPB_NumFATs_OFFSET , SEEK_SET);
    fread(&(fat_head->NumFATs) , sizeof(fat_head->NumFATs) , 1 , fd);
    //read total number of sectors
    fseek(fd , base_addr + BPB_TotSec32_OFFSET , SEEK_SET);
    fread(&(fat_head->total_sectors) , sizeof(fat_head->total_sectors) , 1 , fd);
    //read single FAT size
    fseek(fd , base_addr + BPB_FATSz32_OFFSET , SEEK_SET);
    fread(&(fat_head->FAT_size_in_sectors) , sizeof(fat_head->FAT_size_in_sectors) , 1 , fd);
    //read root cluster
    fseek(fd , base_addr + BPB_RootClus_OFFSET , SEEK_SET);
    fread(&(fat_head->root_cluster) , sizeof(fat_head->root_cluster) , 1 , fd);

    uint8_t sig1 , sig2;
    fseek(fd , base_addr + SIGNATURE_WORD_OFFSET , SEEK_SET);
    fread(&sig1 , 1 , 1 , fd);
    fread(&sig2 , 1 , 1 , fd);
    if(sig1 != 0x55 || sig2 != 0xAA)return 3;

    return 0;
}