/**
 * @file
 * @brief Declaration of stack machine helper functions and opcodes and errors.
 */
#ifndef STACK_MACHINE_STACK_MACHINE_UTILS_H
#define STACK_MACHINE_STACK_MACHINE_UTILS_H

#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <map>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#define IN_OPCODE    0b00000001u
#define OUT_OPCODE   0b00000010u
#define POP_OPCODE   0b00000100u
#define PUSH_OPCODE  0b00000101u
#define ADD_OPCODE   0b00001000u
#define SUB_OPCODE   0b00001001u
#define MUL_OPCODE   0b00001010u
#define DIV_OPCODE   0b00001011u
#define SQRT_OPCODE  0b00001100u
#define DUP_OPCODE   0b00001101u

#define JMP_OPCODE   0b00100000u
#define JMPNE_OPCODE 0b00100010u // !=
#define JMPE_OPCODE  0b00100011u // ==
#define JMPL_OPCODE  0b00100100u // <
#define JMPLE_OPCODE 0b00100101u // <=
#define JMPG_OPCODE  0b00100110u // >
#define JMPGE_OPCODE 0b00100111u // >=

#define RET_OPCODE   0b00110000u
#define CALL_OPCODE  0b00110001u

#define HLT_OPCODE   0b00000000u

#define ERR_INVALID_OPERATION   0b11111111u
#define ERR_INVALID_REGISTER    0b11111110u
#define ERR_STACK_UNDERFLOW     0b11111101u
#define ERR_INVALID_LABEL       0b11111100u
#define ERR_INVALID_FILE        0b11111011u

#define REGISTERS_NUMBER 4u
#define IS_REG_OP_MASK 0b10000000u

#define PUSHR_OPCODE (PUSH_OPCODE | IS_REG_OP_MASK)
#define POPR_OPCODE  (POP_OPCODE  | IS_REG_OP_MASK)

#define COMPARE_EPS 1e-9

class AssemblyMachine {

protected:
    double* registers = nullptr;
    int pc = -1;
    unsigned char* assembly = nullptr;
    int assemblySize = -1;

public:
    explicit AssemblyMachine(const char* assemblyFileName);

    ~AssemblyMachine();

    AssemblyMachine(AssemblyMachine& assemblyMachine) = delete;
    AssemblyMachine &operator=(const AssemblyMachine&) = delete;

    int getAssemblySize() const {
        return assemblySize;
    }

    /**
     * Reads the next operation from assembly machine. Increases pc by the number of bytes read.
     * @param[in, out] assemblyMachine assembly machine to read operation from
     * @return operation code of the operation read.
     */
    unsigned char getNextOperation();

    /**
     * Reads the next operand from assembly machine. Increases pc by the number of bytes read.
     * @param[in, out] assemblyMachine assembly machine to read operand from
     * @return operand read.
     */
    double getNextOperand();

    /**
     * Reads the next register from assembly machine. Increases pc by the number of bytes read.
     * @param[in, out] assemblyMachine stack machine to read register from
     * @return register number read, or ERR_INVALID_REGISTER, if register number is invalid.
     */
    unsigned char getNextRegister();

    /**
     * Reads the next jump offset from assembly machine. Increases pc by the number of bytes read.
     * @param[in, out] assemblyMachine assembly machine to read jump offset from
     * @return jump offset.
     */
    int getNextJumpOffset();

    /**
     * Processes the no-operand operation.
     * @param[in] opcode code of the operation to process
     * @return given operation code or error code, if operation was invalid.
     */
    virtual unsigned char processOperation(unsigned char opcode) = 0;

    /**
     * Processes the single operand operation.
     * @param[in]      opcode  code of the operation to process
     * @param[in, out] operand operand to process
     * @return given operation code or error code, if operation was invalid.
     */
    virtual unsigned char processOperation(unsigned char opcode, double& operand) = 0;

    /**
     * Processes the jump operation.
     * @param[in] opcode     code of the jump operation to process
     * @param[in] jumpOffset offset of the jump to process
     * @return given operation code or error code, if operation was invalid.
     */
    virtual unsigned char processJumpOperation(unsigned char opcode, int jumpOffset) = 0;

    /**
     * Processes the next operation.
     * @return processed operation code or error code, if operation was invalid.
     */
    unsigned char processNextOperation();
};

/**
 * Associative table for code labels. Stores labels' names and offsets.
 */
class LabelTable {
    constexpr static unsigned int MAX_LINE_LENGTH = 256u;

    struct keyCompare {
        bool operator()(char* a, char* b) const {
            return strcmp(a, b) < 0;
        }
    };

    std::map<char*, unsigned int, keyCompare> labels;

public:
    LabelTable() = default;

    LabelTable(const LabelTable& labelTable);

    void swap(LabelTable& other);

    LabelTable& operator=(LabelTable other);

    /**
     * Gets the label offset by it's name.
     * @param[in] labelName name of the label to find
     * @return label offset, or -1, if there is no label with the given name.
     */
    int getLabelOffset(char* labelName);

    /**
     * Creates new label from the given name and offset.
     * @param[in] line        line containing name of the label. Can contain ':' symbol at the end.
     * @param[in] labelOffset offset of the label
     * @return 0, if label was created successfully, or ERR_INVALID_LABEL, if label with the given name already exists.
     */
    unsigned char addLabel(const char* line, unsigned int labelOffset);

    ~LabelTable();
};

/**
 * Buffer for disassembly file. Stores lines of code and labels to be inserted in this code.
 */
class DisassemblyBuffer {
    constexpr static unsigned int MAX_LINE_LENGTH = 256u;
    constexpr static unsigned int MAX_LABEL_LENGTH = 12u; // 1 ('L') + 10 (max int length) + 1 ('\0')

    /** Contains lines to output in the file with the offset delta associated with this line **/
    std::vector<std::pair<char*, int>> lines;

    /** Contains labels mapped by their offset **/
    std::map<unsigned int, char*> labels;

public:
    /**
     * Writes operation name into the disassembly buffer.
     * @param[in] operation operation name to write
     */
    void writeOperation(const char* operation);

    /**
     * Writes double operand into the disassembly buffer.
     * @param[in] operand operand to write
     */
    void writeOperand(double operand);

    /**
     * Writes register name into the disassembly buffer.
     * @param[in] regName register name to write
     */
    void writeRegister(const char* regName);

    /**
     * Creates and writes jump label (as argument of JMP or similar operation) into the disassembly buffer.
     * Jump labels are created from offsets, and then put into disassembly file in flushToFile method.
     * @param[in] labelOffset jump label offset
     */
    void writeJumpLabelArgument(int labelOffset);

    /**
     * Returns the label name by it's offset. If there's no label with the given offset, the new one is created.
     * @param[in] labelOffset offset of the label to get
     * @return name of the label.
     */
    const char* getLabelByOffset(int labelOffset);

    /**
     * Flushes this buffer content into the given file. All data is cleared.
     * @param[out] output disassembly file to flush buffer into
     * @return 0, if flushing completed successfully, or ERR_INVALID_LABEL, if any of the labels was invalid.
     */
    unsigned char flushToFile(FILE* output);
};

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
 * @return parsed operand, or NAN if operand is invalid.
 */
double parseOperand(char*& line);

/**
 * Parses first possible register from the given string. Note that the given string is also modified (pointer moved to the next instruction).
 * @param[in, out] line string to parse register from
 * @return parsed register number, or ERR_INVALID_REGISTER if register is invalid.
 */
unsigned char parseRegister(char*& line);

/**
 * Checks if the given token is a label (doesn't contain spaces and ends with ':' symbol).
 * @param token  token to check
 * @return true, if the given token is a label, false otherwise.
 */
bool isLabel(const char* token);

/**
 * Checks if the give opcode is the jump operation.
 * @param opcode operation code to check
 * @return true, if the given operation is jump operation, false otherwise.
 */
bool isJumpOperation(unsigned char opcode);

/**
 * Reads the next operation from assembly file. Increases offset by the number of bytes read.
 * @param[in]      input             assembly file
 * @param[in, out] currentByteOffset current offset in bytes
 * @return operation code of the operation read.
 */
unsigned char asmReadOperation(FILE* input, int& currentByteOffset);

/**
 * Reads the next double operand from assembly file. Increases offset by the number of bytes read.
 * @param[in]      input             assembly file
 * @param[in, out] currentByteOffset current offset in bytes
 * @return operand read.
 */
double asmReadOperand(FILE* input, int& currentByteOffset);

/**
 * Reads the next register from assembly file. Increases offset by the number of bytes read.
 * @param[in]      input             assembly file
 * @param[in, out] currentByteOffset current offset in bytes
 * @return register number read, or ERR_INVALID_REGISTER, if register number is invalid.
 */
unsigned char asmReadRegister(FILE* input, int& currentByteOffset);

/**
 * Reads the next jump offset from assembly file. Increases offset by the number of bytes read.
 * @param[in]      input             assembly file
 * @param[in, out] currentByteOffset current offset in bytes
 * @return jump offset.
 */
int asmReadJumpOffset(FILE* input, int& currentByteOffset);

/**
 * Removes leading and trailing space characters (whitespaces, '\\n', '\\t', etc) from the given C-string. Note that the given string is also changed.
 * @param[in, out] s string to remove spaces, '\\n', etc from
 * @return string without leading and trailing space characters.
 */
char* trim(char*& s);

/**
 * Writes byte into the assembly file and increases offset by number of written bytes. If output file is null, just increases offset (fake write).
 * @param[out]     output            assembly file
 * @param[in]      b                 byte to write
 * @param[in, out] currentByteOffset current offset in bytes
 */
void asmWrite(FILE* output, unsigned char b, int& currentByteOffset);

/**
 * Writes double operand into the assembly file and increases offset by number of written bytes. If output file is null, just increases offset (fake write).
 * @param[out]     output            assembly file
 * @param[in]      value             operand to write
 * @param[in, out] currentByteOffset current offset in bytes
 */
void asmWrite(FILE* output, double value, int& currentByteOffset);

/**
 * Writes int operand into the assembly file and increases offset by number of written bytes. If output file is null, just increases offset (fake write).
 * @param[out]     output            assembly file
 * @param[in]      value             operand to write
 * @param[in, out] currentByteOffset current offset in bytes
 */
void asmWrite(FILE* output, int value, int& currentByteOffset);

#endif // STACK_MACHINE_STACK_MACHINE_UTILS_H
