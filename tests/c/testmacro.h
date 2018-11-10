#ifdef LEROS_HOST_TEST

#include <iostream>
#include <string>
#define ARG(n) std::stoi(argv[n + 1])

#else

#define ARG(n) *((int *)(argv) + n)

#endif