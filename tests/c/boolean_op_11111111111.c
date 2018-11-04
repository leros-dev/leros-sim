#define ACNT3
#include "testmacro.h"

TEST_START{
    unsigned v1 = ARG(0) & 0b11111111111;
	unsigned v2 = ARG(1) | 0b11111111111;
	unsigned v3 = ARG(2) ^ 0b11111111111;
	unsigned v4 = v1 + v2 + v3;
    TEST_END(v4);
}
