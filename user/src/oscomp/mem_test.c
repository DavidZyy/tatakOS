#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "generated/syscall.h"
#include "syscall.h"
#include "stddef.h"

#define PGSIZE 4096

/**
 * swap测试文件。
 */

// void test_mem(){
//     uint64 cur_pos;
//     syscall(NR_memuse);
//     cur_pos = brk(0);
//     while((cur_pos = brk(cur_pos + PGSIZE)) != -1) {
//         *(uint32 *)(cur_pos - PGSIZE) = 0xabcd;
//     }
//     syscall(NR_memuse);
// }

#define PAGES 500 
#define TWO_PROCESS

void test_mem(){
    uint64 cur_pos, start_pos;
    syscall(NR_memuse);

    // printf("START fork\n");
#ifdef TWO_PROCESS
    int pid = fork();
#endif

    start_pos = cur_pos = brk(0);
    int i = 0;

    /*swap out pages*/
    while((cur_pos = brk(cur_pos + PGSIZE)) != -1 && i < PAGES) {
        i++;
        // syscall(NR_memuse);
        // printf("i = %d\n", i);
        *(uint64 *)(cur_pos - PGSIZE) = cur_pos - PGSIZE;
    }
    syscall(NR_memuse);
    printf("START read Test\n");

    /*swap in pages*/
    for(uint64 addr = start_pos; addr < start_pos + PAGES * PGSIZE; addr += PGSIZE) {
        uint64 num = *(uint64 *)addr;
        if(num != addr) {
#ifdef TWO_PROCESS
            if(pid == 0){
                printf(rd("child: panic\n"));
                // printf(rd("child: panic, num: %d, addr: %d\n"), num, addr);
            }
            else if(pid > 0){
                printf(rd("parent: panic\n"));
                // printf(rd("parent: panic, num: %d, addr: %d\n"), num, addr);

            }
            else 
                printf(rd("fork failed\n"));
#else
            printf(rd("panic!\n"));
#endif
            printf(rd("panic!\n"));
            for(;;);
        }
#ifdef TWO_PROCESS
        if(pid == 0)
            printf(bl("read from %x, num is %x\n"), addr, num);
        else if(pid > 0)
            printf(ylw("read from %x, num is %x\n"), addr, num);
#elif
        printf(ylw("read from %x, num is %x\n", addr, num));
#endif
    }
    printf(grn("OOOOOOOOOOOOKKKKKKKKKKKKKKKKK\n"));
}

int main(void) {
    test_mem();
    return 0;
}