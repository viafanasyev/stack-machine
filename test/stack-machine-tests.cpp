/**
 * @file
 */
#include "testlib.h"
#include "../src/stack-machine.h"
#include "../src/stack-machine-utils.h"

TEST(failures, emptyStackPop_stackUnderflowErrorCodeReturned) {
    const char* asmTestFileName = "ASM_TEST_FILE_NAME.txt";
    FILE* asmTestFile = fopen(asmTestFileName, "wb");
    asmWrite(asmTestFile, (unsigned char)POP_OPCODE);
    asmWrite(asmTestFile, (unsigned char)HLT_OPCODE);
    fflush(asmTestFile);
    fclose(asmTestFile);

    int exitCode = run(asmTestFileName);

    ASSERT_EQUALS(exitCode, ERR_STACK_UNDERFLOW);
}

TEST(failures, singleOperandStackAdd_stackUnderflowErrorCodeReturned) {
    const char* asmTestFileName = "ASM_TEST_FILE_NAME.txt";
    FILE* asmTestFile = fopen(asmTestFileName, "wb");
    asmWrite(asmTestFile, (unsigned char)PUSH_OPCODE);
    asmWrite(asmTestFile, 0.0);
    asmWrite(asmTestFile, (unsigned char)ADD_OPCODE);
    asmWrite(asmTestFile, (unsigned char)HLT_OPCODE);
    fflush(asmTestFile);
    fclose(asmTestFile);

    int exitCode = run(asmTestFileName);

    ASSERT_EQUALS(exitCode, ERR_STACK_UNDERFLOW);
}

TEST(failures, singleOperandStackSub_stackUnderflowErrorCodeReturned) {
    const char* asmTestFileName = "ASM_TEST_FILE_NAME.txt";
    FILE* asmTestFile = fopen(asmTestFileName, "wb");
    asmWrite(asmTestFile, (unsigned char)PUSH_OPCODE);
    asmWrite(asmTestFile, 0.0);
    asmWrite(asmTestFile, (unsigned char)SUB_OPCODE);
    asmWrite(asmTestFile, (unsigned char)HLT_OPCODE);
    fflush(asmTestFile);
    fclose(asmTestFile);

    int exitCode = run(asmTestFileName);

    ASSERT_EQUALS(exitCode, ERR_STACK_UNDERFLOW);
}

TEST(failures, singleOperandStackMul_stackUnderflowErrorCodeReturned) {
    const char* asmTestFileName = "ASM_TEST_FILE_NAME.txt";
    FILE* asmTestFile = fopen(asmTestFileName, "wb");
    asmWrite(asmTestFile, (unsigned char)PUSH_OPCODE);
    asmWrite(asmTestFile, 0.0);
    asmWrite(asmTestFile, (unsigned char)MUL_OPCODE);
    asmWrite(asmTestFile, (unsigned char)HLT_OPCODE);
    fflush(asmTestFile);
    fclose(asmTestFile);

    int exitCode = run(asmTestFileName);

    ASSERT_EQUALS(exitCode, ERR_STACK_UNDERFLOW);
}

TEST(failures, singleOperandStackDiv_stackUnderflowErrorCodeReturned) {
    const char* asmTestFileName = "ASM_TEST_FILE_NAME.txt";
    FILE* asmTestFile = fopen(asmTestFileName, "wb");
    asmWrite(asmTestFile, (unsigned char)PUSH_OPCODE);
    asmWrite(asmTestFile, 0.0);
    asmWrite(asmTestFile, (unsigned char)DIV_OPCODE);
    asmWrite(asmTestFile, (unsigned char)HLT_OPCODE);
    fflush(asmTestFile);
    fclose(asmTestFile);

    int exitCode = run(asmTestFileName);

    ASSERT_EQUALS(exitCode, ERR_STACK_UNDERFLOW);
}

TEST(failures, emptyStackSqrt_stackUnderflowErrorCodeReturned) {
    const char* asmTestFileName = "ASM_TEST_FILE_NAME.txt";
    FILE* asmTestFile = fopen(asmTestFileName, "wb");
    asmWrite(asmTestFile, (unsigned char)SQRT_OPCODE);
    asmWrite(asmTestFile, (unsigned char)HLT_OPCODE);
    fflush(asmTestFile);
    fclose(asmTestFile);

    int exitCode = run(asmTestFileName);

    ASSERT_EQUALS(exitCode, ERR_STACK_UNDERFLOW);
}

TEST(failures, emptyStackOut_stackUnderflowErrorCodeReturned) {
    const char* asmTestFileName = "ASM_TEST_FILE_NAME.txt";
    FILE* asmTestFile = fopen(asmTestFileName, "wb");
    asmWrite(asmTestFile, (unsigned char)OUT_OPCODE);
    asmWrite(asmTestFile, (unsigned char)HLT_OPCODE);
    fflush(asmTestFile);
    fclose(asmTestFile);

    int exitCode = run(asmTestFileName);

    ASSERT_EQUALS(exitCode, ERR_STACK_UNDERFLOW);
}
