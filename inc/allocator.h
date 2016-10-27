#ifndef SRC_ALLOCATOR_H
#define SRC_ALLOCATOR_H

#include <stdint.h>
#include <stddef.h>

static const size_t MEMORY_REGIONS_NUM = 16; // Больше, чем в карте

static enum TYPE{RESERVED,FREE};

static struct memory_region {
    uint64_t begin;
    uint64_t end;
};

static struct memory_map {
    struct memory_region * memory_regions;
    struct memory_region * reserved_memory_regions;
    size_t total_number;
    size_t reserved_number;
};

void add_region(uint64_t begin, uint64_t end, enum TYPE);
void reserve_region(uint64_t begin, uint64_t end);
void print_my_map();


#endif //SRC_ALLOCATOR_H
