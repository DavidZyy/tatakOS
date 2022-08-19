/**
 * This file define the basic operations for pages.
*/
#include "mm/page.h"
#include "mm/alloc.h"
#include "atomic/spinlock.h"
#include "defs.h"
#include "printf.h"
#include "param.h"

#define __MODULE_NAME__ PAGE
#include "debug.h"

#include "atomic/sleeplock.h"
#include "utils.h"
#include "page-flags.h"
#include "list.h"
#include "config.h"
#include "mm/rmap.h"

page_t pages[PAGE_NUMS];
struct spinlock reflock;
struct page_state pg_state = {0,0,0,0,0,0,0,0,0,0};

extern void wakeup(void *chan);
extern void sleep(void *chan, struct spinlock *lk);

uint64_t __read_page_state(uint64_t offset){
  void *ptr = &pg_state;

  return *((uint64_t *)(ptr + offset)); 
}

void __mod_page_state(uint64_t offset, uint64_t delta){
  void *ptr = &pg_state;

  *((uint64_t *)(ptr + offset)) += delta;
}

void reset_page(page_t *page){
  page->flags = 0;
  INIT_LIST_HEAD(&page->lru);
  page->mapping = 0;
  page->index = 0;
#ifdef RMAP
  page->pte.direct = 0;
#endif
}

void page_init(void) {
  initlock(&reflock, "reflock");
  for(int i = 0; i < PAGE_NUMS; i++) {
    pages[i].refcnt = (atomic_t){1};
    pages[i].order = 0;
    pages[i].alloc = 1;
    reset_page(&pages[i]);
  }
}

// pgref_t ref_page(uint64_t pa) {
//   // pgref_t ret;
//   // acquire(&reflock);
//   // ret = ++pages[PAGE2NUM(pa)].refcnt;
//   // release(&reflock);
//   // return ret;
//   return atomic_inc(&pages[PAGE2NUM(pa)].refcnt);
// }



// pgref_t page_ref(uint64_t pa) {
//   return atomic_get(&pages[PAGE2NUM(pa)].refcnt);
// }

int page_type(uint64_t pa) {
  return pages[PAGE2NUM(pa)].type;
}


extern void *alloc_one_page_table_page();

pte_t *__walk(pagetable_t pagetable, uint64 va, int alloc, int pg_spec) {
  if(va >= MAXVA)
    panic("walk");

  for(int level = 2; level > pg_spec; level--) {
    pte_t *pte = &pagetable[PX(level, va)];
    if(*pte & PTE_V) {
      pagetable = (pagetable_t)PTE2PA(*pte);
    } else {
      if(!alloc || (pagetable = (pde_t*)alloc_one_page_table_page()) == 0)
        return 0;
      *pte = PA2PTE(pagetable) | PTE_V;
    }
  }
  return &pagetable[PX(pg_spec, va)];
}

// Create PTEs for virtual addresses starting at va that refer to
// physical addresses starting at pa. va and size might not
// be page-aligned. Returns 0 on success, -1 if walk() couldn't
// allocate a needed page-table page.
int __mappages(pagetable_t pagetable, uint64 va, uint64 size, uint64 pa, int prot, int spec) {
  uint64 start, a, last;
  pte_t *pte;

  if(size == 0)
    panic("mappages: size");
  
  start = a = PGROUNDDOWN_SPEC(va, spec);
  last = PGROUNDDOWN_SPEC(va + size - 1, spec);
  for(;;){
    if((pte = __walk(pagetable, a, 1, spec)) == 0)
      goto bad;
    if(*pte & PTE_V) {
      pte_print(pte);
      panic("mappages: remap");
    }
    *pte = PA2PTE_SPEC(pa, spec) | prot | PTE_V;
#ifdef RMAP
    /* 给用户空间的映射建立rmap，使用上面的会在ioremap挂掉 */
    if(in_rmap_area(a)){
      page_t *page = PATOPAGE(pa);

      page_add_rmap(page, pte);
    }
#endif
    if(a == last)
      break;
    a += PGSIZE_SPEC(spec);
    pa += PGSIZE_SPEC(spec);
  }
  return 0;

  bad:
  debug("mappages fail");
  __uvmunmap(pagetable, start, (a-start)/PGSIZE_SPEC(spec), 0, spec);
  return -1;
}

// Remove npages of mappings starting from va. va must be
// page-aligned. The mappings must exist.
// Optionally free the physical memory.
void __uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, int do_free, int spec) {
  uint64 a;
  pte_t *pte;
  int pgsize = PGSIZE_SPEC(spec);
  uint64_t pa;

  int need_flush = FETCH_PGTBL() == pagetable;

  if((va % pgsize) != 0)
    panic("uvmunmap: not aligned");

  for(a = va; a < va + npages*pgsize; a += pgsize){
    // printf(ylw("a: %p\n"), a);
    if((pte = __walk(pagetable, a, 0, spec)) == 0){
      continue;
    }
    if((*pte & PTE_V) == 0) {
      continue;
    }
    if((*pte & (PTE_R | PTE_W | PTE_X)) == 0)
      panic("uvmunmap: not a leaf");

#ifdef RMAP
    if(in_rmap_area(a)){
      pa = PTE2PA(*pte);
      page_t *page = PATOPAGE(pa);

      page_remove_rmap(page, pte);
    }
#endif

    if(do_free){
      pa = PTE2PA(*pte);
      // kfree((void*)pa);
      /* kfree不会把页从lru上移动下来，被坑了 */
      page_t *page = PATOPAGE(pa);
      if(page->refcnt.counter == 1){
        /* 这样区分是否准确，如果anonymous page在swap cache中呢？*/
        if(page->mapping)
          free_one_pagecache_page(page);
        else
          free_one_anonymous_page(page);
      }
      else
        put_page(page);
    }
    *pte = 0;

/* 位置不对，应该加到put_page前面 */
// #ifdef RMAP
//     if(in_rmap_area(a)){
//       page_t *page = PATOPAGE(pa);

//       page_remove_rmap(page, pte);
//     }
// #endif

    if(need_flush)
      sfence_vma_addr(a);
  }
}

void pte_print(pte_t *pte) {
  uint64 pa = PTE2PA(*pte);
  char rwxuvc[7];
  rwxuvc[0] = *pte & PTE_R ? 'r' : '-';
  rwxuvc[1] = *pte & PTE_W ? 'w' : '-';
  rwxuvc[2] = *pte & PTE_X ? 'x' : '-';
  rwxuvc[3] = *pte & PTE_U ? 'u' : '-';
  rwxuvc[4] = *pte & PTE_V ? 'v' : '-';
  rwxuvc[5] = *pte & PTE_COW ? 'c' : '-';
  rwxuvc[6] = '\0';
  
  printf("pte %#lx pa %#lx %s\n", *pte, pa, rwxuvc);
}


pgref_t __page_refcnt_pointer(page_t *page){
  return atomic_get(&page->refcnt);
}

pgref_t __page_refcnt_paddr(uint64_t pa){
  return __page_refcnt_pointer(PATOPAGE(pa));
}


pgref_t __deref_page_pointer(page_t *page) {
  return atomic_dec(&page->refcnt) - 1;
}

pgref_t __deref_page_paddr(uint64_t pa) {
  return __deref_page_pointer(&pages[PAGE2NUM(pa)]);
}

pgref_t __get_page_pointer(page_t *page){
  assert(atomic_get(&page->refcnt) > 0);
  return atomic_inc(&page->refcnt) + 1;
}

pgref_t __get_page_paddr(uint64_t pa){
  return __get_page_pointer(&pages[PAGE2NUM(pa)]);
}


pgref_t __put_page_pointer(page_t *page){
  pgref_t ret;

  if(page_refcnt(page) <= 0)
    ER();

  ret = atomic_dec(&page->refcnt) - 1;
  if(ret == 0){
    free_one_page(page);
  }
  return ret; 

}

pgref_t __put_page_padder(uint64_t pa){
  return __put_page_pointer(&pages[PAGE2NUM(pa)]);
}

/**
 * 使用类睡眠锁实现的lock page。
 */
void lock_page(page_t *page){
  page_spin_lock(page);
  while(unlikely(TestSetPageLocked(page))){
    while(PageLocked(page))
      /* 传入1告诉sleep是lock_page */
      sleep(page, (struct spinlock *)1);
  }
  page_spin_unlock(page);
}


void unlock_page(page_t *page){
  page_spin_lock(page);
  if (!TestClearPageLocked(page))
    ER();
  wakeup(page);
  page_spin_unlock(page);
}

void get_lock_page(page_t *page){
  get_page(page);
  lock_page(page);
}

void unlock_put_page(page_t *page){
  unlock_page(page);
  put_page(page);
}

/**
 * @brief 之前flag位是带移位的，原PG_diry = 1 << 现PG_diry 
 * 
 */
void set_page_dirty(uint64_t pa){
  page_t *page = &pages[PAGE2NUM(pa)];

  // page->flags |= PG_dirty;
  SetPageDirty(page);
}


void clear_page_dirty(uint64_t pa){
  page_t *page = &pages[PAGE2NUM(pa)];

  // page->flags &= ~PG_dirty;
  ClearPageDirty(page);
}

int page_is_dirty(uint64_t pa){
  page_t *page = &pages[PAGE2NUM(pa)];

  // return page->flags & PG_dirty;
  return PageDirty(page);
}
