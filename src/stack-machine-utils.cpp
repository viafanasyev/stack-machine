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

union intAsBytes {
    int intValue;
    byte bytes[sizeof(int)];
};

LabelTable::LabelTable(const LabelTable& labelTable) {
    for (auto entry : labelTable.labels) {
        addLabel(entry.first, entry.second);
    }
}

void LabelTable::swap(LabelTable& other) {
    std::swap(labels, other.labels);
}

LabelTable& LabelTable::operator=(LabelTable other) {
    swap(other);
    return *this;
}

/**
 * Gets the label offset by it's name.
 * @param[in] labelName name of the label to find
 * @return label offset, or -1, if there is no label with the given name.
 */
int LabelTable::getLabelOffset(char* labelName) {
    if (labels.count(labelName) == 0) return -1;
    return labels[labelName];
}

byte LabelTable::addLabel(const char* line, unsigned int labelOffset) {
    char* labelName = (char*)calloc(MAX_LINE_LENGTH, sizeof(char));
    for (unsigned int i = 0; i < MAX_LINE_LENGTH; ++i) {
        if ((line[i] == ':') || (line[i] == '\0')) {
            labelName[i] = '\0';
            break;
        }
        labelName[i] = line[i];
    }

    if (labels.count(labelName) != 0) {
        free(labelName);
        return ERR_INVALID_LABEL;
    }

    labels[labelName] = labelOffset;
    return 0;
}

LabelTable::~LabelTable() {
    for (auto entry : labels) {
        free(entry.first);
    }
}

/**
 * Writes operation name into the disassembly buffer.
 * @param[in] operation operation name to write
 */
void DisassemblyBuffer::writeOperation(const char* operation) {
    assert(operation != nullptr);

    char* line = (char*)calloc(MAX_LINE_LENGTH, sizeof(char));
    strcpy(line, operation);
    lines.push_back(line);
}

/**
 * Writes double operand into the disassembly buffer.
 * @param[in] operand operand to write
 */
void DisassemblyBuffer::writeOperand(double operand) {
    char line[MAX_LINE_LENGTH];
    sprintf(line, " %lg", operand);
    strcat(lines.back(), line);
}

/**
 * Writes register name into the disassembly buffer.
 * @param[in] regName register name to write
 */
void DisassemblyBuffer::writeRegister(const char* regName) {
    assert(regName != nullptr);

    strcat(lines.back(), " ");
    strcat(lines.back(), regName);
}

/**
 * Creates and writes jump label (as argument of JMP or similar operation) into the disassembly buffer.
 * Jump labels are created from offsets, and then put into disassembly file in flushToFile method.
 * @param[in] labelOffset jump label offset
 */
void DisassemblyBuffer::writeJumpLabelArgument(int labelOffset) {
    assert(labelOffset >= 0);

    strcat(lines.back(), " ");
    strcat(lines.back(), getLabelByOffset(labelOffset));
}

/**
 * Returns the label name by it's offset. If there's no label with the given offset, the new one is created.
 * @param[in] labelOffset offset of the label to get
 * @return name of the label.
 */
const char* DisassemblyBuffer::getLabelByOffset(int labelOffset) {
    assert(labelOffset >= 0);

    if (labels.count(labelOffset) == 0) {
        char* labelName = (char*)calloc(MAX_LABEL_LENGTH, sizeof(char));
        unsigned int labelsNumber = labels.size();
        sprintf(labelName, "L%u", labelsNumber);
        labels[labelOffset] = labelName;
    }
    return labels[labelOffset];
}

/**
 * Flushes this buffer content into the given file. All data is cleared.
 * @param[out] output disassembly file to flush buffer into
 * @return 0, if flushing completed successfully, or ERR_INVALID_LABEL, if any of the labels was invalid.
 */
byte DisassemblyBuffer::flushToFile(FILE* output) {
    assert(output != nullptr);

    for (size_t i = 0; i < lines.size(); ++i) {
        if (labels.count(i) != 0) {
            char* label = labels[i];
            fprintf(output, "%s:\n", label);
            free(label);
            labels.erase(i);
        }

        char* line = lines[i];
        fprintf(output, "%s\n", line);
        free(line);
    }
    lines.clear();

    // If there are any labels left after the code, then they are invalid, because they point to an empty fragment
    if (!labels.empty()) {
        for (auto entry : labels) {
            free(entry.second);
        }
        return ERR_INVALID_LABEL;
    }

    return 0;
}

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
    if (strcmp(operation, "JMP" ) == 0) return JMP_OPCODE ;
    return ERR_INVALID_OPERATION;
}

/**
 * Gets the operation name by it's operation code.
 * @param[in] opcode code of the operation
 * @return operation name, or nullptr if operation is invalid.
 */
const char* getOperationNameByOpcode(byte opcode) {
    switch (opcode) {
        case IN_OPCODE:   return "IN"  ;
        case OUT_OPCODE:  return "OUT" ;
        case POP_OPCODE:  case POPR_OPCODE:  return "POP" ;
        case PUSH_OPCODE: case PUSHR_OPCODE: return "PUSH";
        case ADD_OPCODE:  return "ADD" ;
        case SUB_OPCODE:  return "SUB" ;
        case MUL_OPCODE:  return "MUL" ;
        case DIV_OPCODE:  return "DIV" ;
        case SQRT_OPCODE: return "SQRT";
        case HLT_OPCODE:  return "HLT" ;
        case JMP_OPCODE:  return "JMP" ;
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
        case PUSHR_OPCODE:
        case POPR_OPCODE:
        case JMP_OPCODE:
            return 1;
        default:
            return ERR_INVALID_OPERATION;
    }
}

/**
 * Gets the register number by it's name.
 * @param[in] regName name of the register
 * @return register number, or ERR_INVALID_REGISTER if register is invalid.
 */
byte getRegisterNumberByName(const char* regName) {
    assert(regName != nullptr);

    if (strcmp(regName, "AX") == 0) return 0;
    if (strcmp(regName, "BX") == 0) return 1;
    if (strcmp(regName, "CX") == 0) return 2;
    if (strcmp(regName, "DX") == 0) return 3;
    return ERR_INVALID_REGISTER;
}

/**
 * Gets the register name by it's number.
 * @param[in] regNumber number of the register
 * @return register name, or nullptr if register is invalid.
 */
const char* getRegisterNameByNumber(byte regNumber) {
    switch (regNumber) {
        case 0: return "AX";
        case 1: return "BX";
        case 2: return "CX";
        case 3: return "DX";
        default: return nullptr;
    }
}

/**
 * Gets the next token (char sequence between space characters) from the given string.
 * Note that the given string is also modified (pointer moved to the next token).
 * @param[in, out] line string to get token from
 * @return token read.
 */
char* getNextToken(char*& line) {
    assert(line != nullptr);

    char* end = line;
    while (!isspace(*end) && (*end != '\0')) ++end;
    *end = '\0';

    char* token = line;

    line = end + 1;

    return token;
}

/**
 * Parses first possible operation from the given string.
 * Note that the given string is also modified (pointer moved to the next token).
 * @param[in, out] line string to parse operation from
 * @return parsed operation code, or ERR_INVALID_OPERATION if operation is invalid.
 */
byte parseOperation(char*& line) {
    assert(line != nullptr);

    return getOpcodeByOperationName(getNextToken(line));
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
 * Parses first possible double operand from the given string.
 * Note that the given string is also modified (pointer moved to the next token).
 * @param[in, out] line string to parse operand from
 * @return parsed operand, or FP_NAN if operand is invalid.
 */
double parseOperand(char*& line) {
    assert(line != nullptr);

    const char* token = getNextToken(line);

    double operand = FP_NAN;
    if (!toDouble(token, operand)) operand = FP_NAN;
    return operand;
}

/**
 * Parses first possible register from the given string.
 * Note that the given string is also modified (pointer moved to the next token).
 * @param[in, out] line string to parse register from
 * @return parsed register number, or ERR_INVALID_REGISTER if register is invalid.
 */
byte parseRegister(char*& line) {
    assert(line != nullptr);

    return getRegisterNumberByName(getNextToken(line));
}

/**
 * Checks if the given token is a label (doesn't contain spaces and ends with ':' symbol).
 * @param[in] token token to check
 * @return true, if the given token is a label, false otherwise.
 */
bool isLabel(const char* token) {
    const char* end = token + strlen(token) - 1;
    while (end > token && isspace(*end) && (*end != ':')) {
        --end;
    }
    if ((end >= token) && (*end == ':')) {
        while (--end >= token) {
            if (isspace(*end)) return false;
        }
        return true;
    } else {
        return false;
    }
}

/**
 * Checks if the give opcode is the jump operation.
 * @param[in] opcode operation code to check
 * @return true, if the given operation is jump operation, false otherwise.
 */
bool isJumpOperation(byte opcode) {
    return opcode == JMP_OPCODE;
}

/**
 * Reads the next operation from assembly file. Increases offset by the number of bytes read.
 * @param[in]      input             assembly file
 * @param[in, out] currentByteOffset current offset in bytes
 * @return operation code of the operation read.
 */
byte asmReadOperation(FILE* input, int& currentByteOffset) {
    assert(input != nullptr);

    currentByteOffset += sizeof(byte);
    return fgetc(input);
}

/**
 * Reads the next operation from assembly machine. Increases pc by the number of bytes read.
 * @param[in, out] assemblyMachine assembly machine to read operation from
 * @return operation code of the operation read.
 */
byte asmReadOperation(AssemblyMachine* assemblyMachine) {
    assert(assemblyMachine != nullptr);

    byte opCode = assemblyMachine->assembly[assemblyMachine->pc];
    assemblyMachine->pc += sizeof(byte);
    return opCode;
}

/**
 * Reads the next double operand from assembly file. Increases offset by the number of bytes read.
 * @param[in]      input             assembly file
 * @param[in, out] currentByteOffset current offset in bytes
 * @return operand read.
 */
double asmReadOperand(FILE* input, int& currentByteOffset) {
    assert(input != nullptr);

    currentByteOffset += sizeof(double);
    doubleAsBytes doubleBytes { 0 };
    for (byte& b : doubleBytes.bytes) {
        b = fgetc(input);
    }
    return doubleBytes.doubleValue;
}

/**
 * Reads the next operand from assembly machine. Increases pc by the number of bytes read.
 * @param[in, out] assemblyMachine assembly machine to read operand from
 * @return operand read.
 */
double asmReadOperand(AssemblyMachine* assemblyMachine) {
    assert(assemblyMachine != nullptr);

    doubleAsBytes doubleBytes { 0 };
    for (byte& b : doubleBytes.bytes) {
        b = assemblyMachine->assembly[assemblyMachine->pc];
        assemblyMachine->pc += sizeof(byte);
    }
    return doubleBytes.doubleValue;
}

/**
 * Reads the next register from assembly file. Increases offset by the number of bytes read.
 * @param[in]      input             assembly file
 * @param[in, out] currentByteOffset current offset in bytes
 * @return register number read, or ERR_INVALID_REGISTER, if register number is invalid.
 */
byte asmReadRegister(FILE* input, int& currentByteOffset) {
    assert(input != nullptr);

    currentByteOffset += sizeof(byte);
    byte reg = fgetc(input);
    if (reg >= REGISTERS_NUMBER) return ERR_INVALID_REGISTER;
    return reg;
}

/**
 * Reads the next register from assembly machine. Increases pc by the number of bytes read.
 * @param[in, out] assemblyMachine stack machine to read register from
 * @return register number read, or ERR_INVALID_REGISTER, if register number is invalid.
 */
byte asmReadRegister(AssemblyMachine* assemblyMachine) {
    assert(assemblyMachine != nullptr);

    byte reg = assemblyMachine->assembly[assemblyMachine->pc];
    assemblyMachine->pc += sizeof(byte);
    if (reg >= REGISTERS_NUMBER) return ERR_INVALID_REGISTER;
    return reg;
}

/**
 * Reads the next jump offset from assembly file. Increases offset by the number of bytes read.
 * @param[in]      input             assembly file
 * @param[in, out] currentByteOffset current offset in bytes
 * @return jump offset.
 */
int asmReadJumpOffset(FILE* input, int& currentByteOffset) {
    assert(input != nullptr);

    currentByteOffset += sizeof(int);
    intAsBytes intBytes { 0 };
    for (byte& b : intBytes.bytes) {
        b = fgetc(input);
    }
    return intBytes.intValue;
}

/**
 * Reads the next jump offset from assembly machine. Increases pc by the number of bytes read.
 * @param[in, out] assemblyMachine assembly machine to read jump offset from
 * @return jump offset.
 */
int asmReadJumpOffset(AssemblyMachine* assemblyMachine) {
    assert(assemblyMachine != nullptr);

    intAsBytes intBytes { 0 };
    for (byte& b : intBytes.bytes) {
        b = assemblyMachine->assembly[assemblyMachine->pc];
        assemblyMachine->pc += sizeof(byte);
    }
    return intBytes.intValue;
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
 * Writes byte into the assembly file and increases offset by number of written bytes. If output file is null, just increases offset (fake write).
 * @param[out]     output            assembly file
 * @param[in]      b                 byte to write
 * @param[in, out] currentByteOffset current offset in bytes
 */
void asmWrite(FILE* output, byte b, int& currentByteOffset) {
    currentByteOffset += sizeof(b);
    if (output != nullptr) {
        fputc(b, output);
    }
}

/**
 * Writes double operand into the assembly file and increases offset by number of written bytes. If output file is null, just increases offset (fake write).
 * @param[out]     output            assembly file
 * @param[in]      value             operand to write
 * @param[in, out] currentByteOffset current offset in bytes
 */
void asmWrite(FILE* output, double value, int& currentByteOffset) {
    currentByteOffset += sizeof(value);
    if (output != nullptr) {
        doubleAsBytes doubleBytes{value};
        for (byte b : doubleBytes.bytes) {
            fputc(b, output);
        }
    }
}

/**
 * Writes int operand into the assembly file and increases offset by number of written bytes. If output file is null, just increases offset (fake write).
 * @param[out]     output            assembly file
 * @param[in]      value             operand to write
 * @param[in, out] currentByteOffset current offset in bytes
 */
void asmWrite(FILE* output, int value, int& currentByteOffset) {
    currentByteOffset += sizeof(value);
    if (output != nullptr) {
        intAsBytes intBytes{value};
        for (byte b : intBytes.bytes) {
            fputc(b, output);
        }
    }
}
