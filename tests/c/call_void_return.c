#include "testmacro.h"

void foo() {
  volatile int a;
  a++;
  return;
}

int main(int argc, char **argv) {
  foo();
  TEST_RETURN(0);
}
