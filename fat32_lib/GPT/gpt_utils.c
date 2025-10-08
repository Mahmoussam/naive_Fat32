#include "gpt_utils.h"


const char SIGNATURE_STRING[] = "EFI PART";

/**
 * Validates the integrity of GPT Header 
 * given GPT_Header structure
 * returns 0 on valid integrity
 */
uint8_t validate_GPT_Header_CRC(GPT_Header *header ,FILE *fd){
    //Check the Header crc => CRC32 specs
    if(header == NULL)return 1;
    if(fd == NULL)return 2;
    uint8_t *data = (uint8_t*)malloc(header->header_size);
    if(data == NULL)return 3;

    uint64_t base_addr = header->MyLBA * GPT_SECTOR_SIZE;

    fseeko(fd , base_addr , SEEK_SET);
    int cnt = fread(data , 1 , header->header_size , fd);
    if(cnt != header->header_size){
        free(data);
        return 4;
    }
    uint32_t crc_val = data[16] | (((uint32_t)data[17]) << 8) | (((uint32_t)data[18]) << 16) | (((uint32_t)data[19]) << 24);
    memset(&(data[16]) , 0 , 4);
    uint32_t crc_calc = CALC_CRC32_TABLE(data , cnt , CRC32_Table);
    free(data);

    if(crc_calc != crc_val){
        //puts("!!! INVALID CRC");
        return 5;
    }
   // printf("Valid CRC32 0x%04X , 0x%04X\n !!" ,crc_val , crc_calc );
    return 0;
}
/**
 * Validates the ingegrity of the entry array
 * uses CRC32 / IEEE standard
 * given GPT_Header
 * returns 0 on valid integrity
 */
uint8_t validate_GUID_Partition_Entry_Array_CRC(GPT_Header *header ,FILE *fd){
    if(header == NULL)return 1;
    if(fd == NULL)return 2;
    int BLSZ = header->NumberOfPartitionEntries * header->sizeOfPartitionEntry;
    uint8_t *data = (uint8_t*)malloc(BLSZ);
    if(data == NULL)return 3;

    uint64_t base_addr = header->PartitionEntryArrayLBA * GPT_SECTOR_SIZE;
    fseeko(fd , base_addr , SEEK_SET);
    int cnt = fread(data , 1 , BLSZ , fd);
    if(cnt != BLSZ){
        free(data);
        return 4;
    }
    uint32_t crc_calc = CALC_CRC32_TABLE(data , cnt , CRC32_Table);
    free(data);

    if(crc_calc != header->partitionArrayCRC32){
        //puts("!!! INVALID CRC");
        return 5;
    }
    //printf("Valid CRC32 0x%04X \n !!" , crc_calc );
    return 0;
}

/**
 * initializes the GPT header with necessary data from a file descriptor , at specific LBA 
 * returns 0 on success
 */
uint8_t init_GPT_Header(GPT_Header *header , uint64_t starting_LBA , FILE *fd){
    if(header == NULL)return 1;
    if(fd == NULL)return 2;
    uint64_t base_addr = starting_LBA * GPT_SECTOR_SIZE;

    //validate signature..
    fseeko(fd , base_addr , SEEK_SET);
    uint8_t read_signature[8];
    
    fread(read_signature , 1 , 8 , fd);
    
    for(uint8_t idx = 0;idx < 8;idx++){
        if(read_signature[idx] != SIGNATURE_STRING[idx])return 3;
    }//valid signature

    //get header size in bytes..
    fseeko(fd , base_addr + HEADER_SIZE_OFFSET , SEEK_SET);
    fread(&(header->header_size) , sizeof(header->header_size) , 1 , fd);

    //MY LBA
    fseeko(fd , MY_LBA_OFFSET + base_addr, SEEK_SET);
    fread(&(header->MyLBA) , sizeof(header->MyLBA) , 1 , fd);
    //alertnate LBA
    fread(&(header->AlternateLBA) , sizeof(header->AlternateLBA) , 1 , fd);

    //disk GUID
    fseeko(fd , base_addr + DISK_GUID_OFFSET, SEEK_SET);
    fread(header->DiskGUID , sizeof(header->DiskGUID[0]) , DISK_GUID_SIZE_IN_BYTES  , fd);

    // partition entry stuff..
    fseeko(fd , base_addr + PARTITION_ENTRY_ARRAY_LBA_OFFSET , SEEK_SET);
    fread(&(header->PartitionEntryArrayLBA) , sizeof(header->PartitionEntryArrayLBA) , 1 , fd);
    fread(&(header->NumberOfPartitionEntries) , sizeof(header->NumberOfPartitionEntries) , 1 , fd);
    fread(&(header->sizeOfPartitionEntry) , sizeof(header->sizeOfPartitionEntry) , 1 , fd);
    uint8_t tmp[4];
    fread(tmp , 1 , 4 , fd);
    header->partitionArrayCRC32 = tmp[0] | (((uint32_t)tmp[1]) << 8) | (((uint32_t)tmp[2]) << 16) | (((uint32_t)tmp[3]) << 24);

    //Check that the MyLBA entry points to the LBA that contains the GUID Partition Table
    if(starting_LBA != header->MyLBA){
        return 4;
    }
    if(validate_GPT_Header_CRC(header , fd)){
        return 5;
    }
    return 0;    
}


/**
 * init gpt partition array entry struct with necessary data
 */
uint8_t init_GPT_PartitionArrayEntry(GPT_PartitionArrayEntry *entry , uint64_t base_addr , FILE *fd){
    if(entry == NULL)return 1;
    if(fd == NULL)return 2;
    entry->base_addr = base_addr;
    fseeko(fd , base_addr , SEEK_SET);
    //just read the first byte for now , seems sufficient to determine which is which
    uint8_t partition_type_guid[1];
    
    //fread(partition_type_guid , 1 , PartitionTypeGUID_SIZE , fd);
    fread(partition_type_guid , 1 , 1 , fd);
    
    if(partition_type_guid[0] == 0){
        entry->partition_type =  UNUSED_PARTITION;
    }
    else if(partition_type_guid[0] == 0xC1){
        entry->partition_type =  EFI_SYSTEM_PARTITION;
    }
    else if(partition_type_guid[0] == 0x02){
        entry->partition_type = PART_WITH_LEGACY_MBR;
    }
    else if(partition_type_guid[0] == 0xA2){
        entry->partition_type = MICROSOFT_BASIC_DATA_PARTITION;
    }
    else{
        printf("UNknown PART GUID TYPE Starting wwith 0x%02X \n" ,partition_type_guid[0]);
        return 3;
    }
    fseeko(fd , base_addr  + PARTITION_ENTRY_FIRST_LBA_OFFSET, SEEK_SET);
    fread(&(entry->StartingLBA) , sizeof(entry->StartingLBA) , 1 , fd);
    fread(&(entry->EndingLBA) , sizeof(entry->EndingLBA) , 1 , fd);
    fread(&(entry->First_Attr_byte) , sizeof(entry->First_Attr_byte) , 1 , fd);
    return 0;
}

/**
 * Reads the readable-name of a partition entry to given location
 */
uint8_t read_GPT_Partition_NAME(GPT_PartitionArrayEntry *entry ,char *buffer ,FILE *fd){
    if(entry == NULL)return 1;
    if(fd == NULL)return 2;
    if(buffer == NULL)return 3;
    fseeko(fd , entry->base_addr + PARTITION_ENTRY_NAME_OFFSET , SEEK_SET);
    int cnt = fread(buffer , 1 , PARTITION_ENTRY_NAME_SIZE , fd);
    if(cnt != PARTITION_ENTRY_NAME_SIZE)return 4;
    return 0;
}