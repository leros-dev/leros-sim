
#include "testmacro.h"

int main(int argc, char** argv){
    int a0 = ARG(0); 
    unsigned char A[a0];
    unsigned char B[a0];
    unsigned char C[a0];
    unsigned char s = 0;
    for(int i = 0; i < a0; i++){
        A[i] = i;
        B[i] = i - 1;
    }

    for(int i = 0; i < a0; i++){
        C[i] = A[i] + B[i];
    }
    
    for(int i = 0; i < a0; i++){
        s += C[i];
    }
    TEST_RETURN(s);
}
