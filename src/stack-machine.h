/**
 * @file
 * @brief Declaration of stack machine functions.
 */
#ifndef STACK_MACHINE_STACK_MACHINE_H
#define STACK_MACHINE_STACK_MACHINE_H

#include <unistd.h>

#define STACK_SECURITY_LEVEL 3
#define STACK_TYPE double
#include "immortal-stack/stack.h"
#undef STACK_TYPE
#define STACK_TYPE int
#include "immortal-stack/stack.h"
#undef STACK_TYPE

#include "stack-machine-utils.h"

class RAM {

public:
    static constexpr int SIZE = 1024;
    static constexpr __useconds_t DELAY_MICROSECONDS = 10000; // 0.01 seconds
protected:
    unsigned char memory[SIZE];

public:
    unsigned char getAt(int pos) const;
    void setAt(int pos, unsigned char value);
};

class StackMachine : public AssemblyMachine {

private:
    Stack_double stack;
    Stack_int callStack;
    RAM ram;

public:
    explicit StackMachine(const char* assemblyFileName);

    ~StackMachine();

    StackMachine(StackMachine& stackMachine) = delete;
    StackMachine &operator=(const StackMachine&) = delete;

    /**
    * Processes the no-operand operation.
    * @param[in] opcode code of the operation to process
    * @return given operation code, if operation processed successfully;
    *         ERR_INVALID_OPERATION, if operation code was invalid;
    *         ERR_STACK_UNDERFLOW, if pop operation was processed on empty stack.
    */
    unsigned char processOperation(unsigned char opcode) override;

    /**
     * Processes the single operand operation.
     * @param[in]      opcode  code of the operation to process
     * @param[in, out] operand operand to process
     * @return given operation code, if operation processed successfully;
     *         ERR_INVALID_OPERATION, if operation code was invalid;
     *         ERR_STACK_UNDERFLOW, if pop operation was processed on empty stack;
     *         ERR_INVALID_RAM_ADDRESS, if address operand exceeds RAM size.
     */
    unsigned char processOperation(unsigned char opcode, double &operand) override;

    /**
     * Processes the jump operation.
     * @param[in] opcode     code of the jump operation to process
     * @param[in] jumpOffset offset of the jump to process
     * @return given operation code, if operation processed successfully;
     *         ERR_INVALID_OPERATION, if operation code or offset was invalid;
     *         ERR_STACK_UNDERFLOW, if pop operation was processed on empty stack.
     */
    unsigned char processJumpOperation(unsigned char opcode, int jumpOffset) override;
};

/**
 * Assembles the given source code file into the assembly file.
 * @param[in] inputFileName  source code file name
 * @param[in] outputFileName resulting assembly file name
 * @return 0, if assembly finished successfully;
 *         ERR_INVALID_OPERATION, if invalid operation was met;
 *         ERR_INVALID_REGISTER, if invalid register was met;
 *         ERR_INVALID_FILE, if input file is invalid.
 */
int assemble(const char* inputFileName, const char* outputFileName);

/**
 * Disassembles the given assembly file into the possible source code file.
 * @param[in] inputFileName  assembly file name
 * @param[in] outputFileName resulting source code file name
 * @return 0, if disassembly finished successfully;
 *         ERR_INVALID_OPERATION, if invalid operation was met;
 *         ERR_INVALID_REGISTER, if invalid register was met;
 *         ERR_INVALID_FILE, if input file is invalid.
 */
int disassemble(const char* inputFileName, const char* outputFileName);

/**
 * Runs the given assembly file.
 * @param[in] inputFileName  assembly file name
 * @return 0, if program finished successfully;
 *         ERR_INVALID_OPERATION, if invalid operation was met;
 *         ERR_INVALID_REGISTER, if invalid register was met;
 *         ERR_STACK_UNDERFLOW, if pop operation was processed on empty stack;
 *         ERR_INVALID_FILE, if input file is invalid;
 *         ERR_INVALID_RAM_ADDRESS, if address operand exceeds RAM size.
 */
int run(const char* inputFileName);

#endif // STACK_MACHINE_STACK_MACHINE_H
