/**
 * @file
 * @brief Implementation of stack machine functions.
 */
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/stat.h>

#define STACK_SECURITY_LEVEL 3
#define STACK_TYPE double
#include "immortal-stack/stack.h"
#undef STACK_TYPE

#include "stack-machine.h"
#include "stack-machine-utils.h"

using byte = unsigned char;

struct StackMachine : AssemblyMachine {
    Stack_double stack;
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
 * Processes the single operand operation with the given stack.
 * @param[in, out] stackMachine stack machine to use in operation
 * @param[in]      opcode  code of the operation to process
 * @param[in, out] operand operand to process
 * @return given operation code, if operation processed successfully;
 *         ERR_INVALID_OPERATION, if operation code was invalid;
 *         ERR_STACK_UNDERFLOW, if pop operation was processed on empty stack.
 */
static byte processOperation(StackMachine* stackMachine, byte opcode, int& operand) {
    assert(stackMachine != nullptr);

    switch (opcode) {
        case JMP_OPCODE:
            stackMachine->pc += operand;
            if (stackMachine->pc < 0 || stackMachine->pc >= stackMachine->assemblySize) return ERR_INVALID_OPERATION;
            break;
        default:
            return ERR_INVALID_OPERATION;
    }
    return opcode;
}

/**
 * Processes the next operation in stack machine.
 * @param[in, out] stackMachine stack machine to use in operation
 * @return processed operation code;
 *         ERR_INVALID_OPERATION if operation was invalid;
 *         ERR_INVALID_REGISTER if register was invalid;
 *         ERR_STACK_UNDERFLOW, if pop operation was processed on empty stack.
 */
static byte processNextOperation(StackMachine* stackMachine) {
    assert(stackMachine != nullptr);

    byte opcode = asmReadOperation(stackMachine);

    if (opcode == ERR_INVALID_OPERATION) return ERR_INVALID_OPERATION;

    if (getOperationArityByOpcode(opcode) == 1) {
        if ((opcode & IS_REG_OP_MASK) != 0) {
            byte reg = asmReadRegister(stackMachine);
            if (reg == ERR_INVALID_REGISTER) return ERR_INVALID_REGISTER;
            double& operand = stackMachine->registers[reg];
            return processOperation(stackMachine, opcode, operand);
        } else {
            if (isJumpOperation(opcode)) {
                int jumpOffset = asmReadJumpOffset(stackMachine);
                // sizeof(offset) is subtracted, because pc is calculated ahead (with offset size)
                jumpOffset -= (int)sizeof(jumpOffset);
                return processOperation(stackMachine, opcode, jumpOffset);
            } else {
                double operand = asmReadOperand(stackMachine);
                if (!std::isfinite(operand)) return ERR_INVALID_OPERATION;
                return processOperation(stackMachine, opcode, operand);
            }
        }
    } else {
        return processOperation(stackMachine, opcode);
    }
}

/**
 * If output file is nullptr, then this function resolves labels and puts info into label table.
 * Otherwise, assembles the given source code file into the assembly file.
 * @param[in]      input      source code file
 * @param[in]      output     resulting assembly file
 * @param[in, out] labelTable label table with info about labels
 * @return 0, if assembly/labels resolving finished successfully;
 *         ERR_INVALID_OPERATION, if invalid operation was met;
 *         ERR_INVALID_REGISTER, if invalid register was met;
 *         ERR_INVALID_LABEL, if invalid label was met.
 */
static byte assemble(FILE* input, FILE* output, LabelTable& labelTable) {
    bool isResolvingLabels = (output == nullptr);

    byte statusCode = 0;
    int currentByteOffset = 0;

    constexpr static unsigned int MAX_LINE_LENGTH = 256u;
    char* const lineOriginPtr = (char*)calloc(MAX_LINE_LENGTH, sizeof(char));
    char* line = lineOriginPtr;
    while (fgets(line, MAX_LINE_LENGTH, input)) {
        if ((strlen(trim(line)) == 0)) {
            line = lineOriginPtr;
            continue;
        }

        if (isLabel(line)) {
            if (isResolvingLabels) {
                if (labelTable.addLabel(line, currentByteOffset) == ERR_INVALID_LABEL) { statusCode = ERR_INVALID_LABEL; break; }
            }
        } else {
            byte opcode = parseOperation(line);
            if (opcode == ERR_INVALID_OPERATION) { statusCode = ERR_INVALID_OPERATION; break; }

            // TODO: Clean up somehow
            char* operandToken = getNextToken(line);
            if (getRegisterNumberByName(operandToken) != ERR_INVALID_REGISTER) {
                opcode |= IS_REG_OP_MASK;
                if (getOperationArityByOpcode(opcode) == ERR_INVALID_OPERATION) { statusCode = ERR_INVALID_OPERATION; break; }
                asmWrite(output, opcode, currentByteOffset);

                byte reg = parseRegister(operandToken);
                if (reg == ERR_INVALID_REGISTER) { statusCode = ERR_INVALID_REGISTER; break; }
                asmWrite(output, reg, currentByteOffset);
            } else if (getOperationArityByOpcode(opcode) == 1) {
                asmWrite(output, opcode, currentByteOffset);
                if (isJumpOperation(opcode)) {
                    int labelOffset = 0;
                    if (!isResolvingLabels) {
                        labelOffset = labelTable.getLabelOffset(operandToken);
                        if (labelOffset < 0) { statusCode = ERR_INVALID_LABEL; break; }
                        labelOffset -= currentByteOffset;
                    }
                    asmWrite(output, labelOffset, currentByteOffset);
                } else {
                    double operand = parseOperand(operandToken);
                    if (!std::isfinite(operand)) { statusCode = ERR_INVALID_OPERATION; break; }
                    asmWrite(output, operand, currentByteOffset);
                }
            } else {
                asmWrite(output, opcode, currentByteOffset);
            }
        }

        line = lineOriginPtr;
    }
    if (isLabel(lineOriginPtr)) statusCode = ERR_INVALID_LABEL; // because dangling label at the end of the code is an error

    free(lineOriginPtr);

    return statusCode;
}

/**
 * Checks if the given operation code is actually an error code.
 * @param[in] opcode operation code to check
 * @return true, if the operation code is error code, false otherwise.
 */
static bool isError(byte opcode) {
    return opcode == ERR_INVALID_OPERATION ||
           opcode == ERR_INVALID_REGISTER  ||
           opcode == ERR_STACK_UNDERFLOW   ||
           opcode == ERR_INVALID_LABEL;
}

/**
 * Resolves labels' offsets in assembly file and puts them into the given label table.
 * @param[in]  input      source code file
 * @param[out] labelTable label table to put info about labels into
 * @return 0, if labels was successfully resolved;
 *         ERR_INVALID_OPERATION, if invalid operation was met;
 *         ERR_INVALID_REGISTER, if invalid register was met;
 *         ERR_INVALID_LABEL, if invalid label was met.
 */
static byte resolveLabels(FILE* input, LabelTable& labelTable) {
    return assemble(input, nullptr, labelTable);
}

/**
 * Assembles the given source code file into the assembly file.
 * Assembly is performed in two runs: first run resolves labels, second one generates resulting .asm file.
 * @param[in] inputFileName  source code file name
 * @param[in] outputFileName resulting assembly file name
 * @return 0, if assembly finished successfully;
 *         ERR_INVALID_OPERATION, if invalid operation was met;
 *         ERR_INVALID_REGISTER, if invalid register was met;
 *         ERR_INVALID_LABEL, if invalid label was met;
 *         -1, if any other error occurred (such as invalid input file).
 */
int assemble(const char* inputFileName, const char* outputFileName) {
    assert(inputFileName != nullptr);
    assert(outputFileName != nullptr);

    FILE* input  = fopen(inputFileName,  "r");
    if (input == nullptr) return -1;
    FILE* output = fopen(outputFileName, "wb");

    LabelTable labelTable;
    byte statusCode = resolveLabels(input, labelTable);
    if (!isError(statusCode)) {
        rewind(input);
        statusCode = assemble(input, output, labelTable);
    }

    fclose(output);
    fclose(input);
    return statusCode;
}

/**
 * Disassembles the given assembly file into the possible source code file.
 * @param[in] inputFileName  assembly file name
 * @param[in] outputFileName resulting source code file name
 * @return 0, if disassembly finished successfully;
 *         ERR_INVALID_OPERATION, if invalid operation was met;
 *         ERR_INVALID_REGISTER, if invalid register was met;
 *         ERR_INVALID_LABEL, if invalid offset was met;
 *         -1, if any other error occurred (such as invalid input file).
 */
int disassemble(const char* inputFileName, const char* outputFileName) {
    assert(inputFileName != nullptr);
    assert(outputFileName != nullptr);

    FILE* input  = fopen(inputFileName,  "rb");
    if (input == nullptr) return -1;
    FILE* output = fopen(outputFileName, "w");

    byte statusCode = 0;

    DisassemblyBuffer disasmBuffer;
    int currentByteOffset = 0;

    byte opcode = asmReadOperation(input, currentByteOffset);
    while (!feof(input)) {
        const char* operation = getOperationNameByOpcode(opcode);
        if (operation == nullptr) { statusCode = ERR_INVALID_OPERATION; break; }
        disasmBuffer.writeOperation(operation);

        if ((opcode & IS_REG_OP_MASK) != 0) {
            byte reg = asmReadRegister(input, currentByteOffset);
            const char* regName = getRegisterNameByNumber(reg);
            if (regName == nullptr) { statusCode = ERR_INVALID_REGISTER; break; }
            disasmBuffer.writeRegister(regName);
        } else if (getOperationArityByOpcode(opcode) == 1) {
            if (isJumpOperation(opcode)) {
                int jumpByteOffset = asmReadJumpOffset(input, currentByteOffset);
                // sizeof(offset) is subtracted, because currentByteOffset is calculated ahead (with offset size)
                jumpByteOffset += currentByteOffset - (int)sizeof(jumpByteOffset);
                if (jumpByteOffset < 0) { statusCode = ERR_INVALID_LABEL; break; }
                disasmBuffer.writeJumpLabelArgument(jumpByteOffset);
            } else {
                double operand = asmReadOperand(input, currentByteOffset);
                if (!std::isfinite(operand)) { statusCode = ERR_INVALID_OPERATION; break; }
                disasmBuffer.writeOperand(operand);
            }
        }

        opcode = asmReadOperation(input, currentByteOffset);
    }

    if (statusCode == 0) {
        statusCode = disasmBuffer.flushToFile(output);
    }

    fclose(output);
    fclose(input);
    return statusCode;
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

    struct stat fileStat{};
    fstat(fileno(input), &fileStat);

    StackMachine stackMachine;
    constructStack(&stackMachine.stack);
    stackMachine.registers = (double*)calloc(REGISTERS_NUMBER, sizeof(double));
    stackMachine.assemblySize = fileStat.st_size;
    stackMachine.assembly = (byte*)calloc(stackMachine.assemblySize, sizeof(byte));
    for (int i = 0; i < stackMachine.assemblySize; ++i) {
        stackMachine.assembly[i] = fgetc(input);
    }
    stackMachine.pc = 0;

    byte opcode = 0;
    do {
        if (isError(opcode = processNextOperation(&stackMachine))) break;
    } while (opcode != HLT_OPCODE);

    free(stackMachine.assembly);
    free(stackMachine.registers);
    destructStack(&stackMachine.stack);
    fclose(input);
    return opcode;
}
