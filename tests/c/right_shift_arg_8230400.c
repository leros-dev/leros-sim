#include "testmacro.h"

int main(int argc, char** argv){
    int a0 = ARG(0); 
    int value = 8230400 >> a0;
	TEST_RETURN(value);
}
