#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "generated/syscall.h"
#include "syscall.h"
#include "stddef.h"
#include "stdlib.h"

/* 所有引用数大于0的页 */
#define ALL_REFERED_PAGES 1
/* 所有映射数大于0的页 */
#define ALL_MAPPED_PAGES 2

/**
 * argv[0] 是memuse
 */
int main(int argc, char *argv[]){
  // if(*argv[0] == 'r')
  if(*argv[1] == 'r')
    memuse(ALL_REFERED_PAGES);
  else if(*argv[1] == 'm')
  // else if(*argv[0] == 'm')
    memuse(ALL_MAPPED_PAGES);
  else 
    memuse(0);
}