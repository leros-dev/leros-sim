#include "testmacro.h"

int main(int argc, char** argv){
    int a0 = ARG(0); 
    int value = a0 >> 1;
	TEST_RETURN(value);
}
