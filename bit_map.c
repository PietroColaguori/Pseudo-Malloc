#include <assert.h>
#include "bit_map.h"
#include <string.h>

// returns the number of bytes to store #bits
int BitMap_getBytes(int bits) {
	return bits / 8 + ((bits % 8) != 0);
}

// creates the bitmap on an external array
void BitMap_init(BitMap *bit_map, int num_bits, uint8_t *buffer) {
	bit_map->buffer = buffer;
	bit_map->num_bits = num_bits;
	bit_map->buffer_size = BitMap_getBytes(num_bits);
	memset(bit_map->buffer, 0, bit_map->buffer_size);
}

// sets the status of the bit (occupied = 1, free = 0)
void BitMap_setBit(BitMap *bit_map, int bit_num, int status)
{
	int byte_num = bit_num >> 3;
	assert(byte_num < bit_map->buffer_size);
	int bit_in_byte = bit_num & 0x03;
	if (status) {
		bit_map->buffer[byte_num] |= (1 << bit_in_byte);
	}
	else {
		bit_map->buffer[byte_num] &= ~(1 << bit_in_byte);
	}
}

// checks the status of bit bit_num
int BitMap_bit(const BitMap *bit_map, int bit_num) {
	int byte_num = bit_num >> 3;
	assert(byte_num < bit_map->buffer_size);
	int bit_in_byte = bit_num & 0x03;
	return (bit_map->buffer[byte_num] & (1 << bit_in_byte)) != 0;
}