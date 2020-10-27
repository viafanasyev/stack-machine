# Stack machine

## Project description

This program is developed as a part of ISP RAS course.  

This program contains a simple stack machine.

### Structure

* src/ : Main project
    * immortal-stack/ : Error-secure generic stack used in stack machine
        * stack.h : Definition and implementation of error-secure generic stack.
        * logger.h : Definition and implementation of logging functions and macros.
        * environment.h : Helper macros that are environment-dependent (OS, bitness, etc).
    * main.cpp : Entry point for the program.

* test/ : Tests and testing library
    * testlib.h, testlib.cpp : Library for testing with assertions and helper macros.
    * main.cpp : Entry point for tests. Just runs all tests.

* doc/ : doxygen documentation

* Doxyfile : doxygen config file

### Run

#### Stack machine

To run main program execute next commands in terminal:
```
cmake . && make
./stack-machine
```

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