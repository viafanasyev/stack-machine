/**
 * @file
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

unsigned char getOpcodeByOperationName(const char* operation);

const char* getOperationNameByOpcode(unsigned char opcode);

unsigned char getOperationArityByOpcode(unsigned char opcode);

unsigned char parseOperation(char*& line);

double parseOperand(char*& line);

unsigned char asmReadOperation(FILE* input);

double asmReadOperand(FILE* input);

char* trim(char* line);

void asmWrite(FILE* output, unsigned char b);

void asmWrite(FILE* output, double value);

void disasmWrite(FILE* output, const char* line);

void disasmWriteOperation(FILE* output, const char* operation);

void disasmWriteOperand(FILE* output, double operand);

#endif // STACK_MACHINE_STACK_MACHINE_UTILS_H
