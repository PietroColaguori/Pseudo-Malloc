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

void* pseudo_malloc(int size) {
    assert(size > 0);
    void* allocated_mem;

    // small allocation, we will use the buddy allocator
    if(size < PAGE_SIZE / 4) {
        allocated_mem = BuddyAllocator_malloc(&balloc, size);
    }

    // big allocation, we will use mmap
    else {
        printf("The needed size %d is greater than %d\n", size, PAGE_SIZE / 4);
        int* allocated_mem = (int*) mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        assert(allocated_mem != MAP_FAILED);
        allocated_mem[0] = size;
        printf("Address %p of size %d allocated usinng mmap\n", allocated_mem, size);
        return (void*)(allocated_mem + 1);
    }

    return allocated_mem;
}

void pseudo_free(void* address, int size) {
    assert(address != NULL);

    // The address was allocated using a buddy allocator
    if (size < PAGE_SIZE / 4) {
        printf("%d < %d [buddy used]\n", size, PAGE_SIZE / 4);
        BuddyAllocator_freeAllocatedMemory(&balloc, address);
    }
    // The address was allocated using mmap
    else {
        printf("\n%d > %d [munmap used]\n", size, PAGE_SIZE / 4);
        int* adjusted_address = (int*)address - 1;
        assert(munmap((void*)adjusted_address, size) != -1);
        printf("Address %p of size %d deallocated using munmap\n", adjusted_address, size);
    }
}

void* pseudo_calloc(int num_elements, int size) {
    void* allocated_mem = pseudo_malloc(num_elements * size);
    memset(allocated_mem, 0, num_elements * size);
    printf("Allocated and zeroed %d contigous blocks of size %d at address %p\n", num_elements, size, allocated_mem);
    return allocated_mem;
}
