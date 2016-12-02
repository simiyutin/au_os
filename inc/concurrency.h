


#include <stdatomic.h>

#ifndef SRC_CONCURRENCY_H
#define SRC_CONCURRENCY_H
struct spinlock {
    atomic_int locked;
};

void lock(struct spinlock*);
void unlock(struct spinlock*);


int thread_create(void (*function)(void *), void *argument);
// правильная сигнатура?
void exit();
void wait(int pid);



#endif //SRC_CONCURRENCY_H
