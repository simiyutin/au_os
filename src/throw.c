#include "../inc/throw.h"
#include "../inc/print.h"

void throw_ex(const char * what) {
    printf("\n");
    printf(what);
    printf("\n");
    int a = 1;
    int b = 1 / (a - 1);
    (void *) b;
}

void assert(int condition) {
    if (!condition) throw_ex("assertion failed");
}
