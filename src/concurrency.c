#include <stdatomic.h>
#include <concurrency.h>
#include "../inc/concurrency.h"
#include "../inc/alloc.h"
#include <stdint.h>
#include "../inc/memory.h"
#include "../inc/concurrency_util.h"
#include "../inc/print.h"
#include <string.h>



#define LOCKED 1
#define UNLOCKED 0

#define RSP(x)	__asm__ ("movq %%rsp, %0" : "=rm"(x))



void lock(struct spinlock* lock){
    while (atomic_exchange_explicit(&lock -> locked, LOCKED, memory_order_acquire) == LOCKED);
}

void unlock(struct spinlock* lock) {
    atomic_store_explicit(&lock -> locked, UNLOCKED, memory_order_release);
}


struct stack_frame {
    uint64_t rflags;
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t rbp;
    uint64_t rbx;
    uint64_t handler;

} __attribute__((packed));



int thread_create(void (*function)(void *), void *argument) {

    struct stack_frame * new_stack = mem_alloc(PAGE_SIZE);

    new_stack->r15 = (uint64_t) function;
    new_stack->r14 = (uint64_t) argument;
    new_stack->handler = (uint64_t) &start_thread_handler;

    uintptr_t cur_stack = 0;
    RSP(cur_stack);

    switch_thread((unsigned long) new_stack);

    printf("I will not be called\n");

    return 0;
}
