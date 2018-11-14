#include "testmacro.h"

void foo() {
  return;
}

int main(int argc, char **argv) {
  foo();
  TEST_RETURN(0);
}
