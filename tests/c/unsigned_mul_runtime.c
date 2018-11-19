#include "testmacro.h"

int main(int argc, char** argv){
    unsigned int a0 = ARG(0);
    unsigned int a1 = ARG(1);

    unsigned int x = a0 * a1;
    TEST_RETURN(x);
} 
