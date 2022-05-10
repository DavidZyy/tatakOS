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


int fs_test(void)
{
    static FATFS sdcard_fs;
    FRESULT status;
    DIR dj;
    FILINFO fno;

    printf("/********************fs test*******************/\n");
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
    f_chdir("dir1");

    const char *fname = "dir4/file3";
    f_stat(fname, &fno);
        if (fno.fattrib & AM_DIR)
            printf("dir:%s\n", fno.fname);
        else
            printf("file:%s\n", fno.fname);
        status = f_findnext(&dj, &fno);
    return 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
// static int
// fdalloc(struct file *f)
// {
//   int fd;
//   struct proc *p = myproc();

//   for(fd = 0; fd < NOFILE; fd++){
//     if(p->ofile[fd] == 0){
//       p->ofile[fd] = f;
//       return fd;
//     }
//   }
//   return -1;
// }


// uint64
// sys_open(void){
//     char pathname[MAXPATH];
//     int flags;
//     FILINFO fno;
//     FRESULT fr;
//     struct file *f;
//     int n, fd;

//     if((n = argstr(0, pathname, MAXPATH)) < 0 || argint(1, &flags) < 0)
//         return -1;

//     fr = f_stat(pathname, &fno);    
//     if(fr != FR_OK){
//         panic("sys_open 1");
//         return -1;
//     }

//     if((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0){
//         panic("sys_open 2");
//         if(f)
//         fileclose(f);
//         return -1;
//     }

//     if(fno.fattrib & AM_DIR){
//         f->type = FD_DIR;
//         f_opendir(&(f->obj.d), pathname);
//     }
//     else{
//         int fg = 0;
//         if(flags & O_CREATE)
//             fg |= FA_CREATE_NEW;
//         else{
//             fg |= FA_READ;
//             fg |= FA_WRITE;
//         }
        
//         f->type = FD_FILE;
//         f_open(&(f->obj.f), pathname, fg);
//     }

//     print_opened_file();
//     return fd;
// }