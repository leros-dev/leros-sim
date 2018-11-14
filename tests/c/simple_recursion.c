#include "testmacro.h"

int foo(int a){
    if(a > 1)
       foo(a-1); 
    return a;
}

int main(int argc, char** argv){
    TEST_RETURN(foo(ARG(0)));
} 
