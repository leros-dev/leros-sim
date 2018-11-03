#define ACNT1
#include "testmacro.h"

TEST_START{
    int value = 823040 << ARG(0);
    TEST_END(value);
}
