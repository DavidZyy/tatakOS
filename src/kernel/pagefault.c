#include "common.h"
#include "riscv.h"
#include "mm/alloc.h"
#include "mm/vm.h"
#include "kernel/proc.h"
#include "defs.h"

/**
 * @return -1 if exception unhandled else 0
*/
int handle_pagefault(uint64_t scause) {
    proc_t *p = myproc();
    uint64_t va = read_csr(stval);

    // illegal address
    if(va >= p->sz) 
        goto bad;
    
    // store page fault
    if(scause == EXCP_STORE_PAGE_FAULT) {
        // cow
        if(cow_copy(p->pagetable, va, NULL) == -1)
            goto bad;
        return 0;
    }
    return -1;

    bad:
    printf("handle pagefault\n");
    p->killed = 1;
    return 0;
}