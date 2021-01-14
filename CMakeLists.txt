cmake_minimum_required(VERSION 3.16)
project(stack-machine)

set(CMAKE_CXX_STANDARD 14)

add_compile_options(-Wall -Wextra -pedantic -Werror -Wfloat-equal -fno-stack-protector)

add_executable(
        assemble
        src/main-asm.cpp
        src/immortal-stack/stack.h
        src/immortal-stack/logger.h
        src/immortal-stack/environment.h
        src/stack-machine.h
        src/stack-machine.cpp
        src/stack-machine-utils.h
        src/stack-machine-utils.cpp
        src/arg-parser.cpp
        src/arg-parser.h)

add_executable(
        disassemble
        src/main-disasm.cpp
        src/immortal-stack/stack.h
        src/immortal-stack/logger.h
        src/immortal-stack/environment.h
        src/stack-machine.h
        src/stack-machine.cpp
        src/stack-machine-utils.h
        src/stack-machine-utils.cpp
        src/arg-parser.h
        src/arg-parser.cpp)

add_executable(
        run
        src/main-run.cpp
        src/immortal-stack/stack.h
        src/immortal-stack/logger.h
        src/immortal-stack/environment.h
        src/stack-machine.h
        src/stack-machine.cpp
        src/stack-machine-utils.h
        src/stack-machine-utils.cpp
        src/arg-parser.h
        src/arg-parser.cpp)

add_executable(
        tests
        test/main.cpp
        test/testlib.h
        test/testlib.cpp
        src/immortal-stack/stack.h
        src/stack-machine.h
        src/stack-machine.cpp
        src/stack-machine-utils.h
        src/stack-machine-utils.cpp
        test/stack-machine-tests.cpp)
