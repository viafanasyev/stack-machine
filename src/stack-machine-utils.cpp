/**
 * @file
 * @brief Implementation of stack machine helper functions.
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

/**
 * Gets the operation code by it's name.
 * @param[in] operation name of the operation
 * @return operation code, or ERR_INVALID_OPERATION if operation is invalid.
 */
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

/**
 * Gets the operation name by it's operation code.
 * @param[in] opcode code of the operation
 * @return operation name, or ERR_INVALID_OPERATION if operation is invalid.
 */
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

/**
 * Gets the arity of the operation by it's operation code
 * @param[in] opcode code of the operation
 * @return arity of the operation, or ERR_INVALID_OPERATION if operation is invalid.
 */
byte getOperationArityByOpcode(byte opcode) {
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

/**
 * Parses first possible operation from the given string. Note that the given string is also modified (pointer moved to the next instruction).
 * @param[in, out] line string to parse operation from
 * @return parsed operation code, or ERR_INVALID_OPERATION of operation is invalid.
 */
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

/**
 * Parses first possible double operand from the given string. Note that the given string is also modified (pointer moved to the next instruction).
 * @param[in, out] line string to parse operand from
 * @return parsed operand, or FP_NAN if operand is invalid.
 */
double parseOperand(char*& line) {
    assert(line != nullptr);

    char* end = line;
    while (*end != '\0' && *end != ' ' && *end != '\n') ++end;
    *end = '\0';

    double operand = FP_NAN;
    if (!toDouble(line, operand)) operand = FP_NAN;
    return operand;
}

/**
 * Reads the next operation from assembly file.
 * @param[in] input assembly file
 * @return operation code of the operation read.
 */
byte asmReadOperation(FILE* input) {
    assert(input != nullptr);

    return fgetc(input);
}

/**
 * Reads the next double operand from assembly file.
 * @param[in] input assembly file
 * @return operand read.
 */
double asmReadOperand(FILE* input) {
    assert(input != nullptr);

    doubleAsBytes doubleBytes { 0 };
    for (byte& b : doubleBytes.bytes) {
        b = fgetc(input);
    }
    return doubleBytes.doubleValue;
}

/**
 * Removes leading and trailing space characters (whitespaces, '\\n', '\\t', etc) from the given C-string. Note that the given string is also changed.
 * @param[in, out] line string to remove spaces, '\\n', etc from
 * @return string without leading and trailing space characters.
 */
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

/**
 * Writes operation code into the assembly file.
 * @param[out] output assembly file
 * @param[in]  b      operation code to write
 */
void asmWrite(FILE* output, byte b) {
    assert(output != nullptr);

    fputc(b, output);
}

/**
 * Writes double operand into the assembly file.
 * @param[out] output assembly file
 * @param[in]  value  operand to write
 */
void asmWrite(FILE* output, double value) {
    assert(output != nullptr);

    doubleAsBytes doubleBytes { value };
    for (byte b : doubleBytes.bytes) {
        fputc(b, output);
    }
}

/**
 * Writes C-string into the disassembly file.
 * @param[out] output disassembly file
 * @param[in]  line   string to write
 */
void disasmWrite(FILE* output, const char* line) {
    assert(output != nullptr);
    assert(line != nullptr);

    fprintf(output, "%s", line);
}

/**
 * Writes operation name into the disassembly file.
 * @param[out] output    disassembly file
 * @param[in]  operation operation name to write
 */
void disasmWriteOperation(FILE* output, const char* operation) {
    assert(output != nullptr);
    assert(operation != nullptr);

    fputs(operation, output);
}

/**
 * Writes double operand into the disassembly file.
 * @param[out] output  disassembly file
 * @param[in]  operand operand to write
 */
void disasmWriteOperand(FILE* output, double operand) {
    assert(output != nullptr);

    fprintf(output, " %lf", operand);
}
