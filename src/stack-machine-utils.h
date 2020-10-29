/**
 * @file
 * @brief Declaration of stack machine helper functions and opcodes and errors.
 */
#ifndef STACK_MACHINE_STACK_MACHINE_UTILS_H
#define STACK_MACHINE_STACK_MACHINE_UTILS_H

#include <cstdio>

#define IN_OPCODE   0b00000001
#define OUT_OPCODE  0b00000010
#define POP_OPCODE  0b00000100
#define PUSH_OPCODE 0b00000101
#define ADD_OPCODE  0b00001000
#define SUB_OPCODE  0b00001001
#define MUL_OPCODE  0b00001010
#define DIV_OPCODE  0b00001011
#define SQRT_OPCODE 0b00001100
#define HLT_OPCODE  0b00000000

#define ERR_INVALID_OPERATION 0b11111111

/**
 * Gets the operation code by it's name.
 * @param[in] operation name of the operation
 * @return operation code, or ERR_INVALID_OPERATION if operation is invalid.
 */
unsigned char getOpcodeByOperationName(const char* operation);

/**
 * Gets the operation name by it's operation code.
 * @param[in] opcode code of the operation
 * @return operation name, or ERR_INVALID_OPERATION if operation is invalid.
 */
const char* getOperationNameByOpcode(unsigned char opcode);

/**
 * Gets the arity of the operation by it's operation code
 * @param[in] opcode code of the operation
 * @return arity of the operation, or ERR_INVALID_OPERATION if operation is invalid.
 */
unsigned char getOperationArityByOpcode(unsigned char opcode);

/**
 * Parses first possible operation from the given string. Note that the given string is also modified (pointer moved to the next instruction).
 * @param[in, out] line string to parse operation from
 * @return parsed operation code, or ERR_INVALID_OPERATION of operation is invalid.
 */
unsigned char parseOperation(char*& line);

/**
 * Parses first possible double operand from the given string. Note that the given string is also modified (pointer moved to the next instruction).
 * @param[in, out] line string to parse operand from
 * @return parsed operand, or FP_NAN if operand is invalid.
 */
double parseOperand(char*& line);

/**
 * Reads the next operation from assembly file.
 * @param[in] input assembly file
 * @return operation code of the operation read.
 */
unsigned char asmReadOperation(FILE* input);

/**
 * Reads the next double operand from assembly file.
 * @param[in] input assembly file
 * @return operand read.
 */
double asmReadOperand(FILE* input);

/**
 * Removes leading and trailing space characters (whitespaces, '\\n', '\\t', etc) from the given C-string. Note that the given string is also changed.
 * @param[in, out] line string to remove spaces, '\\n', etc from
 * @return string without leading and trailing space characters.
 */
char* trim(char* line);

/**
 * Writes operation code into the assembly file.
 * @param[out] output assembly file
 * @param[in]  b      operation code to write
 */
void asmWrite(FILE* output, unsigned char b);

/**
 * Writes double operand into the assembly file.
 * @param[out] output assembly file
 * @param[in]  value  operand to write
 */
void asmWrite(FILE* output, double value);

/**
 * Writes C-string into the disassembly file.
 * @param[out] output disassembly file
 * @param[in]  line   string to write
 */
void disasmWrite(FILE* output, const char* line);

/**
 * Writes operation name into the disassembly file.
 * @param[out] output    disassembly file
 * @param[in]  operation operation name to write
 */
void disasmWriteOperation(FILE* output, const char* operation);

/**
 * Writes double operand into the disassembly file.
 * @param[out] output  disassembly file
 * @param[in]  operand operand to write
 */
void disasmWriteOperand(FILE* output, double operand);

#endif // STACK_MACHINE_STACK_MACHINE_UTILS_H
