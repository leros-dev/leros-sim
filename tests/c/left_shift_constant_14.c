#define ACNT1
#include "testmacro.h"

TEST_START
    int value = ARG(0) << 14;
TEST_END(value)

