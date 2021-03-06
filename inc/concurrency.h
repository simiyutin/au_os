#include <stdatomic.h>
#include <stdint.h>

#ifndef SRC_CONCURRENCY_H
#define SRC_CONCURRENCY_H



struct spinlock {
    atomic_int locked;
};

static struct spinlock threads_lock;

static struct thread * running_thread;
static struct thread master_thread;
static struct thread slave_thread;

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

enum thread_status {
    RUNNING, STOPPED
};

struct thread {
    enum thread_status status;
    struct stack_frame * frame;
};

void lock(struct spinlock*);
void unlock(struct spinlock*);


void threads_init();

struct thread * thread_alloc();
struct thread * thread_create(void (*function)(void *), void *argument);

void thread_run (struct thread * thread_from_run, struct thread * thread_to_run);

void thread_exit();
void thread_wait(struct thread *);

#endif //SRC_CONCURRENCY_H
