#include <stdatomic.h>
#include <concurrency.h>
#include "../inc/concurrency.h"

void lock(struct spinlock* lock){
    const unsigned ticket = atomic_fetch_add_explicit(&lock ->next, 1, memory_order_relaxed);
    while (atomic_load_explicit(&lock->current, memory_order_acquire) != ticket);
};
void unlock(struct spinlock* lock) {
    const unsigned current = atomic_load_explicit(&lock->current, memory_order_relaxed);

    atomic_store_explicit(&lock -> current, current + 1, memory_order_release);
};

