test = """#include "testmacro.h"

int main(int argc, char** argv){
    int a0 = ARG(0);
	char res = a0 %s 0;
	TEST_RETURN(res);
}
"""



i = 0

arr = []
arr.append((">","gt"))
arr.append((">=","gte"))
arr.append(("==","eq"))
arr.append(("!=","neq"))
arr.append(("<","lt"))
arr.append(("<=","lte"))

for (lhs, rhs) in arr:
	testname = "signed_set_zero_%s.c" % rhs
	text_file = open(testname, "w")
	text_file.write(test % lhs)
	text_file.close()

	print("tests/c/%s;-2;2;1" % testname)
