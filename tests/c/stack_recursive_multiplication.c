#include "testmacro.h"

int multiply(int lhs, int rhs){
    if(rhs > 1){
        lhs += multiply(lhs, rhs - 1);
    }
    return lhs;
}

int main(int argc, char** argv){
    int a0 = ARG(0);
    int a1 = ARG(1);

    unsigned int x = multiply(a0, a1); 
    TEST_RETURN(x);
} 
