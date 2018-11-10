
#include "testmacro.h"

int main(int argc, char** argv){
    int a0 = ARG(0);
    int a1 = ARG(1);
    int a2 = ARG(2); 
    unsigned v1 = a0 & 0b111111111111111;
	unsigned v2 = a1 | 0b111111111111111;
	unsigned v3 = a2 ^ 0b111111111111111;
	unsigned v4 = v1 + v2 + v3;
TEST_RETURN(v4);}

