#include <stdio.h>
#include <stdint.h>

#include "fat32_lib/types.h"
#include "fat32_lib/MBR/mbr_types.h"
#include "fat32_lib/MBR/mbr_utils.h"
#include "fat32_lib/GPT/gpt_types.h"
#include "fat32_lib/GPT/gpt_utils.h"

/*
cd "d:\explore_FAT\naive_fat32\" ;if ($?) { gcc reader_test.c (Get-ChildItem -Recurse fat32_lib -Filter *.c | ForEach-Object { $_.FullName }) -I./fat32_lib -o reader_test } ;if ($?) { .\reader_test }
*/

/*
    Is my machine big/little endian?
*/
void endianity_check();

int main(){
    //endianity_check();

    FILE *in = fopen("my_usb_disk_physical" , "rb");

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

    //printf("\nCould Read %d entries from MBR\n\n" , cnt);
    // for(int i = 0;i < cnt;i++){
    //     printf("Partition #%d: Type: 0x%X\n" , i , partitions_table[i].parition_type);
    //     printf("    Atrrib: 0x%X\n    Start LBA: 0x%X  Size(sectors):%u\n===========================\n" , partitions_table[i].drive_atrributes , partitions_table[i].start_sector , partitions_table[i].size_in_sectors);        
    // }
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
