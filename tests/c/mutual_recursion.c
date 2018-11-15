#include "testmacro.h"

// returns 0 if the given number becomes 0, so the given number is odd
// returns even(number - 1) elsewhere

int even(int number);

int odd(int number){
 if (number==0) 
    return 0;
 else
    return even(number-1);
}
 
// returns 0 if the given number becomes 0, so the given number is even
// returns odd(number - 1) elsewhere
int even(int number){
 if(number==0) 
    return 1;
 else
    return odd(number-1);
}

int main(int argc, char** argv){
    TEST_RETURN(odd(ARG(0)));
} 
