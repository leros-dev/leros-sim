#define ACNT3
#include "testmacro.h"

TEST_START 
    /* Move ARG(0)'th to rightmost side */
    unsigned int bit1 =  (ARG(0) >> ARG(1)) & 1; 
  
    /* Move ARG(2)'th to rightmost side */
    unsigned int bit2 =  (ARG(0) >> ARG(2)) & 1; 
  
    /* XOR the two bits */
    unsigned int x = (bit1 ^ bit2); 
TEST_END(x)