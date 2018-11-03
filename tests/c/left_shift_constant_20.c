#define ACNT1
#include "testmacro.h"

TEST_START{
    int value = ARG(0) << 20;
    TEST_END(value);
}
