// init: The initial user-level program

#include "types.h"
#include "fs/stat.h"
#include "atomic/spinlock.h"
#include "atomic/sleeplock.h"
#include "fs/fs.h"
#include "fs/file.h"
#include "user.h"
#include "fs/fcntl.h"
#include "debug.h"

char *argv[] = { "sh", 0 };

int
main(void)
{
  // for(;;);
  int pid, wpid;

  

  // printf(red("init run!\n"));

  if(open("console", O_RDWR) < 0){
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0);  // stdout
  dup(0);  // stderr

  for(;;){
    printf(yellow("init: starting sh\n"));
    pid = fork();
    if(pid < 0){
      printf("init: fork failed\n");
      exit(1);
    }
    if(pid == 0){
      // for(;;);
      exec("sh", argv);
      // for(;;);
      printf("init: exec sh failed\n");
      exit(1);
    }

    for(;;){
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((int *) 0);
      if(wpid == pid){
        // the shell exited; restart it.
        printf(green("shell exit!!\n"));
        for(;;);
        break;
      } else if(wpid < 0){
        printf("init: wait returned an error\n");
        exit(1);
      } else {
        // it was a parentless process; do nothing.
      }
    }
  }
}
