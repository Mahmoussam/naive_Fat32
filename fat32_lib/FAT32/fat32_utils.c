#include "fat32_utils.h"
// variables and buffers
static uint8_t writing_buffer[WRITING_BUFFER_SIZE];


// implementations

uint8_t read_fat32_BPB_header(FAT_All_BPB_Head *fat_head , uint64_t base_addr, FILE *fd){
    if(fat_head == NULL)return 1;
    if(fd == NULL)return 2;
    fat_head->base_addr = base_addr;
    
    
    //read bytes per sector
    fseeko(fd , base_addr + BPB_BytsPerSec_OFFSET , SEEK_SET);
    fread(&(fat_head->sector_size) , sizeof(fat_head->sector_size) , 1 , fd);
    //read sector per cluster
    fseeko(fd , base_addr + BPB_SecPerClus_OFFSET , SEEK_SET);
    fread(&(fat_head->sectors_per_cluster) , sizeof(fat_head->sectors_per_cluster) , 1 , fd);
    
    if(fat_head->sectors_per_cluster == 0)return 3;
    //read number of FATs
    fseeko(fd , base_addr + BPB_NumFATs_OFFSET , SEEK_SET);
    fread(&(fat_head->NumFATs) , sizeof(fat_head->NumFATs) , 1 , fd);
    //read number of reserved sectors
    fseeko(fd , base_addr + BPB_RsvdSecCnt , SEEK_SET);
    fread(&(fat_head->RsvdSecCnt) , sizeof(fat_head->RsvdSecCnt) , 1 , fd);
    //read total number of sectors
    fseeko(fd , base_addr + BPB_TotSec32_OFFSET , SEEK_SET);
    fread(&(fat_head->total_sectors) , sizeof(fat_head->total_sectors) , 1 , fd);
    //read single FAT size
    fseeko(fd , base_addr + BPB_FATSz32_OFFSET , SEEK_SET);
    fread(&(fat_head->FAT_size_in_sectors) , sizeof(fat_head->FAT_size_in_sectors) , 1 , fd);
    //read root cluster
    fseeko(fd , base_addr + BPB_RootClus_OFFSET , SEEK_SET);
    fread(&(fat_head->root_cluster) , sizeof(fat_head->root_cluster) , 1 , fd);

    uint8_t sig1 , sig2;
    fseeko(fd , base_addr + SIGNATURE_WORD_OFFSET , SEEK_SET);
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
    
    // calc cluster size in bytes
    fat_head->cluster_size = ((uint32_t)fat_head->sector_size) * ((uint32_t)fat_head->sectors_per_cluster);
    return 0;
}

uint8_t get_next_cluster_fat32( uint32_t cluster,  uint32_t *next_cluster ,FAT_All_BPB_Head *fat_head  , FILE * fd){
    if(next_cluster == NULL) return 1;
    if(fd == NULL)return 2;
    if(fat_head == NULL) return 3;
    if(cluster < 2 || cluster > fat_head->total_data_clusters)return 4;
    
    // each entry takes 4 bytes..
    fseeko(fd , fat_head->current_FAT_ADDR +  (((uint64_t)cluster)<<2) , SEEK_SET);
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

uint8_t read_directory_entry_at_cursor(FAT32_Directory_Entry *entry , FILE * fd){
    if(entry == NULL)return 1;
    if(fd == NULL)return 2;
    // 
    entry->entry_address = ftello(fd);
    // read name
    fread(entry->DIR_Name , 1 , 11 , fd);
    // read attr
    fread(&entry->DIR_Attr , 1 , 1 , fd);
    
    //read date/time of creation and lst access date
    fseeko(fd , entry->entry_address + DIR_CrtTime_OFFSET , SEEK_SET);
    fread(&entry->DIR_CrtTime ,sizeof(entry->DIR_CrtTime) , 1 , fd);
    fread(&entry->DIR_CrtDate ,sizeof(entry->DIR_CrtDate) , 1 , fd);
    fread(&entry->DIR_LstAccDate ,sizeof(entry->DIR_LstAccDate) , 1 , fd);
    //read Fst cluster
    uint8_t tmp_word;
    fseeko(fd , entry->entry_address + DIR_FstClusHI_OFFSET , SEEK_SET);
    fread(&tmp_word , 1 , 1 , fd);
    entry->DIR_FstClus = tmp_word;
    entry->DIR_FstClus <<= 8;
    fseeko(fd , entry->entry_address + DIR_FstClusLO_OFFSET , SEEK_SET);
    fread(&tmp_word , 1 , 1 , fd);
    entry->DIR_FstClus |= tmp_word;

    entry->current_cluster = entry->DIR_FstClus;
    // read file size 
    fseeko(fd , entry->entry_address + DIR_FileSize_OFFSET , SEEK_SET);
    fread(&entry->DIR_FileSize ,sizeof(entry->DIR_FileSize) , 1 , fd);
    
    return 0;
}

uint8_t get_current_directory_cluster_entries(FAT32_Directory_Entry* data_result , FAT_All_BPB_Head *fat_head , FAT32_Directory_Entry *parent_directory , FILE *fd , uint32_t EntriesNums ){
    if(data_result == NULL)return 1;
    if(parent_directory == NULL)return 2;
    if(fd == NULL)return 3;
    if(fat_head == NULL)return 4;
    if(parent_directory->current_cluster == EOF)return 5;
    uint32_t next_cluster;
    uint8_t errc = 0;
    if((errc = get_next_cluster_fat32(parent_directory->current_cluster , &next_cluster , fat_head , fd)))
        return 10 + errc;
    uint64_t start_addr;
    errc = get_cluster_address(parent_directory->current_cluster , &start_addr ,  fat_head);
    fseeko(fd , start_addr , SEEK_SET);
    FAT32_Directory_Entry *ptr = data_result;
    for(uint32_t ii = 0;ii < EntriesNums;ii++){
        read_directory_entry_at_cursor(ptr , fd);
        
        ptr++;
    }
    parent_directory->current_cluster = next_cluster;
    return 0;
}

uint8_t write_complete_file_out(FAT32_Directory_Entry *directory , FILE *in_fd , int out_fd , FAT_All_BPB_Head * fat_head){
    if(directory == NULL)return 1;
    if(in_fd == NULL)return 2;
    //make sure it is a file
    if(is_entry_a_subdirectory(directory->DIR_Attr))
        return 3;

    uint32_t current_cluster = directory->DIR_FstClus;
    if(current_cluster == EOF)
        return 4;
    
    uint32_t remaining_size = directory->DIR_FileSize 
            , target_size;
    uint8_t errc;
    while(current_cluster != EOF && remaining_size){
        target_size = (remaining_size < fat_head->cluster_size ? remaining_size : fat_head->cluster_size);
        
        if((errc = write_from_cluster_out(current_cluster , target_size , out_fd , fat_head , in_fd)))
            return errc + 10;

        remaining_size -= target_size;
        current_cluster = get_next_cluster_fat32(current_cluster , &current_cluster , fat_head , in_fd);
    }

    return 0;
}

uint8_t write_from_cluster_out(uint32_t cluster , uint32_t size , int out_fd , FAT_All_BPB_Head * fat_head , FILE *in_fd){
    if(fat_head == NULL)return 1;
    uint8_t errc;
    uint64_t start_addr;
    errc = get_cluster_address(cluster , &start_addr , fat_head);
    if(errc)
        return errc + 10;
    // if size to write exceeds the actual cluster size !
    if(size > fat_head->cluster_size)
        return 2;
    fseeko(in_fd , start_addr , SEEK_SET);
    uint32_t target_size;
    //printf("\tw_f_c_o:: %u , %u\n" , cluster , size);
    while(size > 0){
        target_size = (size < WRITING_BUFFER_SIZE? size : WRITING_BUFFER_SIZE);
        int read_cnt = fread(writing_buffer , 1 , target_size , in_fd);
        int swritten = write(out_fd , writing_buffer , read_cnt);
        //printf("\t? %u , %u\n" , read_cnt , swritten);
        // for(int i = 0;i < read_cnt;i++){
        //     printf("%02X " , writing_buffer[i]);
        // }puts("");
        if(swritten == -1)
            return 3;
        size -= swritten;
    }
    return 0;
}


