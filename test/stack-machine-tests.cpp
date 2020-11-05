/**
 * @file
 */
#include "testlib.h"
#include "../src/stack-machine.h"
#include "../src/stack-machine-utils.h"

TEST(failures, emptyStackPop_stackUnderflowErrorCodeReturned) {
    const char* asmTestFileName = "ASM_TEST_FILE_NAME.txt";
    FILE* asmTestFile = fopen(asmTestFileName, "wb");
    int dummy = 0;
    asmWrite(asmTestFile, (unsigned char)POP_OPCODE, dummy);
    asmWrite(asmTestFile, (unsigned char)HLT_OPCODE, dummy);
    fflush(asmTestFile);
    fclose(asmTestFile);

    int exitCode = run(asmTestFileName);

    ASSERT_EQUALS(exitCode, ERR_STACK_UNDERFLOW);
}

TEST(failures, singleOperandStackAdd_stackUnderflowErrorCodeReturned) {
    const char* asmTestFileName = "ASM_TEST_FILE_NAME.txt";
    FILE* asmTestFile = fopen(asmTestFileName, "wb");
    int dummy = 0;
    asmWrite(asmTestFile, (unsigned char)PUSH_OPCODE, dummy);
    asmWrite(asmTestFile, 0.0, dummy);
    asmWrite(asmTestFile, (unsigned char)ADD_OPCODE, dummy);
    asmWrite(asmTestFile, (unsigned char)HLT_OPCODE, dummy);
    fflush(asmTestFile);
    fclose(asmTestFile);

    int exitCode = run(asmTestFileName);

    ASSERT_EQUALS(exitCode, ERR_STACK_UNDERFLOW);
}

TEST(failures, singleOperandStackSub_stackUnderflowErrorCodeReturned) {
    const char* asmTestFileName = "ASM_TEST_FILE_NAME.txt";
    FILE* asmTestFile = fopen(asmTestFileName, "wb");
    int dummy = 0;
    asmWrite(asmTestFile, (unsigned char)PUSH_OPCODE, dummy);
    asmWrite(asmTestFile, 0.0, dummy);
    asmWrite(asmTestFile, (unsigned char)SUB_OPCODE, dummy);
    asmWrite(asmTestFile, (unsigned char)HLT_OPCODE, dummy);
    fflush(asmTestFile);
    fclose(asmTestFile);

    int exitCode = run(asmTestFileName);

    ASSERT_EQUALS(exitCode, ERR_STACK_UNDERFLOW);
}

TEST(failures, singleOperandStackMul_stackUnderflowErrorCodeReturned) {
    const char* asmTestFileName = "ASM_TEST_FILE_NAME.txt";
    FILE* asmTestFile = fopen(asmTestFileName, "wb");
    int dummy = 0;
    asmWrite(asmTestFile, (unsigned char)PUSH_OPCODE, dummy);
    asmWrite(asmTestFile, 0.0, dummy);
    asmWrite(asmTestFile, (unsigned char)MUL_OPCODE, dummy);
    asmWrite(asmTestFile, (unsigned char)HLT_OPCODE, dummy);
    fflush(asmTestFile);
    fclose(asmTestFile);

    int exitCode = run(asmTestFileName);

    ASSERT_EQUALS(exitCode, ERR_STACK_UNDERFLOW);
}

TEST(failures, singleOperandStackDiv_stackUnderflowErrorCodeReturned) {
    const char* asmTestFileName = "ASM_TEST_FILE_NAME.txt";
    FILE* asmTestFile = fopen(asmTestFileName, "wb");
    int dummy = 0;
    asmWrite(asmTestFile, (unsigned char)PUSH_OPCODE, dummy);
    asmWrite(asmTestFile, 0.0, dummy);
    asmWrite(asmTestFile, (unsigned char)DIV_OPCODE, dummy);
    asmWrite(asmTestFile, (unsigned char)HLT_OPCODE, dummy);
    fflush(asmTestFile);
    fclose(asmTestFile);

    int exitCode = run(asmTestFileName);

    ASSERT_EQUALS(exitCode, ERR_STACK_UNDERFLOW);
}

TEST(failures, emptyStackSqrt_stackUnderflowErrorCodeReturned) {
    const char* asmTestFileName = "ASM_TEST_FILE_NAME.txt";
    FILE* asmTestFile = fopen(asmTestFileName, "wb");
    int dummy = 0;
    asmWrite(asmTestFile, (unsigned char)SQRT_OPCODE, dummy);
    asmWrite(asmTestFile, (unsigned char)HLT_OPCODE, dummy);
    fflush(asmTestFile);
    fclose(asmTestFile);

    int exitCode = run(asmTestFileName);

    ASSERT_EQUALS(exitCode, ERR_STACK_UNDERFLOW);
}

TEST(failures, emptyStackOut_stackUnderflowErrorCodeReturned) {
    const char* asmTestFileName = "ASM_TEST_FILE_NAME.txt";
    FILE* asmTestFile = fopen(asmTestFileName, "wb");
    int dummy = 0;
    asmWrite(asmTestFile, (unsigned char)OUT_OPCODE, dummy);
    asmWrite(asmTestFile, (unsigned char)HLT_OPCODE, dummy);
    fflush(asmTestFile);
    fclose(asmTestFile);

    int exitCode = run(asmTestFileName);

    ASSERT_EQUALS(exitCode, ERR_STACK_UNDERFLOW);
}

TEST(failures, invalidOpcode_invalidOperationErrorCodeReturned) {
    const char* asmTestFileName = "ASM_TEST_FILE_NAME.txt";
    FILE* asmTestFile = fopen(asmTestFileName, "wb");
    int dummy = 0;
    asmWrite(asmTestFile, (unsigned char)ERR_INVALID_OPERATION, dummy); // ERR_INVALID_OPERATION is absolutely invalid opcode
    asmWrite(asmTestFile, (unsigned char)HLT_OPCODE, dummy);
    fflush(asmTestFile);
    fclose(asmTestFile);

    int exitCode = run(asmTestFileName);

    ASSERT_EQUALS(exitCode, ERR_INVALID_OPERATION);
}

TEST(failures, invalidRegister_invalidRegisterErrorCodeReturned) {
    const char* asmTestFileName = "ASM_TEST_FILE_NAME.txt";
    FILE* asmTestFile = fopen(asmTestFileName, "wb");
    int dummy = 0;
    asmWrite(asmTestFile, (unsigned char)PUSHR_OPCODE, dummy);
    asmWrite(asmTestFile, (unsigned char)(REGISTERS_NUMBER + 1), dummy);
    asmWrite(asmTestFile, (unsigned char)HLT_OPCODE, dummy);
    fflush(asmTestFile);
    fclose(asmTestFile);

    int exitCode = run(asmTestFileName);

    ASSERT_EQUALS(exitCode, ERR_INVALID_REGISTER);
}
