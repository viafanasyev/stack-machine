# Stack machine

## Project description

This program is developed as a part of ISP RAS course.  

This program contains a register stack machine. It can assemble, disassemble and run programs. 
See Run section to find available operations and examples.

NOTE: This program works only on UNIX-like OS.

### Structure

* src/ : Main project
    * immortal-stack/ : Error-secure generic stack used in stack machine
        * stack.h : Definition and implementation of error-secure generic stack.
        * logger.h : Definition and implementation of logging functions and macros.
        * environment.h : Helper macros that are environment-dependent (OS, bitness, etc).
    * stack-machine.h, stack-machine.cpp : Simple stack machine implementation with ability to assemble, disassemble and run programs.
    * stack-machine-utils.h, stack-machine-utils.cpp : Helper functions for stack machine. Also contains used opcodes and errors.
    * main.cpp : Entry point for the program.

* test/ : Tests and testing library
    * testlib.h, testlib.cpp : Library for testing with assertions and helper macros.
    * main.cpp : Entry point for tests. Just runs all tests.

* doc/ : doxygen documentation

* Doxyfile : doxygen config file

### Run

#### Stack machine

To run main program execute next commands in terminal:
```shell script
cmake . && make
./stack-machine --help            # To see help
./stack-machine --asm file.txt    # To assemble file with code. Result is put in out.asm
./stack-machine --disasm file.asm # To disassemble asm file. Result is put in out.disasm
./stack-machine --run file.asm    # To run asm file
```

Only one asm, disasm and run command could be used at the time.

##### Available operations

File with code can contain next operations:
```
IN       # Read double value from console and put it on stack
OUT      # Pop value from stack and write it in console
POP      # Pop value from stack
POP AX   # Pop value from stack and put it into register
PUSH 1.5 # Put the given value on top of the stack
PUSH AX  # Put the value from register on top of the stack
ADD      # Pop two values from stack and put the addition result on top of the stack 
SUB      # Pop two values from stack and put the substraction result on top of the stack
MUL      # Pop two values from stack and put the multiplication result on top of the stack
DIV      # Pop two values from stack and put the division result on top of the stack
SQRT     # Pop one value from stack and put the square root on top of the stack
HLT      # Stop the program
```

Program should end with `HLT` command, otherwise it's behaviour is undefined.  
Each command should be on separate line.  
Possible registers: `AX`, `BX`, `CX`, `DX`.  

Example program (square root of sum):
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

If the error occurs while running the assembly file (for example, if the stack is empty and `POP` command is called), 
then this program fails with assertion and exits by SIGABRT 
(see https://github.com/viafanasyev/immortal-stack for list available error messages at `STACK_SECURITY_LEVEL 3`).  

Probably, there will be more advanced error messages in the future.
 

#### Tests

To run tests execute next commands in terminal:
```
cmake . && make
./tests
```

### Documentation

Doxygen is used to create documentation. You can watch it by opening `doc/html/index.html` in browser.  

### OS

Program is developed under Ubuntu 20.04.1 LTS.