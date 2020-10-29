/**
 * @file
 * @brief Implementation of stack machine functions.
 */
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#define STACK_SECURITY_LEVEL 3
#define STACK_TYPE double
#include "immortal-stack/stack.h"
#undef STACK_TYPE
#include "stack-machine.h"
#include "stack-machine-utils.h"

using byte = unsigned char;

/**
 * Processes the no-operand operation with the given stack.
 * @param[in, out] stack  stack to use in operation
 * @param[in]      opcode code of the operation to process
 * @return given operation code, or ERR_INVALID_OPERATION if operation code was invalid.
 */
static byte processOperation(Stack_double* stack, byte opcode) {
    assert(stack != nullptr);

    if (opcode == IN_OPCODE) {
        double inputValue = FP_NAN;
        printf("> ");
        scanf("%lg", &inputValue);
        push(stack, inputValue);
    } else if (opcode == OUT_OPCODE) {
        printf("%lg\n", pop(stack));
    } else if (opcode == POP_OPCODE) {
        pop(stack);
    } else if (opcode == ADD_OPCODE) {
        double rhs = pop(stack);
        double lhs = pop(stack);
        push(stack, lhs + rhs);
    } else if (opcode == SUB_OPCODE) {
        double rhs = pop(stack);
        double lhs = pop(stack);
        push(stack, lhs - rhs);
    } else if (opcode == MUL_OPCODE) {
        double rhs = pop(stack);
        double lhs = pop(stack);
        push(stack, lhs * rhs);
    } else if (opcode == DIV_OPCODE) {
        double rhs = pop(stack);
        double lhs = pop(stack);
        push(stack, lhs / rhs);
    } else if (opcode == SQRT_OPCODE) {
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
 * @param[in, out] stack   stack to use in operation
 * @param[in]      opcode  code of the operation to process
 * @param[in, out] operand operand to process
 * @return given operation code, or ERR_INVALID_OPERATION if operation code was invalid.
 */
static byte processOperation(Stack_double* stack, byte opcode, double& operand) {
    assert(stack != nullptr);

    switch (opcode) {
        case PUSH_OPCODE:
        case PUSHR_OPCODE:
            push(stack, operand);
            break;
        case POPR_OPCODE:
            operand = pop(stack);
            break;
        default:
            return ERR_INVALID_OPERATION;
    }
    return opcode;
}

/**
 * Processes the next operation from assembly file with the given stack.
 * @param[in, out] stack     stack to use in operation
 * @param[in, out] registers stack machine registers
 * @param[in]      input     assembly file
 * @return processed operation code;
 *         ERR_INVALID_OPERATION if operation was invalid;
 *         ERR_INVALID_REGISTER if register was invalid.
 */
static byte processNextOperation(Stack_double* stack, double* registers, FILE* input) {
    assert(stack != nullptr);
    assert(registers != nullptr);

    byte opcode = asmReadOperation(input);

    if (opcode == ERR_INVALID_OPERATION) return ERR_INVALID_OPERATION;

    if (getOperationArityByOpcode(opcode) == 1) {
        if ((opcode & IS_REG_OP_MASK) != 0) {
            byte reg = asmReadRegister(input);
            if (reg == ERR_INVALID_REGISTER) return ERR_INVALID_REGISTER;
            double& operand = registers[reg];
            return processOperation(stack, opcode, operand);
        } else {
            double operand = asmReadOperand(input);
            if (!std::isfinite(operand)) return ERR_INVALID_OPERATION;
            return processOperation(stack, opcode, operand);
        }
    } else {
        return processOperation(stack, opcode);
    }
}

/**
 * Assembles the given source code file into the assembly file.
 * @param[in] inputFileName  source code file name
 * @param[in] outputFileName resulting assembly file name
 * @return 0, if assembly ended successfully;
 *         ERR_INVALID_OPERATION, if invalid operation was met;
 *         ERR_INVALID_REGISTER, if invalid register was met.
 */
int assemble(const char* inputFileName, const char* outputFileName) {
    assert(inputFileName != nullptr);
    assert(outputFileName != nullptr);

    FILE* input  = fopen(inputFileName,  "r");
    FILE* output = fopen(outputFileName, "wb");

    constexpr int lineMaxSize = 256;
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
 * @return 0, if assembly ended successfully;
 *         ERR_INVALID_OPERATION, if invalid operation was met;
 *         ERR_INVALID_REGISTER, if invalid register was met.
 */
int disassemble(const char* inputFileName, const char* outputFileName) {
    assert(inputFileName != nullptr);
    assert(outputFileName != nullptr);

    FILE* input  = fopen(inputFileName,  "rb");
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
 * Runs the given assembly file.
 * @param[in] inputFileName  assembly file name
 * @return 0, if program ended successfully;
 *         ERR_INVALID_OPERATION, if invalid operation was met;
 *         ERR_INVALID_REGISTER, if invalid register was met.
 */
int run(const char* inputFileName) {
    assert(inputFileName != nullptr);

    FILE* input = fopen(inputFileName, "rb");
    Stack_double stack;
    constructStack(&stack);
    auto registers = (double*)calloc(REGISTERS_NUMBER, sizeof(double));

    byte opcode = ERR_INVALID_OPERATION;
    while (opcode != HLT_OPCODE) {
        if ((opcode = processNextOperation(&stack, registers, input)) == ERR_INVALID_OPERATION) return ERR_INVALID_OPERATION;
    }

    free(registers);
    destructStack(&stack);
    fclose(input);
    return 0;
}
