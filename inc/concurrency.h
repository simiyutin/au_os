//
// Created by boris on 18.11.16.
//
#include <stdatomic.h>

#ifndef SRC_CONCURRENCY_H
#define SRC_CONCURRENCY_H
struct spinlock {
    atomic_uint next;
    atomic_uint current;
};

void lock(struct spinlock*);
void unlock(struct spinlock*);
#endif //SRC_CONCURRENCY_H
