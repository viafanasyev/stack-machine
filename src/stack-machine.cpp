/**
 * @file
 * @brief Implementation of stack machine functions.
 */
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "stack-machine.h"

using byte = unsigned char;

union doubleAsBytes {
    double doubleValue;
    byte bytes[sizeof(double)];
};

double RAM::getAt(int pos) const {
    usleep(DELAY_MICROSECONDS);
    doubleAsBytes doubleBytes { 0 };
    for (byte& b : doubleBytes.bytes) {
        b = memory[pos];
        pos += sizeof(byte);
    }
    return doubleBytes.doubleValue;
}

void RAM::setAt(int pos, double value) {
    usleep(DELAY_MICROSECONDS);
    doubleAsBytes doubleBytes { value };
    for (byte b : doubleBytes.bytes) {
        memory[pos] = b;
        pos += sizeof(byte);
    }
}

StackMachine::StackMachine(const char* assemblyFileName) : AssemblyMachine(assemblyFileName) {
    constructStack(&stack);
    constructStack(&callStack);
    ram = RAM();
}

StackMachine::~StackMachine() {
    destructStack(&stack);
    destructStack(&callStack);
}

/**
 * Processes the no-operand operation.
 * @param[in] opcode code of the operation to process
 * @return given operation code, if operation processed successfully;
 *         ERR_INVALID_OPERATION, if operation code was invalid;
 *         ERR_STACK_UNDERFLOW, if pop operation was processed on empty stack.
 */
byte StackMachine::processOperation(byte opcode) {
    assert(assemblySize >= 0);
    assert((pc >= 0) && (pc <= assemblySize));
    assert(assembly != nullptr);
    assert(registers != nullptr);

    if (opcode == IN_OPCODE) {
        double inputValue = NAN;
        printf("> ");
        scanf("%lg", &inputValue);
        push(&stack, inputValue);
    } else if (opcode == OUT_OPCODE) {
        if (getStackSize(&stack) < 1) return ERR_STACK_UNDERFLOW;

        printf("%lg\n", pop(&stack));
    } else if (opcode == POP_OPCODE) {
        if (getStackSize(&stack) < 1) return ERR_STACK_UNDERFLOW;

        pop(&stack);
    } else if (opcode == ADD_OPCODE) {
        if (getStackSize(&stack) < 2) return ERR_STACK_UNDERFLOW;

        double rhs = pop(&stack);
        double lhs = pop(&stack);
        push(&stack, lhs + rhs);
    } else if (opcode == SUB_OPCODE) {
        if (getStackSize(&stack) < 2) return ERR_STACK_UNDERFLOW;

        double rhs = pop(&stack);
        double lhs = pop(&stack);
        push(&stack, lhs - rhs);
    } else if (opcode == MUL_OPCODE) {
        if (getStackSize(&stack) < 2) return ERR_STACK_UNDERFLOW;

        double rhs = pop(&stack);
        double lhs = pop(&stack);
        push(&stack, lhs * rhs);
    } else if (opcode == DIV_OPCODE) {
        if (getStackSize(&stack) < 2) return ERR_STACK_UNDERFLOW;

        double rhs = pop(&stack);
        double lhs = pop(&stack);
        push(&stack, lhs / rhs);
    } else if (opcode == SQRT_OPCODE) {
        if (getStackSize(&stack) < 1) return ERR_STACK_UNDERFLOW;

        double top = pop(&stack);
        push(&stack, sqrt(top));
    } else if (opcode == DUP_OPCODE) {
        if (getStackSize(&stack) < 1) return ERR_STACK_UNDERFLOW;

        push(&stack, top(&stack));
    } else if (opcode == POW_OPCODE) {
        if (getStackSize(&stack) < 2) return ERR_STACK_UNDERFLOW;

        double rhs = pop(&stack);
        double lhs = pop(&stack);
        push(&stack, pow(lhs, rhs));
    } else if (opcode == RET_OPCODE) {
        if (getStackSize(&callStack) < 1) return ERR_STACK_UNDERFLOW;

        int returnAddress = pop(&callStack);
        pc = returnAddress;
    } else if (opcode == HLT_OPCODE) {
        /* Do nothing */
    } else {
        return ERR_INVALID_OPERATION;
    }
    return opcode;
}

/**
 * Processes the single operand operation.
 * @param[in]      opcode  code of the operation to process
 * @param[in, out] operand operand to process
 * @return given operation code, if operation processed successfully;
 *         ERR_INVALID_OPERATION, if operation code was invalid;
 *         ERR_STACK_UNDERFLOW, if pop operation was processed on empty stack;
 *         ERR_INVALID_RAM_ADDRESS, if address operand exceeds RAM size.
 */
byte StackMachine::processOperation(byte opcode, double& operand) {
    assert(assemblySize >= 0);
    assert((pc >= 0) && (pc <= assemblySize));
    assert(assembly != nullptr);
    assert(registers != nullptr);

    switch (opcode) {
        case PUSH_OPCODE:
        case PUSHR_OPCODE:
            push(&stack, operand);
            break;
        case PUSHM_OPCODE:
        case PUSHRM_OPCODE:
            if ((operand < 0) || ((int)operand >= ram.SIZE)) return ERR_INVALID_RAM_ADDRESS;
            push(&stack, ram.getAt((int)operand));
            break;
        case POPR_OPCODE:
            operand = pop(&stack);
            break;
        case POPM_OPCODE:
        case POPRM_OPCODE:
            if ((operand < 0) || ((int)operand >= ram.SIZE)) return ERR_INVALID_RAM_ADDRESS;
            ram.setAt((int)operand, pop(&stack));
            break;
        default:
            return ERR_INVALID_OPERATION;
    }
    return opcode;
}

/**
 * Processes the jump operation.
 * @param[in] opcode     code of the jump operation to process
 * @param[in] jumpOffset offset of the jump to process
 * @return given operation code, if operation processed successfully;
 *         ERR_INVALID_OPERATION, if operation code or offset was invalid;
 *         ERR_STACK_UNDERFLOW, if pop operation was processed on empty stack.
 */
byte StackMachine::processJumpOperation(byte opcode, int jumpOffset) {
    assert(assemblySize >= 0);
    assert((pc >= 0) && (pc <= assemblySize));
    assert(assembly != nullptr);
    assert(registers != nullptr);
    assert(isJumpOperation(opcode));

    double lhs = NAN, rhs = NAN;
    if (opcode != JMP_OPCODE && opcode != CALL_OPCODE) {
        if (getStackSize(&stack) < 2) return ERR_STACK_UNDERFLOW;
        rhs = pop(&stack); lhs = pop(&stack);
    }
    switch (opcode) {
        case JMPE_OPCODE:
            if (fabs(lhs - rhs) < COMPARE_EPS) break;
            return opcode;
        case JMPNE_OPCODE:
            if (fabs(lhs - rhs) >= COMPARE_EPS) break;
            return opcode;
        case JMPL_OPCODE:
            if (lhs < rhs) break;
            return opcode;
        case JMPLE_OPCODE:
            if (lhs <= rhs) break;
            return opcode;
        case JMPG_OPCODE:
            if (lhs > rhs) break;
            return opcode;
        case JMPGE_OPCODE:
            if (lhs >= rhs) break;
            return opcode;
        case CALL_OPCODE:
            push(&callStack, pc);
            break;
        case JMP_OPCODE:
            break;
        default:
            return ERR_INVALID_OPERATION;
    }
    pc += jumpOffset;
    if (pc < 0 || pc >= assemblySize) return ERR_INVALID_OPERATION;
    return opcode;
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
            if (asRamAccess(operandToken)) opcode |= IS_RAM_OP_MASK;
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
           opcode == ERR_INVALID_LABEL     ||
           opcode == ERR_INVALID_FILE      ||
           opcode == ERR_INVALID_RAM_ADDRESS;
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
 *         ERR_INVALID_FILE, if input file is invalid.
 */
int assemble(const char* inputFileName, const char* outputFileName) {
    assert(inputFileName != nullptr);
    assert(outputFileName != nullptr);

    FILE* input  = fopen(inputFileName,  "r");
    if (input == nullptr) return ERR_INVALID_FILE;
    FILE* output = fopen(outputFileName, "wb");
    if (output == nullptr) return ERR_INVALID_FILE;

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
 *         ERR_INVALID_FILE, if input file is invalid.
 */
int disassemble(const char* inputFileName, const char* outputFileName) {
    assert(inputFileName != nullptr);
    assert(outputFileName != nullptr);

    FILE* input  = fopen(inputFileName,  "rb");
    if (input == nullptr) return ERR_INVALID_FILE;
    FILE* output = fopen(outputFileName, "w");
    if (output == nullptr) return ERR_INVALID_FILE;

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
            disasmBuffer.writeRegister(regName, (opcode & IS_RAM_OP_MASK) != 0);
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
                disasmBuffer.writeOperand(operand, (opcode & IS_RAM_OP_MASK) != 0);
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
 *         ERR_INVALID_FILE, if input file is invalid;
 *         ERR_INVALID_RAM_ADDRESS, if address operand exceeds RAM size.
 */
int run(const char* inputFileName) {
    assert(inputFileName != nullptr);

    StackMachine stackMachine(inputFileName);
    if (stackMachine.getAssemblySize() < 0) return ERR_INVALID_FILE;

    byte opcode = 0;
    do {
        opcode = stackMachine.processNextOperation();
    } while (opcode != HLT_OPCODE && !isError(opcode));

    return opcode;
}
