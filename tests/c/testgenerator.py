test = """#define ACNT3
#include "testmacro.h"

TEST_START{
    unsigned v1 = ARG(0) & 0b%s;
	unsigned v2 = ARG(1) | 0b%s;
	unsigned v3 = ARG(2) ^ 0b%s;
	unsigned v4 = v1 + v2 + v3;
    TEST_END(v4);
}
"""
i = 0

for i in range(1,32):
	testname = "boolean_op_%s.c"
	boolString = ""
	for j in range(0, i):
		boolString += "1"
	text_file = open(testname % boolString, "w")
	text_file.write(test % (boolString, boolString, boolString))
	text_file.close()

	print("boolean_op_%s.c;0;12345678;1234567;0;12345678;1234567;0;12345678;1234567" % boolString)
