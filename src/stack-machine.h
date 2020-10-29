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
 * @return 0, if assembly finished successfully;
 *         ERR_INVALID_OPERATION, if invalid operation was met;
 *         ERR_INVALID_REGISTER, if invalid register was met.
 */
int assemble(const char* inputFileName, const char* outputFileName);

/**
 * Disassembles the given assembly file into the possible source code file.
 * @param[in] inputFileName  assembly file name
 * @param[in] outputFileName resulting source code file name
 * @return 0, if disassembly finished successfully;
 *         ERR_INVALID_OPERATION, if invalid operation was met;
 *         ERR_INVALID_REGISTER, if invalid register was met.
 */
int disassemble(const char* inputFileName, const char* outputFileName);

/**
 * Runs the given assembly file.
 * @param[in] inputFileName  assembly file name
 * @return 0, if program finished successfully;
 *         ERR_INVALID_OPERATION, if invalid operation was met;
 *         ERR_INVALID_REGISTER, if invalid register was met;
 *         ERR_STACK_UNDERFLOW, if pop operation was processed on empty stack.
 */
int run(const char* inputFileName);

#endif // STACK_MACHINE_STACK_MACHINE_H
