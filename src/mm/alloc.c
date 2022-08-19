#include "printf.h"
#include "defs.h"
#include "mm/freelist.h"
#include "mm/alloc.h"
#include "mm/buddy.h"
#include "mm/slob.h"
#include "param.h"
#include "utils.h"

#define __MODULE_NAME__ ALLOC
#include "debug.h"
#include "pagevec.h"

#define JUNK 1

extern char end[];

void kinit(void) {
    page_init();
    buddy_init();
    slob_init();

    debug("init success");
}

extern uint64_t use_reserved_flag;

#define TestSetUseReserved test_and_set_bit(0, &use_reserved_flag)
#define TestClearUseReserved test_and_clear_bit(0, &use_reserved_flag)

// extern void free_more_memory(void);


// /**
//  *  内存存在优先级：内核数据结构 > 匿名页(SWAP) > pagecahce(FILE)
//  *  当内核数据结构内存不足时，应从低优先级回收
//  */
// void *__kmalloc(size_t sz, int flag) {
//     void *ret;

//     if(flag & AF_USER) {
//         if(sz != PGSIZE)
//             ER();
//     } else {
//         // 
//         int flag = 0;
//       retry:
//         ret = buddy_alloc(sz);
//         if(ret == NULL) {
//             if(flag) ERROR("out of mem");
//             // 没有页了，调用free_more_memory释放更多的内存，返回之后，goto重新分配。还不行则使用oom killer
//             buddy_print_free();
//             // 当sz大于PGSIZE时，free_more_memory的策略可能需要调整，不然连续的释放可能凑不齐相连的页面
//             free_more_memory();
//             buddy_print_free();
//             flag = 1;
//             goto retry;
//         }
//     }
// }

extern void free_more_memory(void);
extern void sleep(void *chan, struct spinlock *lk);
extern void wakeup(void *chan);

void *kmalloc(size_t size) {
    void *ret = NULL;
retry:
    if(size < PGSIZE) { // Smaller, we use slob
        // printf("alloc from slob\n");
        // if(size == 1)
        //     panic("one byte?");
        ret = slob_alloc(size);
    } else { // more than one page, We use buddy
        ret = buddy_alloc(size);
    }
    /* slob和buddy分配失败，都会返回空 */
    if(!ret){
        /* 当前没有进程正在进行页回收 */
        if(!TestSetUseReserved){
            buddy_print_free();
            free_more_memory();
            buddy_print_free();
            printf("\n");

            /* 回收完毕，清除标记位 */
            if(!TestClearUseReserved)
                ER();
            wakeup(&use_reserved_flag);
        }
        else{
            /* 只需要一个进程回收页即可，如果检测到已经有其他进程回收了，睡眠 */
            /* 似乎没有锁需要释放 */
            sleep(&use_reserved_flag, NULL);
        }
        goto retry;
    }
    return ret;
}

void *kzalloc(size_t size) {
    void *ret = kmalloc(size);
    if(ret) {
        memset(ret, 0, size);
    }
    return ret;
}

/**
 * @deprecated use kmalloc(PGSIZE) instead
*/
void *kalloc(void) {
    return kzalloc(PGSIZE);
}

/**
 * IMPORTANT!
 * Note: kfree does not check the legality of addr now,
 * so never try to free an unallocated mem or try to free a truncated mem.
*/
void kfree(void *addr) {
    //todo: do more checks...
    if(addr == NULL) return;

    if((uint64)addr & ~PGMASK) { // piece
        slob_free(addr);
    } else {
        buddy_free(addr);
    }
    
}


void _kfree_safe(void **paddr) {
    if(paddr && *paddr) {
        kfree(*paddr);
        *paddr = NULL;
    }
}

void free_one_page(page_t *page) {
    zone_t *zone = &memory_zone;
    spin_lock(&zone->lru_lock);
    if(TestClearPageLRU(page))
        del_page_from_lru(zone, page);
    spin_unlock(&zone->lru_lock);

#ifdef CHECK_BOUNDARY
    if(page_mapped(page))
        ER();
#endif

    buddy_free_one_page(page);
}


/* new added functions for page frame reclaiming */
// void __pagevec_free(pagevec_t *pvec){
//     int i = pagevec_count(pvec);

//     /* buddy/kfree的接口可以考虑换一下，参数可选择页指针 */
//     while(--i >= 0){
//         kfree(NUM2PAGE(pvec->pages[i] - pages));
//     }
// }
uint64_t get_total_mem() {
    return buddy_gettotal();
}

uint64_t get_free_mem() {
    return buddy_getfree();
}

void *alloc_one_pagecache_page(){
  inc_page_state(nr_pagecache);
  return kzalloc(PGSIZE);
}

/* 释放使用的是页指针 */
void free_one_pagecache_page(void *page){
  dec_page_state(nr_pagecache);
  /* 原来freewalk中用的是kfree */
  put_page((page_t *)page);
}


void *alloc_one_anonymous_page(){
  inc_page_state(nr_anonymous);
  return kzalloc(PGSIZE);
}

/* 释放使用的是页指针 */
void free_one_anonymous_page(void *page){
  dec_page_state(nr_anonymous);
  /* 原来freewalk中用的是kfree */
  put_page((page_t *)page);
}

void *alloc_one_page_table_page(){
  inc_page_state(nr_page_table_pages);
  return kzalloc(PGSIZE);
}

/* 释放使用的是页的物理地址 */
void free_one_page_table_page(void *addr){
  dec_page_state(nr_page_table_pages);
  /* 原来freewalk中用的是kfree */
  put_page((uint64_t)addr);
}

void *alloc_one_trapframe_page(){
    inc_page_state(nr_trapframe);
    return kzalloc(PGSIZE);
}

struct trapframe;
typedef struct trapframe tf_t;
void free_one_trapframe_page(tf_t **pptf){
    dec_page_state(nr_trapframe);
    kfree_safe(pptf);
}

void *alloc_pages(uint64_t size, int type){
    if(size & ~PGMASK)
        ER();

    int nr = size >> PGSHIFT;
    if(type == 0)
        add_page_state(nr_pipe, nr);
    else if(type == 1)
        add_page_state(nr_anonymous, nr); 
    else    
        ER();
    return kmalloc(size);
}

void free_pages(void *addr, int type){
    page_t *page = PATOPAGE(addr);

    int nr = 1 <<  page->order;
    if(type == 0)
        sub_page_state(nr_pipe, nr);
    else if(type == 1)
        sub_page_state(nr_anonymous, nr); 
    else    
        ER();
    return kfree(addr);
}