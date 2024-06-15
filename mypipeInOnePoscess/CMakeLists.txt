cmake_minimum_required(VERSION 3.28)
project(mypipeInOnePoscess C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(mypipeInOnePoscess
        mypipe.c
        mypipe.h)
