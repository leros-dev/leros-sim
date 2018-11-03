#define ACNT1
#include "testmacro.h"

TEST_START{
    int value = ARG(0) << 26;
    TEST_END(value);
}
