# leros-sim
Simulator and verification suite for the Leros ISA

## Verification suite
The `simdriver.py` python script is used for implementing a compiler- and simulator verification suite. 
The purpose of the verification suite is to have a set of C language tests which are executed on both the host system and the Leros simulator. Using the syntax described below, a single C source file can be specified, which will be compiled and run on both systems.

<p align="center">
  <img width=500px src="https://github.com/mortbopet/leros-sim/blob/master/resources/simdriver.png">
</p>

Currently, `simdrivertests.txt` contains the current test suite, containing test cases for the tests in `tests/c/`.

## Usage
Initially, the `leros-sim` must be built. The simulator is capable of running either leros .object files which contains an entry point (`_start`) routine, or flat binary files. Currently, the object file support only loads the .text segment, and thus, programs containing static data or other segments will not be loaded correctly.
Execute `build.sh` in the `leros-sim` repository to build the simulator.  
Next, the Leros compiler must be available - check out https://github.com/leros-dev/leros-llvm and build it using the `build.sh` script in the root folder of the repository.  


To run all of the tests specified in the `simdrivertests.txt` file, execute the `simdriver.py`. The script expects three arguments:
* `--llp`: LLVM Path, path to the `bin/` folder of the Leros compiler tools, ie. `--llp ~/leros-clang/bin`
* `--sim`: Path to executable of the Leros simulator (`leros-sim`), ie. `--sim ~/leros-sim/leros-sim`
* `--test`: Path to the test suite specification file, ie. `--test ~/leros-sim/simdrivertests.txt`

Given these input arguments, the script will begin execution of all tests located in the test suite specification file:  
`python simdriver.py --llp="..." --sim="..." --test="..."`

## Adding tests
An example of a simple test could be:
```c++
#include "testmacro.h"

int main(int argc, char** argv){
    int a0 = ARG(0);
    int a1 = ARG(1);
    
    int res = a0 + a1;

    TEST_RETURN(res);
}
```
In defining a test, we utilize the following macros:
* ARG(N): Fetches the input argument specified by N. On host, this is fetched from `argv` using `atoi`. On Leros tests, we parse an argument string to the simulator, which the simulator translates to integers and inserts into its memory. `argv` is then reinterpreted as `(int*)argv` and we load the arguments through this pointer.
* TEST_RETURN(res): on Leros, emits `return res`. On host, a `printf("%d",res)` is emitted before the return (used by the `simdriver.py` script for fetching the return value), and returns 0.
For more information on the macros, refer to [`testmacro.h`](https://github.com/mortbopet/leros-sim/blob/master/tests/c/testmacro.h). 

## Todo
* Do multithreaded testing
