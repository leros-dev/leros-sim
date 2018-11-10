
#include "testmacro.h"

int main(int argc, char** argv){
    int a0 = ARG(0);
    int a1 = ARG(1);
    int a2 = ARG(2);  
    /* Move a0'th to rightmost side */
    unsigned int bit1 =  a0 >> a1 & 1;
  
    /* Move a2'th to rightmost side */
    unsigned int bit2 =  a0 >> a2 & 1;
  
    /* Equate the two bits */
    unsigned int x = (bit1  == bit2); 
TEST_RETURN(x);} 
