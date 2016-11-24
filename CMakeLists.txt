cmake_minimum_required(VERSION 3.6)
project(src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    inc/alloc.h
    inc/backtrace.h
    inc/balloc.h
    inc/ctype.h
    inc/debug.h
    inc/desc.h
    inc/i8259a.h
    inc/ints.h
    inc/ioport.h
    inc/kernel.h
    inc/list.h
    inc/memory.h
    inc/paging.h
    inc/print.h
    inc/rbtree.h
    inc/serial.h
    inc/stdlib.h
    inc/string.h
    inc/time.h
    src/alloc.c
    src/backtrace.c
    src/balloc.c
    src/ctype.c
    src/i8259a.c
    src/ints.c
    src/list.c
    src/main.c
    src/memory.c
    src/paging.c
    src/print.c
    src/rbtree.c
    src/serial.c
    src/stdlib.c
    src/string.c
    src/time.c)

add_executable(src ${SOURCE_FILES})