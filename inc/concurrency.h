//
// Created by boris on 18.11.16.
//
#include <stdatomic.h>

#ifndef SRC_CONCURRENCY_H
#define SRC_CONCURRENCY_H
struct spinlock {
    atomic_int locked;
};

void lock(struct spinlock*);
void unlock(struct spinlock*);

// семантика должна быть такая - создаем поток и запускаяем там  переданную функцию?
int thread_create(void (*function)(void *), void *argument);



#endif //SRC_CONCURRENCY_H
