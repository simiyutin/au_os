
#include <allocator.h>
#include "../inc/allocator.h"
#include "../inc/print.h"

static struct memory_region memory_regions[16];
static struct memory_region reserved_regions[16];
static struct memory_map mmap = {memory_regions,reserved_regions,0,0};

void add_region(uint64_t begin, uint64_t end, enum TYPE type){
    mmap.memory_regions[mmap.total_number].begin = begin;
    mmap.memory_regions[mmap.total_number].end = end;
    ++mmap.total_number;
    if (type == RESERVED) {
        mmap.reserved_memory_regions[mmap.reserved_number].begin = begin;
        mmap.reserved_memory_regions[mmap.reserved_number].end = end;
        ++mmap.reserved_number;
    }
}

void print_my_map() {

    printf("all_regions:\n");
    for (int i = 0; i < mmap.total_number; ++i) {
        printf("new region!\n");
        printf("region start! %x\n", mmap.memory_regions[i].begin);
        printf("region end! %x\n", mmap.memory_regions[i].end);
    }

    printf("reserved regions (may be with intersections):\n");

    for (int i = 0; i < mmap.reserved_number; ++i) {
        printf("new region!\n");
        printf("reserved region start! %x\n", mmap.reserved_memory_regions[i].begin);
        printf("reserved region end! %x\n", mmap.reserved_memory_regions[i].end);
    }

}

void reserve_region(uint64_t begin, uint64_t end) {
    mmap.reserved_memory_regions[mmap.reserved_number].begin = begin;
    mmap.reserved_memory_regions[mmap.reserved_number].end = end;
    ++mmap.reserved_number;
}