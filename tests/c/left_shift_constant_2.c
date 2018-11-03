#define ACNT1
#include "testmacro.h"

TEST_START{
    int value = ARG(0) << 2;
    TEST_END(value);
}
