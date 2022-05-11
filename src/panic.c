#include "printf.h"
#include "utils.h"
#include "debug.h"

volatile int panicked = 0;

void panic(char *s)
{
  printf(red("panic: "));
  printf(s);
  printf("\n");
  printf("backtrace: \n");
  backtrace();
  panicked = 1; // freeze uart output from other CPUs
  for(;;)
    ;
}