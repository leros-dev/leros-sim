#define ACNT1
#include "testmacro.h"

TEST_START
    int A[ARG(0)];
    int B[ARG(0)];
    int C[ARG(0)];
    int s = 0;
    for(int i = 0; i < ARG(0); i++){
        A[i] = i;
        B[i] = i - 1;
    }

    for(int i = 0; i < ARG(0); i++){
        C[i] = A[i] + B[i];
    }
    
    for(int i = 0; i < ARG(0); i++){
        s += C[i];
    }
TEST_END(s)
