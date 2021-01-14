# Stack machine

## Project description

This program is developed as a part of ISP RAS course.  

This program contains a register stack machine with RAM. It can assemble, disassemble and run programs. 
See Run section to find available operations and examples.

NOTE: This program works only on UNIX-like OS.

### Structure

* src/ : Main project
    * arg-parser.h, arg-parser.cpp : Helper functions for input arguments parsing
    * immortal-stack/ : Error-secure generic stack used in stack machine
        * stack.h : Definition and implementation of error-secure generic stack.
        * logger.h : Definition and implementation of logging functions and macros.
        * environment.h : Helper macros that are environment-dependent (OS, bitness, etc).
    * stack-machine.h, stack-machine.cpp : Simple stack machine implementation with ability to assemble, disassemble and run programs.
    * stack-machine-utils.h, stack-machine-utils.cpp : Helper functions for stack machine. Also contains used opcodes and errors.
    * main-asm.cpp    : Entry point for the assembler.
    * main-disasm.cpp : Entry point for the disassembler.
    * main-run.cpp    : Entry point for the stack machine.

* test/ : Tests and testing library
    * testlib.h, testlib.cpp : Library for testing with assertions and helper macros.
    * stack-machine-tests.cpp : Tests for stack machine.
    * main.cpp : Entry point for tests. Just runs all tests.

* examples/ : Files with code of examples given below

* doc/ : doxygen documentation

* Doxyfile : doxygen config file

### Run

#### Assembler

To run assembler execute next commands in terminal:
```shell script
cmake . && make
./asm file.txt               # To assemble file.txt. Result is put in file.asm
./asm file1.txt file2.asm    # To assemble file1.txt. Result is put in file2.asm
```

#### Disassembler

To run disassembler execute next commands in terminal:
```shell script
cmake . && make
./disasm file.asm            # To disassemble file.asm. Result is put in file__disassembly.txt
./disasm file1.asm file2.txt # To disassemble file1.asm. Result is put in file2.txt
```

#### Stack machine

To run stack machine execute next commands in terminal:
```shell script
cmake . && make
./run file.asm               # To run file.asm
```

##### Available operations

Assembly file can contain next operations:
```
IN          # Read double value from console and put it on stack
OUT         # Pop value from stack and write it in console
POP         # Pop value from stack
POP AX      # Pop value from stack and put it into register
POP [1]     # Pop value from stack and put it into given RAM address
POP [AX]    # Pop value from stack and put it into RAM address located in register
PUSH 1.5    # Put the given value on top of the stack
PUSH AX     # Put the value from register on top of the stack
PUSH [1]    # Put the value from RAM (at the given address) on top of the stack
PUSH [AX]   # Put the value from RAM (at the address located in register) on top of the stack
ADD         # Pop two values from stack and put (lhs + rhs) on top of the stack
SUB         # Pop two values from stack and put (lhs - rhs) on top of the stack
MUL         # Pop two values from stack and put (lhs * rhs) on top of the stack
DIV         # Pop two values from stack and put (lhs / rhs) on top of the stack
SQRT        # Pop one value from stack and put the square root on top of the stack
DUP         # Duplicates value on top of the stack
JMP LABEL   # Unconditional jump to the given label
JMPE LABEL  # Pop two values from the stack and jump to the given label if (lhs == rhs) (compared using 1e-9 epsilon)
JMPNE LABEL # Pop two values from the stack and jump to the given label if (lhs != rhs) (compared using 1e-9 epsilon)
JMPL LABEL  # Pop two values from the stack and jump to the given label if (lhs <  rhs)
JMPLE LABEL # Pop two values from the stack and jump to the given label if (lhs <= rhs)
JMPG LABEL  # Pop two values from the stack and jump to the given label if (lhs >  rhs)
JMPGE LABEL # Pop two values from the stack and jump to the given label if (lhs >= rhs)
CALL LABEL  # Put return address (PC of the command after this operation) on call stack and jump to the given label
RET         # Pop return address from call stack and move PC to that address
HLT         # Stop the program

* rhs - value on top of the stack, lhs - value under rhs
```

Program should end with `HLT` command, otherwise it's behaviour is undefined.  

Each command should be on separate line.  

Possible registers: `AX`, `BX`, `CX`, `DX`.  

Labels have to be written on separate lines, can not contain spaces and must end with `:` symbol. Example: `START:` (see more examples below).

##### Examples

Example program 1 (square root of sum):
```
IN
IN
ADD
SQRT
OUT
HLT
```

Example program 2 (square root of the squared value minus 2):
```
IN
POP AX
PUSH AX
PUSH AX
MUL
PUSH 2
SUB
SQRT
OUT
HLT
```

Example program 3 (infinite cycle: read number and print it's square root):
```
START:     <-- Label should be on separate line
IN
SQRT
OUT
JMP START
HLT        <-- Not necessary
```

Example program 4 (call-ret demonstration):
```
IN
IN
CALL DOUBLE_SUM
OUT
HLT

DOUBLE_SUM:
ADD
DUP
ADD
RET
```

Example program 5 (RAM access demonstration):
```
    CALL INIT       <-- Init array with zeros
LOOP_START:
    IN              <-- Read index of an array
    POP AX
    PUSH AX
    PUSH 0
    JMPL LOOP_END   <--| If index exceeds an array size (< 0 or >= 5) - exit from program
    PUSH AX            |
    PUSH 5             |
    JMPGE LOOP_END  <--|
    IN              <-- Read new value for array element
    POP BX
    CALL SET        <-- Set new value
    CALL PRINT      <-- Print array
    JMP LOOP_START
LOOP_END:
    HLT

SET:
    PUSH BX
    POP [AX]
    RET

INIT:
    PUSH 0
    POP [0]
    PUSH 0
    POP [1]
    PUSH 0
    POP [2]
    PUSH 0
    POP [3]
    PUSH 0
    POP [4]
    RET

PRINT:
    PUSH [0]
    OUT
    PUSH [1]
    OUT
    PUSH [2]
    OUT
    PUSH [3]
    OUT
    PUSH [4]
    OUT
    RET
```

All examples can be found in `examples` directory.

#### Tests

To run tests execute next commands in terminal:
```shell script
cmake . && make
./tests
```

### Documentation

Doxygen is used to create documentation. You can watch it by opening `doc/html/index.html` in browser.  

### OS

Program is developed under Ubuntu 20.04.1 LTS.