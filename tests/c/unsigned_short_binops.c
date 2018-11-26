
#include "testmacro.h"

int main(int argc, char** argv) {
    unsigned short a0 = ARG(0);
    unsigned short a1 = ARG(1);
    unsigned short a2 = ARG(2); 
    unsigned short v1 = a0 & 0b1111111111111111;
	unsigned short v2 = a1 | 0b1111111111111111;
	unsigned short v3 = a2 ^ 0b1111111111111111;
	unsigned short v4 = v1 + v2 + v3;
    TEST_RETURN(v4);
}

