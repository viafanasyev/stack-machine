cmake_minimum_required(VERSION 3.16)
project(stack-machine)

set(CMAKE_CXX_STANDARD 14)

add_compile_options(-Wall -Wextra -pedantic -Werror -Wfloat-equal -fno-stack-protector)

add_executable(
        stack-machine
        src/main.cpp
        src/immortal-stack/stack.h
        src/immortal-stack/logger.h
        src/immortal-stack/environment.h)

add_executable(
        tests
        test/main.cpp
        test/testlib.h
        test/testlib.cpp
        src/immortal-stack/stack.h)