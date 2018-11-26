
#include "testmacro.h"

void ArrayAdd(short* C, short* A, short* B, int size){
    for(int i = 0; i < size; i++){
        *C = *A + *B;
        C++; A++; B++;
    }
}

int main(int argc, char** argv){
    int a0 = ARG(0); 
    short A[a0];
    short B[a0];
    short C[a0];
    short s = 0;
    for(int i = 0; i < a0; i++){
        A[i] = i;
        B[i] = i - 1;
    }

    ArrayAdd(C, A, B, a0);

    // Verify by sum
    for(int i = 0; i < a0; i++){
        s += C[i];
    }

    TEST_RETURN(s);
}
