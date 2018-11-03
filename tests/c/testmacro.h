#ifdef LEROS_HOST_TEST
    #include <stdio.h>
    #include <string>

    #define TEST_START int main(int argc, char** argv)

    #define TEST_END(retval) \
        printf("%d", retval); \
        return 0;


    #define ARG(i) std::stoi(argv[i+1])

#else   
    // Argument count for the Leros function. Should be defined before including this file!
    #ifdef ACNT1
        #define TEST_START int test(int a0)
    #elif defined(ACNT2)
        #define TEST_START int test(int a0, int a1)
    #elif defined(ACNT3)
        #define TEST_START int test(int a0, int a1, int a2)
    #elif defined(ACNT4)
        #define TEST_START int test(int a0, int a1, int a2, int a3)
    #endif

    #define TEST_END(retval) return retval;

    #define ARG(i) a##i

#endif