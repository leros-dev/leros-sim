#include "testmacro.h"

TEST_START{
    int value = 12345600 << ARG;
    TEST_END(value);
}
