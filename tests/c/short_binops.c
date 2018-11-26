
#include "testmacro.h"

int main(int argc, char** argv){
    // short a0 = ARG(0);
    // short a1 = ARG(1);
    short a2 = ARG(2); 
    // short v1 = a0 & 0b1111111111111111;
	// short v2 = a1 | 0b1111111111111111;
	short v3 = a2 ^ 0b1011111111111111;
	// short v4 = v1 + v2 + v3;
TEST_RETURN(v3);}

