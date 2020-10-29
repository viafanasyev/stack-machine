/**
 * @file
 */
#include <cassert>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include "stack-machine-utils.h"

using byte = unsigned char;

union doubleAsBytes {
    double doubleValue;
    byte bytes[sizeof(double)];
};

byte getOpcodeByOperationName(const char* operation) {
    assert(operation != nullptr);

    if (strcmp(operation, "IN"  ) == 0) return IN_OPCODE  ;
    if (strcmp(operation, "OUT" ) == 0) return OUT_OPCODE ;
    if (strcmp(operation, "POP" ) == 0) return POP_OPCODE ;
    if (strcmp(operation, "PUSH") == 0) return PUSH_OPCODE;
    if (strcmp(operation, "ADD" ) == 0) return ADD_OPCODE ;
    if (strcmp(operation, "SUB" ) == 0) return SUB_OPCODE ;
    if (strcmp(operation, "MUL" ) == 0) return MUL_OPCODE ;
    if (strcmp(operation, "DIV" ) == 0) return DIV_OPCODE ;
    if (strcmp(operation, "SQRT") == 0) return SQRT_OPCODE;
    if (strcmp(operation, "HLT" ) == 0) return HLT_OPCODE ;
    return ERR_INVALID_OPERATION;
}

const char* getOperationNameByOpcode(byte opcode) {
    switch (opcode) {
        case IN_OPCODE:   return "IN"  ;
        case OUT_OPCODE:  return "OUT" ;
        case POP_OPCODE:  return "POP" ;
        case PUSH_OPCODE: return "PUSH";
        case ADD_OPCODE:  return "ADD" ;
        case SUB_OPCODE:  return "SUB" ;
        case MUL_OPCODE:  return "MUL" ;
        case DIV_OPCODE:  return "DIV" ;
        case SQRT_OPCODE: return "SQRT";
        case HLT_OPCODE:  return "HLT" ;
        default: return nullptr;
    }
}

byte getOperationArityByOpcode(unsigned char opcode) {
    switch (opcode) {
        case IN_OPCODE:
        case OUT_OPCODE:
        case POP_OPCODE:
        case ADD_OPCODE:
        case SUB_OPCODE:
        case MUL_OPCODE:
        case DIV_OPCODE:
        case SQRT_OPCODE:
        case HLT_OPCODE:
            return 0;
        case PUSH_OPCODE:
            return 1;
        default:
            return ERR_INVALID_OPERATION;
    }
}

byte parseOperation(char*& line) {
    assert(line != nullptr);

    char* end = line;
    while (!isspace(*end) && (*end != '\0')) ++end;
    *end = '\0';

    byte opcode = getOpcodeByOperationName(line);

    line = end + 1;

    return opcode;
}

/**
 * Converts the given C-style string to a double.
 * @param[in] string string to convert
 * @param[out] value converted value
 * @return true, if conversion succeeded, false otherwise.
 */
static bool toDouble(const char* string, double& value) {
    assert(string != nullptr);

    char* endptr = nullptr;
    value = strtod(string, &endptr);
    return (*endptr == '\0') && (endptr != string);
}

double parseOperand(char*& line) {
    assert(line != nullptr);

    char* end = line;
    while (*end != '\0' && *end != ' ' && *end != '\n') ++end;
    *end = '\0';

    double operand = FP_NAN;
    if (!toDouble(line, operand)) operand = FP_NAN;
    return operand;
}

byte asmReadOperation(FILE* input) {
    assert(input != nullptr);

    return fgetc(input);
}

double asmReadOperand(FILE* input) {
    assert(input != nullptr);

    doubleAsBytes doubleBytes { 0 };
    for (byte& b : doubleBytes.bytes) {
        b = fgetc(input);
    }
    return doubleBytes.doubleValue;
}

char* trim(char* s) {
    assert(s != nullptr);

    // Remove leading whitespaces
    while (isspace(*s)) ++s;

    if (strcmp(s, "") == 0) return s;

    // Remove trailing whitespaces
    char* end = s + strlen(s) - 1;
    while (isspace(*end)) --end;
    *(end + 1) = '\0';

    return s;
}

void asmWrite(FILE* output, byte b) {
    assert(output != nullptr);

    fputc(b, output);
}

void asmWrite(FILE* output, double value) { // TODO: Move to util.h, util.cpp?
    assert(output != nullptr);

    doubleAsBytes doubleBytes { value };
    for (byte b : doubleBytes.bytes) {
        fputc(b, output);
    }
}

void disasmWrite(FILE* output, const char* line) {
    assert(output != nullptr);
    assert(line != nullptr);

    fprintf(output, "%s", line);
}

void disasmWriteOperation(FILE* output, const char* operation) {
    assert(output != nullptr);
    assert(operation != nullptr);

    fputs(operation, output);
}

void disasmWriteOperand(FILE* output, double operand) {
    assert(output != nullptr);

    fprintf(output, " %lf", operand);
}
