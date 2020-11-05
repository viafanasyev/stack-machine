/**
 * @file
 */
#include <argp.h>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include "stack-machine.h"
#include "stack-machine-utils.h"

enum runningMode {
    NONE   = 0,
    ASM    = 1,
    DISASM = 2,
    RUN    = 3,
};

static constexpr size_t maxFileNameLength = 256;
static const char* assemblyFileExtension = ".asm";
static const char* disassemblyFileExtension = ".txt";

struct arguments {
    runningMode mode;
    char inputFile[maxFileNameLength];
    char outputFile[maxFileNameLength];
};

static char doc[] = "Stack machine with possibility to assemble, disassemble and run programs";

static struct argp_option options[] = {
        { "asm",    ASM,    "file.txt", 0,            "Assemble the source file into the runnable file",        0 },
        { "disasm", DISASM, "file.asm", 0,            "Disassemble the the runnable file into the source file", 0 },
        { "run",    RUN,    "file.asm", 0,            "Run the runnable file",                                  0 },
        { "output", 'o',    "file",     0,            "Output the program result into the specified file",      0 },
};

static error_t parseOption(int key, char* arg, struct argp_state* state) {
    auto args = (arguments*)(state->input);

    switch (key) {
        case ASM:
        case DISASM:
        case RUN:
            if (args->mode != NONE && strlen(args->inputFile) != 0) {
                argp_failure(state, -1, 0, "Too much --asm, --disasm or --run options. Only one is possible");
            }
            args->mode = (runningMode)key;
            strcpy(args->inputFile, arg);
            break;
        case 'o':
            if (strlen(args->outputFile) != 0) {
                argp_failure(state, -1, 0, "Too much --output options. Only one is possible");
            }
            strcpy(args->outputFile, arg);
            break;

        case ARGP_KEY_ARG:
            if (state->arg_num != 0) argp_usage(state);
            break;
        case ARGP_KEY_END:
            if (state->arg_num != 0) argp_usage(state);
            if (args->mode == NONE) argp_usage(state);
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = { options, parseOption, nullptr, doc, nullptr, nullptr, nullptr };

static void stripExtension(char* fileName) {
    assert(fileName != nullptr);

    char* end = fileName + strlen(fileName);

    while ((end > fileName) && (*end != '.') && (*end != '\\') && (*end != '/')) {
        --end;
    }

    if ((end > fileName) && (*end == '.') && (*(end - 1) != '\\') && (*(end - 1) != '/')) {
        *end = '\0';
    }
}

static void replaceExtension(char* destination, const char* originalFileName, const char* newExtension) {
    assert(originalFileName != nullptr);
    assert(newExtension != nullptr);

    char tmp[maxFileNameLength];
    strcpy(tmp, originalFileName);
    stripExtension(tmp);
    strcpy(destination, strcat(tmp, newExtension));
}

static arguments parseArgs(int argc, char* argv[]) {
    assert(argv != nullptr);

    arguments args { NONE, "", "" };
    argp_parse(&argp, argc, argv, 0, nullptr, &args);
    assert(args.mode != NONE);

    if (strlen(args.outputFile) == 0) {
        switch (args.mode) {
            case ASM:
                replaceExtension(args.outputFile, args.inputFile, assemblyFileExtension);
                break;
            case DISASM:
                replaceExtension(args.outputFile, args.inputFile, disassemblyFileExtension);
                break;
            case RUN:
                /* Do nothing */
                break;
            default:
                exit(-1);
        }
    }
    return args;
}

int main(int argc, char* argv[]) {
    arguments args = parseArgs(argc, argv);

    int exitCode = 0;

    switch (args.mode) {
        case ASM:
            exitCode = assemble(args.inputFile, args.outputFile);
            break;
        case DISASM:
            exitCode = disassemble(args.inputFile, args.outputFile);
            break;
        case RUN:
            exitCode = run(args.inputFile);
            break;

        case NONE:
        default:
            fprintf(stderr, "Invalid mode");
            exitCode = -1;
    }

    if (exitCode == ERR_INVALID_OPERATION) {
        fprintf(stderr, "Invalid operation met\n");
    } else if (exitCode == ERR_INVALID_REGISTER) {
        fprintf(stderr, "Invalid register met\n");
    } else if (exitCode == ERR_STACK_UNDERFLOW) {
        fprintf(stderr, "Stack underflow\n");
    } else if (exitCode == ERR_INVALID_LABEL) {
        fprintf(stderr, "Invalid label\n");
    }

    return exitCode;
}
