/**
 * @file
 * @brief Implementation of stack machine functions.
 */
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#define STACK_SECURITY_LEVEL 3
#define STACK_TYPE double
#include "immortal-stack/stack.h"
#undef STACK_TYPE
#include "stack-machine.h"
#include "stack-machine-utils.h"

using byte = unsigned char;

struct StackMachine {
    Stack_double stack;
    double* registers = nullptr;
};

/**
 * Processes the no-operand operation with the given stack.
 * @param[in, out] stackMachine stack machine to use in operation
 * @param[in]      opcode code of the operation to process
 * @return given operation code, if operation processed successfully;
 *         ERR_INVALID_OPERATION, if operation code was invalid;
 *         ERR_STACK_UNDERFLOW, if pop operation was processed on empty stack.
 */
static byte processOperation(StackMachine* stackMachine, byte opcode) {
    assert(stackMachine != nullptr);

    Stack_double* stack = &stackMachine->stack;

    if (opcode == IN_OPCODE) {
        double inputValue = FP_NAN;
        printf("> ");
        scanf("%lg", &inputValue);
        push(stack, inputValue);
    } else if (opcode == OUT_OPCODE) {
        if (getStackSize(stack) < 1) return ERR_STACK_UNDERFLOW;

        printf("%lg\n", pop(stack));
    } else if (opcode == POP_OPCODE) {
        if (getStackSize(stack) < 1) return ERR_STACK_UNDERFLOW;

        pop(stack);
    } else if (opcode == ADD_OPCODE) {
        if (getStackSize(stack) < 2) return ERR_STACK_UNDERFLOW;

        double rhs = pop(stack);
        double lhs = pop(stack);
        push(stack, lhs + rhs);
    } else if (opcode == SUB_OPCODE) {
        if (getStackSize(stack) < 2) return ERR_STACK_UNDERFLOW;

        double rhs = pop(stack);
        double lhs = pop(stack);
        push(stack, lhs - rhs);
    } else if (opcode == MUL_OPCODE) {
        if (getStackSize(stack) < 2) return ERR_STACK_UNDERFLOW;

        double rhs = pop(stack);
        double lhs = pop(stack);
        push(stack, lhs * rhs);
    } else if (opcode == DIV_OPCODE) {
        if (getStackSize(stack) < 2) return ERR_STACK_UNDERFLOW;

        double rhs = pop(stack);
        double lhs = pop(stack);
        push(stack, lhs / rhs);
    } else if (opcode == SQRT_OPCODE) {
        if (getStackSize(stack) < 1) return ERR_STACK_UNDERFLOW;

        double top = pop(stack);
        push(stack, sqrt(top));
    } else if (opcode == HLT_OPCODE) {
        /* Do nothing */
    } else {
        return ERR_INVALID_OPERATION;
    }
    return opcode;
}

/**
 * Processes the single operand operation with the given stack.
 * @param[in, out] stackMachine stack machine to use in operation
 * @param[in]      opcode  code of the operation to process
 * @param[in, out] operand operand to process
 * @return given operation code, if operation processed successfully;
 *         ERR_INVALID_OPERATION, if operation code was invalid;
 *         ERR_STACK_UNDERFLOW, if pop operation was processed on empty stack.
 */
static byte processOperation(StackMachine* stackMachine, byte opcode, double& operand) {
    assert(stackMachine != nullptr);

    switch (opcode) {
        case PUSH_OPCODE:
        case PUSHR_OPCODE:
            push(&stackMachine->stack, operand);
            break;
        case POPR_OPCODE:
            operand = pop(&stackMachine->stack);
            break;
        default:
            return ERR_INVALID_OPERATION;
    }
    return opcode;
}

/**
 * Processes the next operation from assembly file with the given stack.
 * @param[in, out] stackMachine stack machine to use in operation
 * @param[in]      input        assembly file
 * @return processed operation code;
 *         ERR_INVALID_OPERATION if operation was invalid;
 *         ERR_INVALID_REGISTER if register was invalid;
 *         ERR_STACK_UNDERFLOW, if pop operation was processed on empty stack.
 */
static byte processNextOperation(StackMachine* stackMachine, FILE* input) {
    assert(stackMachine != nullptr);

    byte opcode = asmReadOperation(input);

    if (opcode == ERR_INVALID_OPERATION) return ERR_INVALID_OPERATION;

    if (getOperationArityByOpcode(opcode) == 1) {
        if ((opcode & IS_REG_OP_MASK) != 0) {
            byte reg = asmReadRegister(input);
            if (reg == ERR_INVALID_REGISTER) return ERR_INVALID_REGISTER;
            double& operand = stackMachine->registers[reg];
            return processOperation(stackMachine, opcode, operand);
        } else {
            double operand = asmReadOperand(input);
            if (!std::isfinite(operand)) return ERR_INVALID_OPERATION;
            return processOperation(stackMachine, opcode, operand);
        }
    } else {
        return processOperation(stackMachine, opcode);
    }
}

/**
 * Assembles the given source code file into the assembly file.
 * @param[in] inputFileName  source code file name
 * @param[in] outputFileName resulting assembly file name
 * @return 0, if assembly finished successfully;
 *         ERR_INVALID_OPERATION, if invalid operation was met;
 *         ERR_INVALID_REGISTER, if invalid register was met;
 *         -1, if any other error occurred (such as invalid input file).
 */
int assemble(const char* inputFileName, const char* outputFileName) {
    assert(inputFileName != nullptr);
    assert(outputFileName != nullptr);

    FILE* input  = fopen(inputFileName,  "r");
    if (input == nullptr) return -1;
    FILE* output = fopen(outputFileName, "wb");

    constexpr unsigned int lineMaxSize = 256u;
    char* const lineOriginPtr = (char*)calloc(lineMaxSize, sizeof(char));
    char* line = lineOriginPtr;
    while (fgets(line, lineMaxSize, input)) {
        if (strlen(trim(line)) == 0) {
            line = lineOriginPtr;
            continue;
        }

        byte opcode = parseOperation(line);
        if (opcode == ERR_INVALID_OPERATION) return ERR_INVALID_OPERATION;

        // TODO: Clean up somehow
        char* operandToken = getNextToken(line);
        if (getRegisterNumberByName(operandToken) != ERR_INVALID_REGISTER) {
            opcode |= IS_REG_OP_MASK;
            asmWrite(output, opcode);

            byte reg = parseRegister(operandToken);
            if (reg == ERR_INVALID_REGISTER) return ERR_INVALID_REGISTER;
            asmWrite(output, reg);
        } else if (getOperationArityByOpcode(opcode) == 1) {
            asmWrite(output, opcode);

            double operand = parseOperand(operandToken);
            if (!std::isfinite(operand)) return ERR_INVALID_OPERATION;
            asmWrite(output, operand);
        } else {
            asmWrite(output, opcode);
        }

        line = lineOriginPtr;
    }
    free(lineOriginPtr);

    fclose(output);
    fclose(input);
    return 0;
}

/**
 * Disassembles the given assembly file into the possible source code file.
 * @param[in] inputFileName  assembly file name
 * @param[in] outputFileName resulting source code file name
 * @return 0, if disassembly finished successfully;
 *         ERR_INVALID_OPERATION, if invalid operation was met;
 *         ERR_INVALID_REGISTER, if invalid register was met;
 *         -1, if any other error occurred (such as invalid input file).
 */
int disassemble(const char* inputFileName, const char* outputFileName) {
    assert(inputFileName != nullptr);
    assert(outputFileName != nullptr);

    FILE* input  = fopen(inputFileName,  "rb");
    if (input == nullptr) return -1;
    FILE* output = fopen(outputFileName, "w");

    byte opcode = asmReadOperation(input);
    while (!feof(input)) {
        if (opcode == ERR_INVALID_OPERATION) return ERR_INVALID_OPERATION;
        const char* operation = getOperationNameByOpcode(opcode);
        if (operation == nullptr) return ERR_INVALID_OPERATION;
        disasmWriteOperation(output, operation);

        if ((opcode & IS_REG_OP_MASK) != 0) {
            byte reg = asmReadRegister(input);
            if (reg == ERR_INVALID_REGISTER) return ERR_INVALID_REGISTER;
            const char* regName = getRegisterNameByNumber(reg);
            if (regName == nullptr) return ERR_INVALID_REGISTER;
            disasmWriteRegister(output, regName);
        } else if (getOperationArityByOpcode(opcode) == 1) {
            double operand = asmReadOperand(input);
            if (!std::isfinite(operand)) return ERR_INVALID_OPERATION;
            disasmWriteOperand(output, operand);
        }

        disasmWrite(output, "\n");

        opcode = asmReadOperation(input);
    }

    fclose(output);
    fclose(input);
    return 0;
}

/**
 * Checks if the given operation code is actually an error code.
 * @param[in] opcode operation code to check
 * @return true, if the operation code is error code, false otherwise.
 */
static bool isError(byte opcode) {
    return opcode == ERR_INVALID_OPERATION || opcode == ERR_INVALID_REGISTER || opcode == ERR_STACK_UNDERFLOW;
}

/**
 * Runs the given assembly file.
 * @param[in] inputFileName  assembly file name
 * @return 0, if program finished successfully;
 *         ERR_INVALID_OPERATION, if invalid operation was met;
 *         ERR_INVALID_REGISTER, if invalid register was met;
 *         ERR_STACK_UNDERFLOW, if pop operation was processed on empty stack;
 *         -1, if any other error occurred (such as invalid input file).
 */
int run(const char* inputFileName) {
    assert(inputFileName != nullptr);

    FILE* input = fopen(inputFileName, "rb");
    if (input == nullptr) return -1;

    StackMachine stackMachine;
    constructStack(&stackMachine.stack);
    stackMachine.registers = (double*)calloc(REGISTERS_NUMBER, sizeof(double));

    byte opcode = ERR_INVALID_OPERATION;
    while (opcode != HLT_OPCODE) {
        if (isError(opcode = processNextOperation(&stackMachine, input))) return opcode;
    }

    free(stackMachine.registers);
    destructStack(&stackMachine.stack);
    fclose(input);
    return 0;
}
