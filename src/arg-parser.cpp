/**
 * @file
 * @brief Implementation of helper functions for input arguments parsing
 */

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "arg-parser.h"

void stripExtension(char* fileName) {
    assert(fileName != nullptr);

    char* end = fileName + strlen(fileName);

    while ((end > fileName) && (*end != '.') && (*end != '\\') && (*end != '/')) {
        --end;
    }

    if ((end > fileName) && (*end == '.') && (*(end - 1) != '\\') && (*(end - 1) != '/')) {
        *end = '\0';
    }
}

void replaceExtension(char* destination, const char* originalFileName, const char* newExtension) {
    assert(originalFileName != nullptr);
    assert(newExtension != nullptr);

    char tmp[maxFileNameLength];
    strcpy(tmp, originalFileName);
    stripExtension(tmp);
    strcpy(destination, strcat(tmp, newExtension));
}

arguments parseArgs(int argc, char* argv[], RunningMode runningMode) {
    assert(argv != nullptr);

    arguments args { "", "" };
    if (argc >= 2) strcpy(args.inputFile, argv[1]);
    if (argc >= 3) strcpy(args.outputFile, argv[2]);

    if (argc > 3) fprintf(stderr, "Only 2 arguments required. Other are ignored");

    if (strlen(args.outputFile) == 0) {
        switch (runningMode) {
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
                fprintf(stderr, "Invalid running mode");
                exit(-1);
        }
    }
    return args;
}
