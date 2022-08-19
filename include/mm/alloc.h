#ifndef _H_ALLOC_
#define _H_ALLOC_

#include "page.h"

void  kinit(void);
void *kalloc(void);
void  kfree(void *addr);
void  _kfree_safe(void **paddr);
void *kzalloc(size_t size);
void *kmalloc(size_t size);
uint64_t get_total_mem();
uint64_t get_free_mem();

#define kfree_safe(pp) _kfree_safe((void **)(pp))
void free_one_page(page_t *page);


void *alloc_one_pagecache_page();
void free_one_pagecache_page(void *page);

void *alloc_one_anonymous_page();
void free_one_anonymous_page(void *page);

void *alloc_one_page_table_page();
void free_one_page_table_page(void *addr);

struct trapframe;
typedef struct trapframe tf_t;
void *alloc_one_trapframe_page();
void free_one_trapframe_page(tf_t **pptf);



void *alloc_pages(uint64_t size, int type);
void free_pages(void *addr, int type);

#define alloc_pipe_pages(size) alloc_pages(size, 0)
#define free_pipe_pages(addr) free_pages(addr, 0)

#define alloc_anonymous_pages(size) alloc_pages(size, 1)
#define free_anonymous_pages(addr) free_pages(addr, 1)

#define alloc_kstack_pages(size) alloc_pages(size, 2)
#define free_kstack_pages(addr) free_pages(addr, 2)

#define AF_KERNEL 0x0
#define AF_USER 0x1

#endif