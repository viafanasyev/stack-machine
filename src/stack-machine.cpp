/**
 * @file
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

static byte processOperation(Stack_double* stack, byte opcode) {
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

static byte processOperation(Stack_double* stack, byte opcode, double operand) {
    switch (opcode) {
        case PUSH_OPCODE:
            push(stack, operand);
            break;
        default:
            return ERR_INVALID_OPERATION;
    }
    return opcode;
}

static byte processNextOperation(Stack_double* stack, FILE* input) {
    byte opcode = asmReadOperation(input);

    if (opcode == ERR_INVALID_OPERATION) return ERR_INVALID_OPERATION;

    if (getOperationArityByOpcode(opcode) == 1) {
        double operand = asmReadOperand(input);
        if (!std::isfinite(operand)) return ERR_INVALID_OPERATION;
        return processOperation(stack, opcode, operand);
    } else {
        return processOperation(stack, opcode);
    }
}

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
        asmWrite(output, opcode);

        if (getOperationArityByOpcode(opcode) == 1) {
            double operand = parseOperand(line);
            if (!std::isfinite(operand)) return ERR_INVALID_OPERATION;
            asmWrite(output, operand);
        }

        line = lineOriginPtr;
    }
    free(lineOriginPtr);

    fclose(output);
    fclose(input);
    return 0;
}

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

        if (getOperationArityByOpcode(opcode) == 1) {
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

int run(const char* inputFileName) {
    assert(inputFileName != nullptr);

    FILE* input = fopen(inputFileName, "rb");
    Stack_double stack;
    constructStack(&stack);

    byte opcode = ERR_INVALID_OPERATION;
    while (opcode != HLT_OPCODE) {
        if ((opcode = processNextOperation(&stack, input)) == ERR_INVALID_OPERATION) return ERR_INVALID_OPERATION;
    }

    destructStack(&stack);
    fclose(input);
    return 0;
}
