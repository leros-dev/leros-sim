#define ARG_PREFIX _A

#ifdef LEROS_HOST_TEST
    #include <stdio.h>
    #include <string>

    #define TEST_START int main(int argc, char** argv){

    #define TEST_END(retval) \
        printf("%d", retval); \
        return 0; \
        }


    #define ARG(i) std::stoi(argv[i+1])

#else   

    #define  TEST_END(retval)    \
            return retval;       \
            }

    // We assume that argv for leros points to a position in memory where the arguments are passed
    // as integers. This is mostly for the test - the Leros simulator will parse the input values as 
    // integers and place them accordingly in memory.


    #define ARG(i) ARG_PREFIX##i

    #ifdef ACNT1
        #define TEST_START \
        int main(int argc, char** argv){        \
            int ARG_PREFIX##0 = *((int*)(argv));    
            
    #elif defined(ACNT2)
        #define TEST_START \
        int main(int argc, char** argv){        \    
            int ARG_PREFIX##0 = *((int*)(argv));    \
            int ARG_PREFIX##1 = *((int*)(argv) + 1);

    #elif defined(ACNT3)
        #define TEST_START \
        int main(int argc, char** argv){        \    
            int ARG_PREFIX##0 = *((int*)(argv));    \
            int ARG_PREFIX##1 = *((int*)(argv) + 1);\
            int ARG_PREFIX##2 = *((int*)(argv) + 2);

    #elif defined(ACNT4)
        #define TEST_START \
        int main(int argc, char** argv){        \    
            int ARG_PREFIX##0 = *((int*)(argv));    \
            int ARG_PREFIX##1 = *((int*)(argv) + 1);\
            int ARG_PREFIX##2 = *((int*)(argv) + 2);\
            int ARG_PREFIX##3 = *((int*)(argv) + 3);
    #endif
    


#endif