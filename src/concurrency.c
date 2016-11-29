#include <stdatomic.h>
#include <concurrency.h>
#include "../inc/concurrency.h"
#include "../inc/alloc.h"
#include <stdint.h>
#include "../inc/memory.h"
#include "../inc/concurrency_util.h"


#define LOCKED 1
#define UNLOCKED 0

#define RSP(x)	__asm__ ("movq %%rsp, %0" : "=rm"(x))

uint64_t thread_id = 0;

void lock(struct spinlock* lock){
    while (atomic_exchange_explicit(&lock -> locked, LOCKED, memory_order_acquire) == LOCKED);
}

void unlock(struct spinlock* lock) {
    atomic_store_explicit(&lock -> locked, UNLOCKED, memory_order_release);
}



int thread_create(void (*function)(void *), void *argument) {


    uintptr_t new_stack = (uintptr_t) mem_alloc(PAGE_SIZE);

    build_stack((void *) new_stack, function, argument);
    return 0;
    uintptr_t cur_stack;
    RSP(cur_stack);
    switch_thread((void **) &cur_stack, (void *) new_stack);
    return 0;
}
