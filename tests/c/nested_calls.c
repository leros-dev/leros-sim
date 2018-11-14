#include "testmacro.h"

int qux(int a){
    return a + 1;
}

int baz(int a){
    return qux(a + 1);
}

int bar(int a){
    return baz(a + 1);
}

int foo(int a){
    return bar(a + 1);
}

int main(int argc, char** argv){
    TEST_RETURN(foo(ARG(0)));
} 
