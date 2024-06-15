cmake_minimum_required(VERSION 3.28)
project(systemProgram C)

set(CMAKE_C_STANDARD 11)

add_executable(systemProgram
        relayer.c

)

add_executable(systemProgram1
        tempfile_1718436850439.c
)

add_executable(mymmap mmap.c)