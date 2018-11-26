
#include "testmacro.h"

int main(int argc, char** argv){
    char a0 = ARG(0);
    char a1 = ARG(1);
    char a2 = ARG(2); 
    char v1 = a0 & 0b11111111;
	char v2 = a1 | 0b11111111;
	char v3 = a2 ^ 0b11111111;
	char v4 = v1 + v2 + v3;
TEST_RETURN(v4);}

