/**
 * @file
 * @brief Declaration of stack machine helper functions and opcodes and errors.
 */
#ifndef STACK_MACHINE_STACK_MACHINE_UTILS_H
#define STACK_MACHINE_STACK_MACHINE_UTILS_H

#include <cstdio>

#define IN_OPCODE   0b00000001u
#define OUT_OPCODE  0b00000010u
#define POP_OPCODE  0b00000100u
#define PUSH_OPCODE 0b00000101u
#define ADD_OPCODE  0b00001000u
#define SUB_OPCODE  0b00001001u
#define MUL_OPCODE  0b00001010u
#define DIV_OPCODE  0b00001011u
#define SQRT_OPCODE 0b00001100u
#define HLT_OPCODE  0b00000000u

#define ERR_INVALID_OPERATION 0b11111111u
#define ERR_INVALID_REGISTER  0b11111110u

#define REGISTERS_NUMBER 4u
#define IS_REG_OP_MASK 0b10000000u

#define PUSHR_OPCODE (PUSH_OPCODE | IS_REG_OP_MASK)
#define POPR_OPCODE  (POP_OPCODE  | IS_REG_OP_MASK)

/**
 * Gets the operation code by it's name.
 * @param[in] operation name of the operation
 * @return operation code, or ERR_INVALID_OPERATION if operation is invalid.
 */
unsigned char getOpcodeByOperationName(const char* operation);

/**
 * Gets the operation name by it's operation code.
 * @param[in] opcode code of the operation
 * @return operation name, or nullptr if operation is invalid.
 */
const char* getOperationNameByOpcode(unsigned char opcode);

/**
 * Gets the arity of the operation by it's operation code
 * @param[in] opcode code of the operation
 * @return arity of the operation, or ERR_INVALID_OPERATION if operation is invalid.
 */
unsigned char getOperationArityByOpcode(unsigned char opcode);

/**
 * Gets the register number by it's name.
 * @param[in] regName name of the register
 * @return register number, or ERR_INVALID_REGISTER if register is invalid.
 */
unsigned char getRegisterNumberByName(const char* regName);

/**
 * Gets the register name by it's number.
 * @param[in] regNumber number of the register
 * @return register name, or nullptr if register is invalid.
 */
const char* getRegisterNameByNumber(unsigned char regNumber);

/**
 * Gets the next token (char sequence between space characters) from the given string.
 * Note that the given string is also modified (pointer moved to the next token).
 * @param[in, out] line string to get token from
 * @return token read.
 */
char* getNextToken(char*& line);

/**
 * Parses first possible operation from the given string.
 * Note that the given string is also modified (pointer moved to the next token).
 * @param[in, out] line string to parse operation from
 * @return parsed operation code, or ERR_INVALID_OPERATION if operation is invalid.
 */
unsigned char parseOperation(char*& line);

/**
 * Parses first possible double operand from the given string.
 * Note that the given string is also modified (pointer moved to the next token).
 * @param[in, out] line string to parse operand from
 * @return parsed operand, or FP_NAN if operand is invalid.
 */
double parseOperand(char*& line);

/**
 * Parses first possible register from the given string. Note that the given string is also modified (pointer moved to the next instruction).
 * @param[in, out] line string to parse register from
 * @return parsed register number, or ERR_INVALID_REGISTER if register is invalid.
 */
unsigned char parseRegister(char*& line);

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
 * Reads the next register from assembly file.
 * @param[in] input assembly file
 * @return register number read, or ERR_INVALID_REGISTER, if register number is invalid.
 */
unsigned char asmReadRegister(FILE* input);

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

/**
 * Writes register name into the disassembly file.
 * @param[out] output  disassembly file
 * @param[in]  regName register name to write
 */
void disasmWriteRegister(FILE* output, const char* regName);

#endif // STACK_MACHINE_STACK_MACHINE_UTILS_H
