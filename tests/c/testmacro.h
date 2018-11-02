#ifdef LEROS_HOST_TEST
    #include <stdio.h>
    #include <string>

    #define TEST_START int main(int argc, char** argv)

    #define TEST_END(retval) \
        printf("%d", retval); \
        return 0;


    #define ARG std::stoi(argv[1])

#else   
    #define TEST_START int test(int arg)
    #define TEST_END(retval) return retval;
    #define ARG arg
#endif