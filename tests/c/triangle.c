
#include "testmacro.h"

int main(int argc, char** argv){
    int a0 = ARG(0); 
    int s = 0;
    for(int i = 1; i <= a0; i++){
        s += i;
    }
TEST_RETURN(s);}
