#include "testmacro.h"

TEST_START{
    int A[ARG];
    int B[ARG];
    int C[ARG];
    int s = 0;
    for(int i = 0; i < ARG; i++){
        A[i] = i;
        B[i] = i - 1;
    }

    for(int i = 0; i < ARG; i++){
        C[i] = A[i] + B[i];
    }
    
    for(int i = 0; i < ARG; i++){
        s += C[i];
    }
    TEST_END(s);
}