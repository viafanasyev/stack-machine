/**
 * @file
 * @brief Declaration of stack machine functions.
 */
#ifndef STACK_MACHINE_STACK_MACHINE_H
#define STACK_MACHINE_STACK_MACHINE_H

/**
 * Assembles the given source code file into the assembly file.
 * @param[in] inputFileName  source code file name
 * @param[in] outputFileName resulting assembly file name
 * @return 0, if assembly ended successfully, or ERR_INVALID_OPERATION, if invalid operation was met.
 */
int assemble(const char* inputFileName, const char* outputFileName);

/**
 * Disassembles the given assembly file into the possible source code file.
 * @param[in] inputFileName  assembly file name
 * @param[in] outputFileName resulting source code file name
 * @return 0, if assembly ended successfully, or ERR_INVALID_OPERATION, if invalid operation was met.
 */
int disassemble(const char* inputFileName, const char* outputFileName);

/**
 * Runs the given assembly file.
 * @param[in] inputFileName  assembly file name
 * @return 0, if program ended successfully, or ERR_INVALID_OPERATION, if invalid operation was met.
 */
int run(const char* inputFileName);

#endif // STACK_MACHINE_STACK_MACHINE_H
