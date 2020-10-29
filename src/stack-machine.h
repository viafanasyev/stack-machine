/**
 * @file
 */
#ifndef STACK_MACHINE_STACK_MACHINE_H
#define STACK_MACHINE_STACK_MACHINE_H

int assemble(const char* inputFileName, const char* outputFileName);

int disassemble(const char* inputFileName, const char* outputFileName);

int run(const char* inputFileName);

#endif // STACK_MACHINE_STACK_MACHINE_H
