#include "testmacro.h"

TEST_START{
    int s = 0;
    for(int i = 1; i <= ARG; i++){
        s += i; 
    }
    TEST_END(s);
}