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
    
    if(fat_head->sectors_per_cluster == 0)return 3;
    //read number of FATs
    fseek(fd , base_addr + BPB_NumFATs_OFFSET , SEEK_SET);
    fread(&(fat_head->NumFATs) , sizeof(fat_head->NumFATs) , 1 , fd);
    //read number of reserved sectors
    fseek(fd , base_addr + BPB_RsvdSecCnt , SEEK_SET);
    fread(&(fat_head->RsvdSecCnt) , sizeof(fat_head->RsvdSecCnt) , 1 , fd);
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
    if(sig1 != 0x55 || sig2 != 0xAA)return 4;

    uint64_t non_data_sectors = fat_head->RsvdSecCnt + fat_head->NumFATs * fat_head->FAT_size_in_sectors;
    // calc data_addr
    fat_head->data_addr = base_addr + ((uint64_t)fat_head->sector_size) * (non_data_sectors);
    // calc total number of data clusters
    fat_head->total_data_clusters = fat_head->total_sectors - (non_data_sectors);
    fat_head->total_data_clusters /= fat_head->sectors_per_cluster;

    // calc address of the first FAT table (default)
    fat_head->current_FAT_ADDR = base_addr + fat_head->sector_size * ((uint64_t)fat_head->RsvdSecCnt);

    return 0;
}

uint8_t get_next_cluster_fat32( uint32_t cluster,  uint32_t *next_cluster ,FAT_All_BPB_Head *fat_head  , FILE * fd){
    if(next_cluster == NULL) return 1;
    if(fd == NULL)return 2;
    if(fat_head == NULL) return 3;
    if(cluster < 2 || cluster > fat_head->total_data_clusters)return 4;
    
    // each entry takes 4 bytes..
    fseek(fd , fat_head->current_FAT_ADDR +  (((uint64_t)cluster)<<2) , SEEK_SET);
    //printf("\n!!! %X\n" , fat_head->current_FAT_ADDR +  (((uint64_t)cluster)<<2));
    if(fread(next_cluster , sizeof((*next_cluster)) , 1 , fd))
        return 0;

    return 5;
}

uint8_t get_cluster_address(uint32_t cluster,uint64_t *result_address, FAT_All_BPB_Head * fat_head ){
    if(result_address == NULL)return 1;
    if(fat_head == NULL)return 2;
    if(cluster < 2 || cluster > fat_head->total_data_clusters)return 3;
    *(result_address) = fat_head->data_addr + ((uint64_t)cluster - 2) * ((uint64_t)fat_head->sectors_per_cluster) * ((uint64_t)fat_head->sector_size);
    return 0;
}