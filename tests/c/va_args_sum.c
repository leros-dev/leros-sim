#include "testmacro.h"
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */

int Sum(int n, ...)
{
  int val,sum;
  va_list vl;
  va_start(vl,n);
  sum=va_arg(vl,int);
  for (;n>1;n--)
  {
    val=va_arg(vl,int);
    sum += val;
  }
  va_end(vl);
  return sum;
}

int main (int argc, char** argv) {
  int sum;
  sum = Sum(7,702,422,631,834,892,104,772);
  
  TEST_RETURN(sum);
}