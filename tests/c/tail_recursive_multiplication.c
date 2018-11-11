#include "testmacro.h"

void multiply(int lhs, int* sum, int rhs){
    if(rhs == 0)
        return;
    
    *sum += lhs;
    return multiply(lhs, sum, rhs - 1);;
}

int main(int argc, char** argv){
    int a0 = ARG(0);
    int a1 = ARG(1);

    int sum = 0;
    multiply(a0, &sum, a1); 
    TEST_RETURN(sum);
} 
