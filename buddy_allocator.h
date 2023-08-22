#pragma once
#include "pool_allocator.h"
#include "linked_list.h"
#include "bit_map.h"

// Structure of Buddy Allocator with BitMap for bookeeping
typedef struct {
  BitMap bit_map;
  uint8_t* mem;
  int num_levels;
  int min_bucket_size;
} BuddyAllocator;

// initializes the buddy allocator, and checks that the buffer is large enough
void BuddyAllocator_init(BuddyAllocator *balloc, char* mem, int num_levels, int min_bucket_size, uint8_t* buffer);

// returns address of the buddy a given index --unused
// char* BuddyAllocator_getBuddyAddress(BuddyAllocator* balloc, int level, int index);

// prints content of memory buffer managed by a buddy allocator --unused
// void BuddyAllocator_printMem(BuddyAllocator* balloc);

// level of smallest buddy that can contain specified size
int BuddyAllocator_getLevel(BuddyAllocator* balloc, int size);

// index of first free buddy found at a certain level, using bitmap
int BuddyAllocator_getFreeBuddy(BitMap* bmap, int level);

// print content of bitmap buffer as hexadecimal single digits
void BuddyAllocator_printBMapStaus(const BuddyAllocator* balloc);

// update bitmap and performs split (wrapper for BuddyAllocator_getFreeBuddy)
int BuddyAllocator_allocateBuddy(BuddyAllocator* balloc, int level);

// releases buddy, updates bitmap and, if necessary, merges two free buddies
void BuddyAllocator_releaseBuddy(BuddyAllocator* balloc, int index);

// returns a pointer to the section of memory corresponding to allocated buffer, given a certain size
void* BuddyAllocator_malloc(BuddyAllocator* balloc, int size);

// frees the memory allocated by the BuddyAllocator_malloc
void BuddyAllocator_freeAllocatedMemory(BuddyAllocator* balloc, void* memory);

// helpers
int levelIdx(size_t idx);