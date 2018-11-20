#include "testmacro.h"

int main(int argc, char** argv){
    unsigned int a0 = ARG(0);
	char res = a0 == 0;
	TEST_RETURN(res);
}
