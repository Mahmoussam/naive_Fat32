# Naive FAT32

As the name suggests, this is a small library I developed with the intention of exploring the low-level aspects of file systems at the physical level.
Also, gaining the ability to tweak it further for small memory microcontrollers (Embedded systems). Thus, memory optimization was favored over speed or buffering.

Currently, it can read physical disk images.
Despite the focus on MBR-protected GPT partitioned implementations, the utility provides a base skeleton that welcomes extensions to various types of partitioning.

The CRC module I used was independently repo-ed [here](https://github.com/Mahmoussam/CRC_SIMPLE)

## Stuff implemented so far:
  -  MBR Structures and Utilities.
  -  GPT structures and utilities.
  -  CRC32 validation of GPT header and PartitionEntriesArray as [per](https://uefi.org/specs/UEFI/2.10/05_GUID_Partition_Table_Format.html#gpt-header)
  -  FAT32 structures and utilities Enabling:
    - determines the Sector/Cluster sizing from raw data.
    - reading BPB structures and FAT tables.
    - reading directory entries.
    - extracting files from the disk image.
     
## Assumptions :
  -  GPT sector size is assumed to be 512B
  -  Little Endian machine (can be endianity independent soon)
  -  GPT Partitioned, MBR protected (Can be extended to support other types)

For demonstration, `reader_test.c` tests all utilities and functions developed so far.
It takes a sample physical disk image (I used HxD to obtain it) and extracts significant data.

May build a higher-level wrapper or tweak it more for embedded systems applications.

## Valuable resources if you wanna learn:
  The FAT specs paper by Microsoft is ofc the way to go , but you may need preparatory sources first:
  ```
  https://wiki.osdev.org/MBR_%28x86%29
  https://en.wikipedia.org/wiki/Master_boot_record#PTE
  https://codeandlife.com/2012/04/02/simple-fat-and-sd-tutorial-part-1/
  http://www.compuphase.com/mbr_fat.htm
  http://www.tavi.co.uk/phobos/fat.html
  https://uefi.org/specs/UEFI/2.10/05_GUID_Partition_Table_Format.html
```
![.](https://github.com/Mahmoussam/naive_Fat32/blob/master/Screenshot%202025-10-10%20162530.png)
![.](https://github.com/Mahmoussam/naive_Fat32/blob/master/Screenshot%202025-10-10%20162300.png)
