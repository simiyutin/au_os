cmake_minimum_required(VERSION 3.6)
project(src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/*.c, inc/*.h inc/concurrency_util.h)

add_executable(src ${SOURCE_FILES})