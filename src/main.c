#include <serial.h>
#include <memory.h>
#include <balloc.h>
#include <paging.h>
#include <debug.h>
#include <alloc.h>
#include <print.h>
#include <ints.h>
#include <time.h>
#include "../inc/string.h"
#include "../inc/alloc.h"
#include "../inc/debug.h"
#include "../inc/ints.h"
#include "../inc/print.h"
#include "../inc/memory.h"
#include "../inc/paging.h"
#include "../inc/list.h"
#include "../inc/kernel.h"
#include "../inc/serial.h"
#include "../inc/time.h"
#include "../inc/balloc.h"
#include "../inc/concurrency.h"
#include "../inc/ramfs.h"
#include "../inc/throw.h"

static void qemu_gdb_hang(void) {
#ifdef DEBUG
    static volatile int wait = 1;

    while (wait);
#endif
}

static void test_kmap(void) {
    const size_t count = 1024;
    struct page **pages = mem_alloc(sizeof(*pages) * count);
    size_t i;

    BUG_ON(!pages);
    for (i = 0; i != count; ++i) {
        pages[i] = __page_alloc(0);
        if (!pages[i])
            break;
    }

    char *ptr = kmap(pages, i);

    BUG_ON(!ptr);
    BUG_ON((uintptr_t) ptr < HIGHER_BASE);

    for (size_t j = 0; j != i * PAGE_SIZE; ++j)
        ptr[i] = 13;

    for (size_t j = 0; j != i * PAGE_SIZE; ++j)
        BUG_ON(ptr[i] != 13);

    kunmap(ptr);
    mem_free(pages);
}

static void test_alloc(void) {
    struct list_head head;
    unsigned long count = 0;

    list_init(&head);
    while (1) {
        struct list_head *node = mem_alloc(sizeof(*node));

        if (!node)
            break;
        BUG_ON((uintptr_t) node < HIGHER_BASE);
        ++count;
        list_add(node, &head);
    }

    printf("Allocated %lu bytes\n", count * sizeof(head));

    while (!list_empty(&head)) {
        struct list_head *node = head.next;

        BUG_ON((uintptr_t) node < HIGHER_BASE);
        list_del(node);
        mem_free(node);
    }

    mem_alloc_shrink();
}

static void test_slab(void) {
    struct list_head head;
    struct mem_cache cache;
    unsigned long count = 0;

    list_init(&head);
    mem_cache_setup(&cache, sizeof(head), sizeof(head));
    while (1) {
        struct list_head *node = mem_cache_alloc(&cache);

        if (!node)
            break;
        BUG_ON((uintptr_t) node < HIGHER_BASE);
        ++count;
        list_add(node, &head);
    }

    printf("Allocated %lu bytes\n", count * sizeof(head));

    while (!list_empty(&head)) {
        struct list_head *node = head.next;

        BUG_ON((uintptr_t) node < HIGHER_BASE);
        list_del(node);
        mem_cache_free(&cache, node);
    }

    mem_cache_release(&cache);
}

static void test_buddy(void) {
    struct list_head head;
    unsigned long count = 0;

    list_init(&head);
    while (1) {

        struct page *page = __page_alloc(0);

        if (!page)
            break;
        ++count;
        list_add(&page->ll, &head);
    }

    printf("Allocated %lu pages\n", count);

    while (!list_empty(&head)) {
        struct list_head *node = head.next;
        struct page *page = CONTAINER_OF(node, struct page, ll);

        list_del(&page->ll);
        __page_free(page, 0);
    }
}

void deadlock_test() {
    static struct spinlock ticket_handler;
    lock(&ticket_handler);
    lock(&ticket_handler);
}

void test_threadfunc(void *arg) {

    printf("I AM CALLED MUAHAHA\n");
    char *textarg = (char *) arg;
    printf(textarg);


    int test = 1;
    printf("local variable test: %d\n", test);

    printf("returning to master thread\n");

    thread_run(&slave_thread, &master_thread);
}

void test_fs() {
    const char * pathname = "first_file";
    create(pathname);
    struct FILE * first_file = open(pathname);
    assert(strcmp(first_file->pathname, pathname) == 0);
    printf(first_file->pathname);
    printf("\n");
    const char * string_to_write = "abcd";
    writestring(first_file, string_to_write);

    printf("\n");
    assert(readchar(first_file, 0) == 'a');
    assert(readchar(first_file, 1) == 'b');
    assert(readchar(first_file, 2) == 'c');
    assert(readchar(first_file, 3) == 'd');
    printf("\n");
    assert(strcmp(string_to_write,
                  read_file_to_string(first_file)) == 0);


    //stress test
    printf("stress test start..\n");
    for(size_t i = 0 ; i < 10000000; ++i) { // ten megabytes, fails on 100 megabytes
        writestring(first_file, "a");
    }

//    writechar(first_file, 10, 'h');
//    writechar(first_file, 11, 'e');
//    writechar(first_file, 12, 'l');
//    writechar(first_file, 13, 'l');
//    writechar(first_file, 14, 'o');
//
//    const char * outp = read_file_to_string(first_file);
//    printf(outp);

    printf("stress test end.\n");

    close(first_file);

    // fails when file does not exist, ok
//    printf("\n\n\nsearching unexisted file ..\n\n\n");
//    struct FILE * second_file = open("does_not_exist");
//    assert(strcmp(second_file->pathname, "does_not_exist") == 0);
//    writestring(second_file, "will crash everything");
//    printf("%s\n", read_file_to_string(second_file));
//    printf("%s\n", read_file_to_string(first_file));


    mkdir("/usr");


}

void assert(int condition) {
    if (!condition) throw_ex("assertion failed");
}




void main(void *bootstrap_info) {

    qemu_gdb_hang();

    serial_setup();
    ints_setup();
    time_setup();

    balloc_setup(bootstrap_info);
    paging_setup();
    page_alloc_setup();
    mem_alloc_setup();
    kmap_setup();

    enable_ints();

    threads_init();


    printf("Tests Begin\n");

    test_fs();


    const char * wtring_netween_threads = "you shall not pass!\n";
    struct thread * thread_to_run = thread_create(test_threadfunc, (void *)"this was passed as argument from previous thread\n");
    slave_thread = *thread_to_run;
    thread_run(&master_thread, thread_to_run);

    printf(wtring_netween_threads);
    printf("i passed!\n");


    test_buddy();
    test_slab();

    test_alloc();

    test_kmap();
    printf("Tests Finished\n");

    while (1);
}
