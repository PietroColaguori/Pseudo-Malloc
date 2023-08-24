#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/mman.h>
#include <math.h>
#include "bit_map.h"
#include "buddy_allocator.h"
#include "allocator_common.h"

#define PAGE_SIZE 4096
#define MANAGED_MEMORY_SIZE 1 << 20
#define NUM_LEVELS 21
#define MIN_BUCKET_SIZE (MANAGED_MEMORY_SIZE >> (NUM_LEVELS - 1))
#define NUM_BUDDIES ((1 << NUM_LEVELS) - 1)
#define NUM_BITS_BITMAP ((NUM_BUDDIES + 7) << 3)

void* pseudo_malloc(int size);
void pseudo_free(void* address, int size);
void* pseudo_calloc(int num_elements, int size);