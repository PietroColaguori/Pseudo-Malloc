#pragma once
#include <stdint.h>

// structure of the bitmap
typedef struct {
	uint8_t *buffer; // externally allocated buffer
	int buffer_size;
	int num_bits;
} BitMap;

// returns the number of bytes to store #bits
int BitMap_getBytes(int bits);

// creates the bitmap on an external array
void BitMap_init(BitMap *bit_map, int num_bits, uint8_t *buffer);

// sets the status of the bit (occupied = 1, free = 0)
void BitMap_setBit(BitMap *bit_map, int bit_num, int status);

// checks the status of bit bit_num
int BitMap_bit(const BitMap *bit_map, int bit_num);