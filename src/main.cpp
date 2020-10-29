/**
 * @file
 */
#include <argp.h>
#include <cassert>
#include <cstdlib>
#include "stack-machine.h"

enum runningMode {
    NONE   = 0,
    ASM    = 1,
    DISASM = 2,
    RUN    = 3,
};

struct arguments {
    runningMode mode;
    const char* inputFile;
    const char* outputFile;
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
            if (args->mode != NONE && args->inputFile != nullptr) {
                argp_failure(state, -1, 0, "Too much --asm, --disasm or --run options. Only one is possible");
            }
            args->mode = (runningMode)key;
            args->inputFile = arg;
            break;
        case 'o':
            if (args->outputFile != nullptr) {
                argp_failure(state, -1, 0, "Too much --output options. Only one is possible");
            }
            args->outputFile = arg;
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

static arguments parseArgs(int argc, char* argv[]) {
    assert(argv != nullptr);

    arguments args {NONE, nullptr, nullptr };
    argp_parse(&argp, argc, argv, 0, nullptr, &args);
    assert(args.mode != NONE);

    if (args.outputFile == nullptr) {
        // TODO: Set to fileName with different extensions?

        switch (args.mode) {
            case ASM:
                args.outputFile = "out.asm";
                break;
            case DISASM:
                args.outputFile = "out.disasm";
                break;
            case RUN:
                args.outputFile = nullptr;
                break;

            case NONE:
            default:
                exit(-1);
        }
    }
    return args;
}

int main(int argc, char* argv[]) {
    arguments args = parseArgs(argc, argv);

    switch (args.mode) {
        case ASM:
            return assemble(args.inputFile, args.outputFile);
        case DISASM:
            return disassemble(args.inputFile, args.outputFile);
        case RUN:
            return run(args.inputFile);

        case NONE:
        default:
            return -1;
    }
}
