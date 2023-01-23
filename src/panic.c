#include "printf.h"
#include "utils.h"
#include "debug.h"

volatile int panicked = 0;

void panic(char *s)
{
  printf(rd("panic: "));
  printf(rd("%s"), s);
  printf("\n");
  // printf("backtrace: \n");
  /*panic 后需要键盘中断查看状态，故关闭*/
  // panicked = 1; // freeze uart output from other CPUs
  for(;;)
    ;
}