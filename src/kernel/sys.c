#include "kernel/sys.h"
#include "kernel/time.h"
#include "defs.h"


utsname_t sysname = {
  .domainname = "tatak.os",
  .machine = "xxxk210xxx",
  .nodename = "wtf",
  .release = "no release",
  .version = "0.0.0.0.0.0.0.0.1"
};


uint64 sys_timetag(void) {
    return ticks;
}


uint64 sys_ktest(void) {
  // sbi_putchar('H');
  // sbi_putchar('E');
  // sbi_putchar('L');
  // sbi_putchar('L');
  // sbi_putchar('O');
  // sbi_putchar('W');
  // sbi_putchar(' ');
  // sbi_putchar('W');
  // sbi_putchar('O');
  // sbi_putchar('R');
  // sbi_putchar('L');
  // sbi_putchar('D');
  // sbi_putchar('\n');
  return 0;
}


uint64_t sys_times(void) {
  uint64_t addr;
  uint64_t ret = 0;

  if(argaddr(0, &addr) < 0) 
    return -1;
  
  acquire(&tickslock);
  if(copyout(myproc()->pagetable, addr, (char *)&ticks, sizeof(ticks)) == -1)  
    ret = -1;
  release(&tickslock);

  return ret;
}

uint64_t sys_uname(void) {
  uint64_t addr;

  if(argaddr(0, &addr) < 0) {
    return -1;
  }

  return copyout(myproc()->pagetable, addr, (char *)&sysname, sizeof(utsname_t));
}
uint64_t sys_gettimeofday(void) {
  timespec_t time;
  uint64_t addr;
  uint64_t ret = 0;

  if(argaddr(0, &addr) < 0) 
    return -1;

  acquire(&tickslock);
  time = TICK2TIMESPEC(ticks);
  release(&tickslock);

  if(copyout(myproc()->pagetable, addr, (char *)&time, sizeof(time)) == -1) {
    ret = -1;
  } 
  printf("ret is %d\n", ret);
  return ret;
}