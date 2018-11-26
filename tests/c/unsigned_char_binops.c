
#include "testmacro.h"

int main(int argc, char** argv){
    unsigned char a0 = ARG(0);
    unsigned char a1 = ARG(1);
    unsigned char a2 = ARG(2); 
    unsigned char v1 = a0 & 0b11111111;
	unsigned char v2 = a1 | 0b11111111;
	unsigned char v3 = a2 ^ 0b11111111;
	unsigned char v4 = v1 + v2 + v3;
TEST_RETURN(v4);}

