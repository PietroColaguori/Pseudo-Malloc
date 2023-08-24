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
#include "pseudo_malloc.h"
#include "allocator_common.h"
#define SEPARATOR "--------------------------------------------------"

// gcc bit_map.c buddy_allocator.c pseudo_malloc.c test.c -lm -o t

char memory[MANAGED_MEMORY_SIZE];
uint8_t buffer[NUM_BITS_BITMAP];
BuddyAllocator balloc;

int main() {
    BuddyAllocator_init(&balloc, memory, NUM_LEVELS, MIN_BUCKET_SIZE, buffer);

    printf("TEST CASE 1: buddy_allocator_malloc and buddy_allocator_free\n\n");

	int* ptr1 = (int*) pseudo_malloc(sizeof(int));
	assert(ptr1 != NULL);
	printf("Allocated memory: %p\n\n", ptr1);

	int* ptr2 = (int*) pseudo_malloc(2 * sizeof(int));
	assert(ptr2 != NULL);
	printf("Allocated memory: %p\n\n", ptr2);

	int* ptr3 = (int*) pseudo_malloc(500);
	assert(ptr3 != NULL);
	printf("Allocated memory: %p\n\n", ptr3);

    // Edge case: 1 byte requested (Maximum space wasted)
    int* ptr4 = (int*) pseudo_malloc(1);
    assert(ptr4 != NULL);
    printf("Allocated memory: %p\n\n", ptr4);

    // Edge case: PAGE_SIZE / 4 (1 KB) (Minimum space wasted)
    int* ptr5 = (int*) pseudo_malloc((PAGE_SIZE / 4) - sizeof(int));
    assert(ptr5 != NULL);
    printf("Allocated memory: %p\n\n", ptr5);

	pseudo_free(ptr1, sizeof(int));

	pseudo_free(ptr2, 2 * sizeof(int));

	pseudo_free(ptr3, 500);

    pseudo_free(ptr4, 1);

    pseudo_free(ptr5, (PAGE_SIZE / 4) - sizeof(int));

	printf(SEPARATOR);

	printf("\n\nTEST CASE 2: mmap and munmap\n\n");
	ptr1 = (int*) pseudo_malloc(1 << 10);
	assert(ptr1 != NULL);
	printf("Allocated memory: %p\n", ptr1);

	ptr2 = (int*) pseudo_malloc(1 << 11);
	assert(ptr2 != NULL);
	printf("Allocated memory: %p\n", ptr2);

    // Allocate 1 MB
    ptr3 = (int*) pseudo_malloc(1024 * 1024);
    assert(ptr3 != NULL);
    printf("Allocated memory: %p\n", ptr3);

    // Allocate more than 1 MB (2 MB)
    ptr4 = (int*) pseudo_malloc(1024 * 1024 * 2);
    assert(ptr4 != NULL);
    printf("Allocated memory: %p\n", ptr4);

	pseudo_free(ptr1, 1 << 10);

	pseudo_free(ptr2, 1 << 11);

    pseudo_free(ptr3, 1024 * 1024);

    pseudo_free(ptr4, 1024 * 1024 * 2);

    printf(SEPARATOR);

    printf("\n\nTEST CASE 3: pseudo_calloc\n\n");

    ptr1 = (int*) pseudo_calloc(10, 4);
    assert(ptr1 != NULL);
    pseudo_free(ptr1, 10 * 4);

    ptr2 = (int*) pseudo_calloc(2, 2500);
    assert(ptr2 != NULL);
    pseudo_free(ptr2, 2500 * 2);
 }