#ifdef LEROS_HOST_TEST

#include <iostream>
#include <string>
#define ARG(n) std::stoi(argv[n + 1])
#define TEST_RETURN(val)                                                       \
  printf("%d", val);                                                           \
  return 0;

#else

#define ARG(n) *((int *)(argv) + n)

#define TEST_RETURN(val) return val;

#endif