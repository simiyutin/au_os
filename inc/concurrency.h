


#include <stdatomic.h>

#ifndef SRC_CONCURRENCY_H
#define SRC_CONCURRENCY_H
struct spinlock {
    atomic_int locked;
};

struct spinlock memory_lock;

void lock(struct spinlock*);
void unlock(struct spinlock*);


int thread_create(void (*function)(void *), void *argument);



#endif //SRC_CONCURRENCY_H
