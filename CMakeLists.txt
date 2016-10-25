cmake_minimum_required(VERSION 3.6)
project(src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    inc/desc.h
    inc/ints.h
    inc/ioport.h
    inc/memory.h
    src/bootstrap.S
    src/main.c
    src/videomem.S inc/multiboot.h)

add_executable(kernel ${SOURCE_FILES})