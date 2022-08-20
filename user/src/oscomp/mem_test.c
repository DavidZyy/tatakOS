#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "generated/syscall.h"
#include "syscall.h"
#include "stddef.h"

#define PGSIZE 4096

// void test_mem(){
//     uint64 cur_pos;
//     syscall(NR_memuse);
//     cur_pos = brk(0);
//     while((cur_pos = brk(cur_pos + PGSIZE)) != -1) {
//         *(uint32 *)(cur_pos - PGSIZE) = 0xabcd;
//     }
//     syscall(NR_memuse);
// }

void test_mem(){
    uint64 cur_pos, start_pos;
    syscall(NR_memuse);

    int pid = fork();

    start_pos = cur_pos = brk(0);
    int i = 0;
    while((cur_pos = brk(cur_pos + PGSIZE)) != -1 && i < 1000) {
        i++;
        // syscall(NR_memuse);
        // printf("i = %d\n", i);
        *(uint64 *)(cur_pos - PGSIZE) = cur_pos - PGSIZE;
    }
    syscall(NR_memuse);
    printf("START read Test\n");
    for(uint64 addr = start_pos; addr < start_pos + 2000 * PGSIZE; addr += PGSIZE) {
        uint64 num = *(uint64 *)addr;
        if(num != addr) {
            printf("panic\n");
            for(;;);
        }
        // printf("read from %x, num is %x\n", addr, num);
    }
}

int main(void) {
    test_mem();
    return 0;
}