/**
 * @file
 * @brief Definition of helper functions for input arguments parsing
 */
#ifndef STACK_MACHINE_ARG_PARSER_H
#define STACK_MACHINE_ARG_PARSER_H

#include <cstddef>

enum RunningMode {
    ASM    = 1,
    DISASM = 2,
    RUN    = 3,
};

constexpr size_t maxFileNameLength = 256;
const char* const assemblyFileExtension = ".asm";
const char* const disassemblyFileExtension = "__disassembly.txt";

struct arguments {
    char inputFile[maxFileNameLength];
    char outputFile[maxFileNameLength];
};

void stripExtension(char* fileName);

void replaceExtension(char* destination, const char* originalFileName, const char* newExtension);

arguments parseArgs(int argc, char* argv[], RunningMode runningMode);

#endif // STACK_MACHINE_ARG_PARSER_H
