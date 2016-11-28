//
// Created by boris on 18.11.16.
//
#include <stdatomic.h>

#ifndef SRC_CONCURRENCY_H
#define SRC_CONCURRENCY_H
struct spinlock {
//    atomic_uint next;
//    atomic_uint current;
    atomic_int locked;
};

void lock(struct spinlock*);
void unlock(struct spinlock*);

void lock_default();
void unlock_default();

// семантика должна быть такая - создаем поток и запускаяем там  переданную функцию?
int thread_create();

#endif //SRC_CONCURRENCY_H
