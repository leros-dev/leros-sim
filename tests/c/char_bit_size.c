#include "testmacro.h"

#include <limits.h>

int main(int argc, char** argv){
    const unsigned n_uword_bits = sizeof(int) * CHAR_BIT;
    TEST_RETURN(n_uword_bits);
} 
