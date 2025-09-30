#ifndef CRC_H
#define CRC_H

#include <stdint.h>

//Polynomials/Generators
// source https://en.wikipedia.org/wiki/Cyclic_redundancy_check#Specification
#define CRC8_POLY                           ((uint8_t)0xD5)
#define CRC8_BLUETOOTH_POLY                 ((uint8_t)0xA7)
#define CRC16_CCITT_POLY                    ((uint16_t)0x1021)
#define CRC32_POLY_NORMAL                   ((uint32_t)0x04C11DB7)
#define CRC32_POLY_REVERSED                 ((uint32_t)0xEDB88320)

/**
 * Calculates CRC8 of given data for given generator polynomial 
 * using shift register approach,byte by byte processing
 */
uint8_t CALC_CRC8(uint8_t *data , int sz , uint8_t gen);
uint16_t CALC_CRC16(uint8_t *data , int sz , uint16_t gen);
/**
 * Calculates CRC32 of given data ,using  left shifting approach
 * Follows standard CRC32/IEEE specs
 * Reflected Input
 * Reflected Output
 * XORout = init =  0xFFFFFFFF
 */
uint32_t CALC_CRC32(uint8_t *data , int sz);

/**
 * Calculates CRC8 of given data for given generator polynomial 
 * using lookup table directly
 */
uint8_t CALC_CRC8_TABLE  (uint8_t *data , int sz , const uint8_t *table);
uint16_t CALC_CRC16_TABLE(uint8_t *data , int sz , const uint16_t *table);
uint32_t CALC_CRC32_TABLE(uint8_t *data , int sz , const uint32_t *table);

// Lookup Tables generated and validated by python scripts
extern const uint8_t CRC8_Table[256];
extern const uint16_t CRC16_CCITT_Table[256];
extern const uint32_t CRC32_Table[256];

#endif