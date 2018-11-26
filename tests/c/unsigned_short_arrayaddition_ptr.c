
#include "testmacro.h"

void ArrayAdd(unsigned short* C, unsigned short* A, unsigned short* B, int size){
    for(int i = 0; i < size; i++){
        *C = *A + *B;
        C++; A++; B++;
    }
}

int main(int argc, char** argv){
    int a0 = ARG(0); 
    unsigned short A[a0];
    unsigned short B[a0];
    unsigned short C[a0];
    unsigned short s = 0;
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
