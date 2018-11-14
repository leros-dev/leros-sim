#include "testmacro.h"

int main(int argc, char** argv){
    int a0 = ARG(0);
    /* Move a0'th to rightmost side */
 
    char b = 0;

    for(int i = 0; i < a0; i++){
        b++;
    }

    TEST_RETURN(b);
}