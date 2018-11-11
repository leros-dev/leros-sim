#include "testmacro.h"

int foo() {
  volatile int a;
  a++;
  return a;
}

int main(int argc, char **argv) {
  int boo = foo();
  TEST_RETURN(boo);
}
