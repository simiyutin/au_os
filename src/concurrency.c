#include <stdatomic.h>
#include <concurrency.h>
#include "../inc/concurrency.h"


#define LOCKED 1
#define UNLOCKED 0

static struct spinlock ticket_handler;

void lock(struct spinlock* lock){
    //todo можно и так будет если что
//    const unsigned ticket = atomic_fetch_add_explicit(&lock ->next, 1, memory_order_relaxed);
//    while (atomic_load_explicit(&lock->current, memory_order_acquire) != ticket);
    while (atomic_exchange_explicit(&lock -> locked, LOCKED, memory_order_acquire) == LOCKED);
}

void unlock(struct spinlock* lock) {
//    const unsigned current = atomic_load_explicit(&lock->current, memory_order_relaxed);
//
//    atomic_store_explicit(&lock -> current, current + 1, memory_order_release);
    atomic_store_explicit(&lock -> locked, UNLOCKED, memory_order_release);
}

void lock_default(){
    lock(&ticket_handler);
}

void unlock_default(){
    unlock(&ticket_handler);
}

int thread_create() {
    return 0;
}
