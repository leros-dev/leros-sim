#include "testmacro.h"

int foo() {
  volatile int a = 0;
  a++;
  return a;
}

int main(int argc, char **argv) {
  int boo = foo();
  TEST_RETURN(boo);
}
