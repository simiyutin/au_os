#include <stdatomic.h>
#include <concurrency.h>
#include "../inc/concurrency.h"
#include "../inc/alloc.h"
#include <stdint.h>
#include "../inc/memory.h"
#include "../inc/concurrency_util.h"
#include "../inc/print.h"
#include <string.h>
#include "../inc/ints.h"



#define LOCKED 1
#define UNLOCKED 0

#define RSP(x)	__asm__ ("movq %%rsp, %0" : "=rm"(x))



void lock(struct spinlock* lock){
    while (atomic_exchange_explicit(&lock -> locked, LOCKED, memory_order_acquire) == LOCKED);
}

void unlock(struct spinlock* lock) {
    atomic_store_explicit(&lock -> locked, UNLOCKED, memory_order_release);
}

void threads_init() {
    master_thread.status = RUNNING;
}


struct thread * thread_alloc() {
    struct stack_frame * new_stack = mem_alloc(PAGE_SIZE);
    struct thread * new_thread = mem_alloc(sizeof(struct thread));

    new_thread->frame = new_stack;
    new_thread->status = STOPPED;
    return new_thread;
}

struct thread * thread_create(void (*function)(void *), void *argument) {

    struct thread * new_thread = thread_alloc();

    new_thread->frame->r15 = (uint64_t) function;
    new_thread->frame->r14 = (uint64_t) argument;
    new_thread->frame->handler = (uint64_t) &start_thread_handler;

    new_thread->status = STOPPED;

    return new_thread;
}

void thread_run (struct thread * thread_from_run, struct thread * thread_to_run) {

    lock(&threads_lock);

    thread_from_run->status = STOPPED;
    thread_to_run->status = RUNNING;
    running_thread = thread_to_run;

    unlock(&threads_lock);

    switch_thread((uintptr_t) &thread_from_run->frame, (uintptr_t) thread_to_run->frame);
}


void thread_exit (){
    running_thread->status = STOPPED;
}

void thread_wait (struct thread * thread_to_wait) {
    while(thread_to_wait->status == RUNNING) {
        __asm__ volatile("":::"memory");
    }

    mem_free(thread_to_wait->frame);
    mem_free(thread_to_wait);
}


