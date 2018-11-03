#define ACNT4
#include "testmacro.h"

TEST_START{ 
    /* Move all bits of first set to rightmost side */
    unsigned int set1 =  (ARG(0) >> ARG(1)) & ((1U << ARG(3)) - 1); 
  
    /* Move all bits of second set to rightmost side */
    unsigned int set2 =  (ARG(0) >> ARG(2)) & ((1U << ARG(3)) - 1); 
  
    /* XOR the two sets */
    unsigned int XOR = (set1 ^ set2); 
  
    /* Put the xor bits back to their original positions */
    XOR = (XOR << ARG(1)) | (XOR << ARG(2)); 
  
    /* XOR the 'xor' with the original number so that the  
       two sets are swapped */
    unsigned int result = ARG(0) ^ XOR; 
  
    TEST_END(result)
} 