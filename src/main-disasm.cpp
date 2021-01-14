/**
 * @file
 */
#include "arg-parser.h"
#include "stack-machine.h"
#include "stack-machine-utils.h"

int main(int argc, char* argv[]) {
    arguments args = parseArgs(argc, argv, DISASM);
    int exitCode = disassemble(args.inputFile, args.outputFile);
    printErrorMessageForExitCode(exitCode);
    return exitCode;
}
