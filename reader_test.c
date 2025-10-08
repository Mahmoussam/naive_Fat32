#include <stdio.h>
#include <stdint.h>

#include "fat32_lib/types.h"
#include "fat32_lib/MBR/mbr_types.h"
#include "fat32_lib/MBR/mbr_utils.h"
#include "fat32_lib/GPT/gpt_types.h"
#include "fat32_lib/GPT/gpt_utils.h"
#include "fat32_lib/FAT32/fat32_utils.h"
/*
cd "d:\explore_FAT\naive_fat32\" ;if ($?) { gcc reader_test.c (Get-ChildItem -Recurse fat32_lib -Filter *.c | ForEach-Object { $_.FullName }) -I./fat32_lib -o reader_test } ;if ($?) { .\reader_test }
*/

/*
    Is my machine big/little endian?
*/
void endianity_check();

int main(){
    //endianity_check();

    FILE *in = fopen("sample_disk2" , "rb");

    if(in == NULL){
        printf("Img not found!\n Aborting..\n");
        return 0;
    }

    //read MBR partition table
    MBR_PartitionTableEntry partitions_table[4];
    int8_t cnt , errc;
    if((errc = read_partition_tables(partitions_table , &cnt , in))){
        printf("Failed with err %d\n",errc);
        return 0;
    }

    printf("\nCould Read %d entries from MBR\n\n" , cnt);
     for(int i = 0;i < cnt;i++){
         printf("Partition #%d: Type: 0x%X\n" , i , partitions_table[i].parition_type);
         printf("    Atrrib: 0x%X\n    Start LBA: 0x%X  Size(sectors):%u\n===========================\n" , partitions_table[i].drive_atrributes , partitions_table[i].start_sector , partitions_table[i].size_in_sectors);        
     }
    uint8_t target_partition = 0xFF;
    for(int i = 0;i < cnt;i++){
        if(partitions_table[i].parition_type == 0xEE){
            target_partition = i;
        }
    }
    if(target_partition == 0xFF){
        printf("No valid partition found");
        return 0;
    }
    printf("Target LBA 0x%X\n" , partitions_table[target_partition].start_sector);
    GPT_Header header;
    errc = init_GPT_Header(&header , partitions_table[target_partition].start_sector ,in);
    if(errc){
        puts("unable to read gpt header");
    }
    printf("LBA %lld , Alterna LBA %lld \n" , header.MyLBA , header.AlternateLBA );
    printf("Header size: %u \n" , header.header_size);
    for(int i = 0;i < 16;i++){
        printf("%X " , header.DiskGUID[i]);
    }
    puts("");
    printf("Partition Entry Array LBA: 0x%X \n" , header.PartitionEntryArrayLBA);
    printf("Num of Entries: %u \n" , header.NumberOfPartitionEntries);
    printf("Size of an Entry: %u \n" , header.sizeOfPartitionEntry);
    printf("partition array crc32: 0x%04X \n" , header.partitionArrayCRC32);

    puts("\nValidating partition entry arry crc!");
    errc = validate_GUID_Partition_Entry_Array_CRC(&header , in);
    if(errc ){
        puts("invalid crc!");
        printf("%d errc\n" , errc);
        return 0;
    }
    else puts("Valid array : ] ");
    puts("============================================================");
    printf("\tReading Partition Array Entries\n\n");
    int valid_entires = 0;
    long long offset = header.PartitionEntryArrayLBA * GPT_SECTOR_SIZE;
    uint64_t fat32_vol_addr = -1;
    for(int i = 0;i < header.NumberOfPartitionEntries ;i++){
        GPT_PartitionArrayEntry entry;
        errc = init_GPT_PartitionArrayEntry(&entry , offset , in);
        if(errc)continue;
        if(entry.partition_type == UNUSED_PARTITION){
            continue;
        }
        printf("[%d] type{%d} , attr{%x} , LBA_START{0x%X} , LBA_END{0x%X} \n" , i , entry.partition_type , entry.First_Attr_byte , entry.StartingLBA , entry.EndingLBA);
        char part_name[PARTITION_ENTRY_NAME_SIZE];
        errc = read_GPT_Partition_NAME(&entry , part_name , in);
        //part_name[PARTITION_ENTRY_NAME_SIZE ] = 0;
        if(errc == 0)
            {
                printf("\t Name>");
                for(int ptr = 0;ptr < PARTITION_ENTRY_NAME_SIZE;ptr++){
                    if(part_name[ptr] == 0)continue;
                    printf("%c" , part_name[ptr]);
                }
                puts("");

                fat32_vol_addr = entry.StartingLBA * GPT_SECTOR_SIZE;
            }
        offset += header.sizeOfPartitionEntry;
    }
    puts("======================================");
    printf("\tReading Fat32 Volume\n");
    FAT_All_BPB_Head fat_head;
    
    errc = read_fat32_BPB_header(&fat_head , fat32_vol_addr , in);
    if(errc == 0){
        printf("\t[!] sector size %u\n\
        [!] total sectors %u\n\
        [!] Number of FATS %u\n\
        [!] Size of a FAT in sectors %u\n\
        [!] Sectors per cluster %u\n\
        [x] ROOT Cluster @ 0x%X\n\
        [!] Rsvdcnt %u\n\
        [#] Data Addr 0x%X\n\
        [#] cnt of data Clusters %u\n\
        [x] FAT 1 addr @ 0x%X\n"
        , fat_head.sector_size , fat_head.total_sectors , fat_head.NumFATs , fat_head.FAT_size_in_sectors
        , fat_head.sectors_per_cluster
        , fat_head.root_cluster
        , fat_head.RsvdSecCnt
        , fat_head.data_addr 
        , fat_head.total_data_clusters
        , fat_head.current_FAT_ADDR);
    }
    else{
        printf("Failed to read FAt32 @ %0X with errc %u\n" , fat32_vol_addr , errc);
        return 1;
    }
    puts("=======================================");
    uint32_t tmpcl;
    errc = get_next_cluster_fat32(2 , &tmpcl , &fat_head , in);
    printf("[!] Test next cluster to #2 is %u     >%u\n",tmpcl,errc);
    errc = get_next_cluster_fat32(0x6 , &tmpcl , &fat_head , in);
    printf("[!] Test next cluster to 0x6 is %u     >%u\n",tmpcl,errc);
    puts("=======================================");
    uint64_t tmpaddr;
    errc = get_cluster_address(2 , &tmpaddr , &fat_head);
    printf("[!] Test cluster address calc of #2 is 0x%X\n" , tmpaddr);

    errc = get_cluster_address(6 , &tmpaddr , &fat_head);
    printf("[!] Test cluster address calc of #6 is 0x%X\n" , tmpaddr);

    puts("======================================");
    printf("\tReading Root directory\n");
    // root directory structure:
    FAT32_Directory_Entry root_dir;
    root_dir.DIR_FstClus = root_dir.current_cluster = fat_head.root_cluster;

    //read it..
    uint32_t dir_entries_per_cluster = fat_head.sector_size * fat_head.sectors_per_cluster / 32;
    FAT32_Directory_Entry *dir_table = malloc(dir_entries_per_cluster * sizeof(FAT32_Directory_Entry))
        , *ptr;
    while((errc = get_current_directory_cluster_entries(dir_table , &fat_head , &root_dir , in , dir_entries_per_cluster)) == 0){
        int cnt = 0;
        ptr = dir_table;
        for(int i = 0;i < dir_entries_per_cluster;i++){
            if(ptr->DIR_Attr){
                cnt++;
                //print its data..
                printf("[*] ");
                for(int d = 0;d < 11;d++)if(ptr->DIR_Name[d] == 0)continue;
                                        else printf("%c" , ptr->DIR_Name[d]);
                printf(" @ 0x%X | Attr: 0x%02X | Size : %u\n"  , ptr->DIR_FstClus , ptr->DIR_Attr , ptr->DIR_FileSize);
            }
            ptr++;
        }
        if(cnt)
            puts("- - - - - - - - - - - - - - - - - - -");
    }
    fclose(in);
    return 0;
}
void endianity_check(){
    uint16_t tmp = 1;
    if((*(char *)&tmp) == 1){
        puts("Little Endian");
    }else{
        puts("Big Endian");
    }
}
