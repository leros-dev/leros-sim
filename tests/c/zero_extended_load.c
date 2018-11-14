#include "testmacro.h"

int main(int argc, char** argv){
    unsigned char b = ARG(0);
    if(1)
        b++;
    TEST_RETURN(b);
}