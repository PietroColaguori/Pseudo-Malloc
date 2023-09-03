#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <math.h> // for floor and log2
#include <sys/mman.h>
#include "buddy_allocator.h"
#include "bit_map.h"

#define ALLOCATED 1
#define FREE 0

// these are trivial helpers to support you in case you want
// to do a bitmap implementation
int levelIdx(size_t idx){
  return (int)floor(log2(idx));
};

int buddyIdx(int idx){
  if (idx&0x1){
    return idx-1;
  }
  return idx+1;
}

int parentIdx(int idx){
  return (idx - 1) / 2;
}

int startIdx(int idx){
  return (idx-(1<<levelIdx(idx)));
}

int leftChildIdx(int parentIdx) {
  return parentIdx * 2 + 1;
}

int rightChildIdx(int parentIdx) {
  return parentIdx * 2 + 2;
}

int brotherIdx(int idx) {
  return (idx % 2 == 1) ? ++idx : --idx;
}

int idxInLevel(int idx, int level) {
  return idx - ((1 << level) - 1);
}

// the buffer is the one managed by the bitmap
// the bitmap used by the buddy allocator for bookeeping is assigned once number of bits needed is known
void BuddyAllocator_init(BuddyAllocator *balloc, char* mem, int num_levels, int min_bucket_size, uint8_t* buffer) {
  balloc -> mem = (unsigned char*) mem;
  balloc -> num_levels = num_levels;
  balloc -> min_bucket_size = min_bucket_size;

  // compute number of bits needed as 2^(num_levels) - 1
  int num_bits = (1 << num_levels) - 1;

  // initialize the bitmap used by the buddy allocator for bookeeping
  BitMap_init(&balloc -> bit_map, num_bits, buffer);
  // initially all buddies are free
  for(int i = 0; i < num_bits; i++) {
    BitMap_setBit(&balloc -> bit_map, i, FREE);
  }

  // show some info about the buddy allocator just initialized
  printf("================================================\n");
  printf("         Buddy Allocator initialized            \n");
  printf("================================================\n");
  printf("Number of levels:\t%d\n", balloc -> num_levels);
  printf("Minimum size allocable:\t%d\n", balloc -> min_bucket_size);
  printf("Number of bits needed:\t%d\n", num_bits);
  printf("Size of manageable memory:\t%d\n", (1 << (num_levels-1)) * min_bucket_size);
  printf("================================================\n");
}

// returns the level of the smallest bucket that can contain the specified size
// edge case: size requested exceeds maxmimum bucket size
// fail case: returns -1
int BuddyAllocator_getLevel(BuddyAllocator* balloc, int size) {
  // edge case
  int max_bucket_size = balloc -> min_bucket_size * (1 << (balloc -> num_levels - 1));
  if(size > max_bucket_size) {
    printf("Max bucket size: %d, given mbs %d and num levels %d\n", max_bucket_size, balloc -> min_bucket_size, balloc -> num_levels);
    printf("Requested size of %d exceeds maximum bucket size of %d\n", size, max_bucket_size);
    return -1;
  }
  
  // start checking for appropriate level from the highest one
  int current_level = balloc -> num_levels - 1;
  int current_size = balloc -> min_bucket_size;

  // we stop as soon as the current size is enoguh to satisfy the request
  while(current_size < size) {

    // in the binary tree each level holds block of a certain size % 2 == 0
    // moving towards the root (level = 0) means doubling the size of the available blocks at each iteration
    current_size *= 2;
    current_level--;
  }

  printf("Level %d satisfies the request of size %d\n", current_level, size);
  if(current_level < 0) return -1;
  else return current_level;
}

// returns the index of the first free buddy found on a certain level using its bitmap
// the level used is the one computed by the previous function (all blocks on a level have the same size)
// fail case: returns -1 (no free buddies on specified level)

int BuddyAllocator_getFreeBuddy(BitMap* bmap, int level) {
  int start = (1 << level) - 1;      // first buddy of the level
  int end = (1 << (level + 1)) - 1;  // last buddy of the level
  for(int i = start; i < end; i++) {
    if(BitMap_bit(bmap, i) == FREE) {
      printf("Available buddy at level %d found! It has index %d in the BitMap\n", level, i);
      return i;
    }
  }
  printf("No available buddies found at level %d\n", level);
  return -1;
}

// NB: wrapper that uses the getFreeBuddy function as auxiliary function
// Actually changes the values inside the BitMap and, if necessary, performs buddy split
// fail case 1: level == -1 means no level for given size has been found
// fail case 2: free_parent_idx == -1 means no available blocks have been found

int BuddyAllocator_allocateBuddy(BuddyAllocator* balloc, int level) {
  if(level < 0 ) return -1;
  int free_buddy_idx = BuddyAllocator_getFreeBuddy(&(balloc -> bit_map), level);

  // Free buddy has been found on specified level
  if(free_buddy_idx != -1) {
    printf("Allocated buddy %d at level %d\n", free_buddy_idx,level);
    BitMap_setBit(&(balloc -> bit_map), free_buddy_idx, ALLOCATED);

    // if both buddies are occupied mark the parent as occupied
    if(BitMap_bit(&(balloc -> bit_map), leftChildIdx(parentIdx(free_buddy_idx))) == ALLOCATED
       && BitMap_bit(&(balloc -> bit_map), rightChildIdx(parentIdx(free_buddy_idx))) == ALLOCATED) {
         BitMap_setBit(&(balloc -> bit_map), parentIdx(free_buddy_idx), ALLOCATED);
    }

    return free_buddy_idx;
  }
  // At the specified level no free buddies were found, we check the previous one
  else {
    printf("Level %d is all allocated, now checking level %d\n", level, level - 1);
    int free_parent_idx = BuddyAllocator_allocateBuddy(balloc, level - 1);
    if(free_parent_idx == -1) return -1;

    // Check value of parent
    int parent_idx = parentIdx(free_parent_idx);
    printf("Node %d has been split into nodes %d and %d\n", parent_idx, leftChildIdx(parent_idx), rightChildIdx(parent_idx));

    return free_parent_idx;
  }
}

// releases the buddy allocated, making it available again in the bitmap
// also needs to check wether both buddies are free, in that case they will be merged into the parent
void BuddyAllocator_releaseBuddy(BuddyAllocator* balloc, int index) {

  // mark the released buddy as free
  BitMap_setBit(&(balloc -> bit_map), index, FREE);
  printf("Buddy %d has been marked as FREE\n", index);

  // we need to move upwards in the tree checking if both buddies are free
  // at most, this processo continues until the root (index = 0)
  while(index != 0) {

    int parent_idx = parentIdx(index);
    // if both buddies are free, the parent must also be set as free
    if(BitMap_bit(&(balloc -> bit_map), leftChildIdx(parent_idx)) == FREE && BitMap_bit(&(balloc -> bit_map), rightChildIdx(parent_idx)) == FREE) {
      BitMap_setBit(&(balloc -> bit_map), parent_idx, FREE);
    }
    else {
      break;
    }
    index = parent_idx;
  }
}

// gets the starting address based on the buddy allocated
void* BuddyAllocator_getAddress(BuddyAllocator* balloc, int level, int idx)
{
  int idx_in_level = idxInLevel(idx, level);
  int offset = idx_in_level - (1 << level);
  return (void*)(balloc -> mem + offset);
}


// this is the core function, all functions created until now will be used
// returns a certain portion of memory as (void*)
// to find the portion of memory we navigate the bitmap until a suitable buddy is found
void* BuddyAllocator_malloc(BuddyAllocator* balloc, int size) {

  // finds level that satisfies size incremented of a sizeof(int) because we write the index of the block as first thing)
  int level = BuddyAllocator_getLevel(balloc, size + sizeof(int));
  assert(level >= 0);

  // finds the buddy available at the level found
  int free_buddy_idx = BuddyAllocator_allocateBuddy(balloc, level);
  if(free_buddy_idx < 0) {
    printf("There is no space for the request made!\n");
    return NULL;
  }

  int* memory = (int*) BuddyAllocator_getAddress(balloc, level, free_buddy_idx);

  printf("Buddy %d allocated at level %d, its memory address is %p\n", free_buddy_idx, level, memory);
  return (void*)(memory + 1);
}

void BuddyAllocator_freeAllocatedMemory(BuddyAllocator* balloc, void* memory) {

  // in the first 4 bytes we can read the index of the buddy associated
  int index = *((int*) memory - 1);
  assert(index >= 0 && index < balloc -> bit_map.num_bits);
  BuddyAllocator_releaseBuddy(balloc, index);
}

// deallocate all buddies in the Buddy Allocator
void BuddyAllocator_deallocateAll(BuddyAllocator* balloc) {
    // starting from the root (node 0)
    for (int level = balloc->num_levels - 1; level >= 0; level--) {
        int start_idx = (1 << level) - 1;
        int end_idx = (1 << (level + 1)) - 1; 

        for (int idx = start_idx; idx < end_idx; idx++) {
            if (BitMap_bit(&(balloc->bit_map), idx) == ALLOCATED) {
                printf("Deallocating buddy at index: %d\n", idx);
                void* memory = BuddyAllocator_getAddress(balloc, level, idx);
                BuddyAllocator_freeAllocatedMemory(balloc, memory);
            }
        }
    }
}
