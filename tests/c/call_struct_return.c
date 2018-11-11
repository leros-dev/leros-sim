#include "testmacro.h"

struct bar
{
    int a;
    char b;
};

struct bar foo()
{
    struct bar b;
    b.a = -2;
    b.b = 4;
    b.a++;
    b.b--;
    return b;
}

int main(int argc, char **argv)
{
    struct bar retval = foo();
    int c = retval.b + retval.a;
    TEST_RETURN(c);
}
