#include <stdatomic.h>
#include <concurrency.h>
#include "../inc/concurrency.h"


#define LOCKED 1
#define UNLOCKED 0

void lock(struct spinlock* lock){
    while (atomic_exchange_explicit(&lock -> locked, LOCKED, memory_order_acquire) == LOCKED);
}

void unlock(struct spinlock* lock) {
    atomic_store_explicit(&lock -> locked, UNLOCKED, memory_order_release);
}

int thread_create() {
    return 0;
}
