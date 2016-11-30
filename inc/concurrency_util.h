#ifndef SRC_CONCURRENCU_UTIL_H
#define SRC_CONCURRENCU_UTIL_H
void switch_thread(/*void **prev, */unsigned long next);
// в ассемблерном коде ничего не делаю с аргументом, скорее всего херня
void build_stack(void * new_stack, void (*function)(void *), void * argument);
#endif //SRC_CONCURRENCU_UTIL_H
