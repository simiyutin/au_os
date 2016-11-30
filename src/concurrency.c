#include <stdatomic.h>
#include <concurrency.h>
#include "../inc/concurrency.h"
#include "../inc/alloc.h"
#include <stdint.h>
#include "../inc/memory.h"
#include "../inc/concurrency_util.h"
#include "../inc/print.h"



#define LOCKED 1
#define UNLOCKED 0

#define RSP(x)	__asm__ ("movq %%rsp, %0" : "=rm"(x))



void lock(struct spinlock* lock){
    while (atomic_exchange_explicit(&lock -> locked, LOCKED, memory_order_acquire) == LOCKED);
}

void unlock(struct spinlock* lock) {
    atomic_store_explicit(&lock -> locked, UNLOCKED, memory_order_release);
}



int thread_create(void (*function)(void *), void *argument) {


    uintptr_t new_stack = (uintptr_t) mem_alloc(PAGE_SIZE);

    build_stack((void *) new_stack, function, argument);

    printf("stack candidate:%lx\n", (unsigned long) new_stack);

    uintptr_t cur_stack = 0;
    printf("before:%lx\n", (unsigned long) cur_stack);
    RSP(cur_stack);

    printf("after:%lx\n", (unsigned long) cur_stack);

    uintptr_t * prev_stack = NULL;

    switch_thread((void **) prev_stack, (unsigned long) new_stack);

    printf("at the end:%lx\n", (unsigned long) *prev_stack);
    return 0;
}
