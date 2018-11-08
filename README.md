# leros-sim
Simulator and verification suite for the Leros ISA

## Verification suite
The `simdriver.py` python script is used for implementing a compiler- and simulator verification suite. 
The purpose of the verification suite is to have a set of C language tests which are executed on both the host system and the Leros simulator. Using the syntax described below, a single C source file can be specified, which will be compiled and run on both systems.

*todo*: write about the simdriver.py  

<p align="center">
  <img src="https://github.com/mortbopet/leros-sim/blob/master/resources/simdriver.png">
</p>

Currently, `simdrivertests.txt` contains the current test suite, containing test cases for the tests in `tests/c/`.

## Adding tests



An example of a simple test could be:
```c++
#define ACNT2
#include "testmacro.h"

TEST_START{
    int result = ARG(0) + ARG(1);
    TEST_END(result);
}
```
In defining a test, we utilize the following macros:
* ACNT{N}: Defines the number of arguments which the test expects. This has an effect on the emitted code for `TEST_START` when compiling for Leros.
* TEST_START: Depending on whether the compiler variable `LEROS_HOST_TEST` is set, either emits a `main` function for host tests, or a function which is not defined as being a `main` function, for Leros simulator execution. 
* ARG(N): Fetches the input argument specified by N. On host, this is fetched from `argv` using `atoi`. On Leros tests, this references the actual argument parameter, ie. `arg2`. 
* TEST_END(v): on Leros, emits `return v`. On host, a `printf("%d",v)` is emitted before the return (used by the `simdriver.py` script for fetching the return value), and returns 0.
For more information on the macros, refer to [`testmacro.h`](https://github.com/mortbopet/leros-sim/blob/master/tests/c/testmacro.h). 
