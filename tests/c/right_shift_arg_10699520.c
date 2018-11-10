#include "testmacro.h"

int main(int argc, char** argv){
    int a0 = ARG(0); 
    int value = 10699520 >> a0;
	TEST_RETURN(value);
}
