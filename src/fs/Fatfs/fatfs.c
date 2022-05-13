/**
 * @file syscall.c
 * @author DavidZyy (1929772352@qq.com)
 * @brief this file use "f_xxx" function to receive arguments from
 * "sys_xxx" function. 
 * @version 0.1
 * @date 2022-05-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "fs/ff.h"
#include "fs/diskio.h"
#include "fs/ffconf.h"
#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "mm/vm.h"
#include "param.h"
#include "fs/stat.h"
#include "fs/fs.h"
#include "atomic/spinlock.h"
#include "kernel/proc.h"
#include "atomic/sleeplock.h"
#include "fs/file.h"
#include "fs/fcntl.h"
#include "fs/fatfs.h"
#include "debug.h"

int
fatfs_init(void){
    static FATFS sdcard_fs;
    FRESULT status;
    printf(green("/********************fatfs init*******************/\n"));
    // f_init();
    status = f_mount(&sdcard_fs, _T("/"), 1);
    printf("mount sdcard:%d\n", status);
    if (status != FR_OK)
        panic("fatfs_init!");
    return 0;
}

int fs_test(void)
{
    static FATFS sdcard_fs;
    FRESULT status;
    // DIR dj;
    // FILINFO fno;

    printf(green("/********************fs test*******************/\n"));
    // f_init();
    status = f_mount(&sdcard_fs, _T("/"), 1);
    printf("mount sdcard:%d\n", status);
    if (status != FR_OK)
        return status;

    // printf("printf filename\n");
    // status = f_findfirst(&dj, &fno, _T("/"), _T("*"));
    // // printf("\e[31m%d\e[0m\n", fno.fname[0]);
    // while (status == FR_OK && fno.fname[0]) {
    //     if (fno.fattrib & AM_DIR)
    //         printf("dir:%s\n", fno.fname);
    //     else
    //         printf("file:%s\n", fno.fname);
    //     status = f_findnext(&dj, &fno);
    // }
    // f_closedir(&dj);
    // f_chdir("dir1");

    // const char *fname = "dir4/file3";
    // f_stat(fname, &fno);
    //     if (fno.fattrib & AM_DIR)
    //         printf("dir:%s\n", fno.fname);
    //     else
    //         // printf("\e[31mfile:%s\n\e[0m", fno.fname);
    //         printf(green("file:%s\n"), fno.fname);
    //     status = f_findnext(&dj, &fno);
    // return 0;
    FIL f;
    f_open(&f, "/dir1/dir4/file3", FA_READ);
    UINT btr = 512, br;
    char buf[512];
    f_read(&f, buf, btr, &br);
    printf(green("%s\n"), buf);

    return 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static int
fdalloc(struct file *f)
{
  int fd;
  struct proc *p = myproc();

  for(fd = 0; fd < NOFILE; fd++){
    if(p->ofile[fd] == 0){
      p->ofile[fd] = f;
      return fd;
    }
  }
  return -1;
}

static int
argfd(int n, int *pfd, struct file **pf)
{
  int fd;
  struct file *f;

  if(argint(n, &fd) < 0)
    return -1;

//   printf(yellow("fd: %d\n"), fd);
  if((f=myproc()->ofile[fd]) == 0)
    panic("argfd 1");
  if(fd < 0 || fd >= NOFILE || (f=myproc()->ofile[fd]) == 0)
    panic("argfd 2");
    // return -1;
  if(pfd)
    *pfd = fd;
  if(pf)
    *pf = f;
  return 0;
}


uint64
sys_open(void){
    char pathname[MAXPATH];
    int flags;
    FILINFO fno;
    FRESULT fr;
    struct file *f;
    int n, fd;

    if((n = argstr(0, pathname, MAXPATH)) < 0 || argint(1, &flags) < 0)
        return -1;

    // printf(red("%s\n"), pathname);
    if(strncmp(pathname, "console", 7) == 0){
    if((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0){
        panic("sys_open 3");
        if(f)
        fileclose(f);
        return -1;
    }
    f->type = FD_DEVICE;

    f->major = CONSOLE;
    // printf(red("fd: %d\n"), fd);
    return fd;
    }

    fr = f_stat(pathname, &fno);    
    if(fr != FR_OK){
        panic("sys_open 1");
        return -1;
    }

    if((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0){
        panic("sys_open 2");
        if(f)
        fileclose(f);
        return -1;
    }

    if(fno.fattrib & AM_DIR){
        f->type = FD_DIR;
        f_opendir(&(f->obj.d), pathname);
    }
    else{
        int fg = 0;
        if(flags & O_CREATE)
            fg |= FA_CREATE_NEW;
        else{
            fg |= FA_READ;
            fg |= FA_WRITE;
        }
        
        f->type = FD_FILE;
        f_open(&(f->obj.f), pathname, fg);
    }

    print_opened_file();
    return fd;
}

// uint64 sys_read(void) {return 0;}
uint64 sys_pipe(void) {return 0;}
uint64 sys_fstat(void) {return 0;}
uint64 sys_chdir(void) {return 0;}
// uint64 sys_dup(void) {return 0;}
// uint64 sys_write(void) {return 0;}
uint64 sys_mknod(void) {return 0;}
uint64 sys_unlink(void) {return 0;}
uint64 sys_link(void) {return 0;}
uint64 sys_mkdir(void) {return 0;}
uint64 sys_close(void) {return 0;}

uint64
sys_exec(void)
{
  char path[MAXPATH], *argv[MAXARG];
  int i;
  uint64 uargv, uarg;

  if(argstr(0, path, MAXPATH) < 0 || argaddr(1, &uargv) < 0){
    return -1;
  }
  memset(argv, 0, sizeof(argv));
  for(i=0;; i++){
    if(i >= NELEM(argv)){
      goto bad;
    }
    if(fetchaddr(uargv+sizeof(uint64)*i, (uint64*)&uarg) < 0){
      goto bad;
    }
    if(uarg == 0){
      argv[i] = 0;
      break;
    }
    argv[i] = kalloc();
    if(argv[i] == 0)
      goto bad;
    if(fetchstr(uarg, argv[i], PGSIZE) < 0)
      goto bad;
  }

  int ret = exec(path, argv);
  for(i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  return ret;

 bad:
  for(i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  return -1;
}

uint64
sys_write(void)
{
    // printf(green("sys_write\n"));
  struct file *f;
  int n;
  uint64 p;

  if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argaddr(1, &p) < 0)
    panic("sys_write");
    // return -1;

//   printf(yellow("sys_write\n"));
  return filewrite(f, p, n);
}

uint64
sys_dup(void)
{
  struct file *f;
  int fd;

  if(argfd(0, 0, &f) < 0)
    return -1;
  if((fd=fdalloc(f)) < 0)
    return -1;
  filedup(f);
  return fd;
}

uint64
sys_read(void)
{
  struct file *f;
  int n;
  uint64 p;

  if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argaddr(1, &p) < 0)
    return -1;
  return fileread(f, p, n);
}