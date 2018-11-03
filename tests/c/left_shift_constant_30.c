#include "testmacro.h"

TEST_START{
    int value = ARG << 30;
    TEST_END(value);
}
