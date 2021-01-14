/**
 * @file
 */
#include "arg-parser.h"
#include "stack-machine.h"
#include "stack-machine-utils.h"

int main(int argc, char* argv[]) {
    arguments args = parseArgs(argc, argv, RUN);
    int exitCode = run(args.inputFile);
    printErrorMessageForExitCode(exitCode);
    return exitCode;
}
