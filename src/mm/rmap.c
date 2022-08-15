#include "config.h"
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
#include "mm/page.h"

// #define QUIET
#define __MODULE_NAME__ RMAP
#include "debug.h"

#include "mm/mm_types.h"
#include "mm/mm.h"
#include "rbtree.h"
#include "utils.h"
#include "memlayout.h"
#include "list.h"
#include "kthread.h"
#include "mm/page.h"
#include "pagevec.h"
#include "swap.h"
#include "writeback.h"
#include "mm/rmap.h"
#include "page-flags.h"

#ifdef RMAP
/**
 * @brief 反向映射模块，参考linux2.6.0
 * 
 */

/**
 * @brief next_and_idx低位用来存idx，高位存next地址。
 * 
 */
static inline struct pte_chain *pte_chain_next(struct pte_chain *pte_chain)
{
	return (struct pte_chain *)(pte_chain->next_and_idx & (~NRPTE));
}

static inline struct pte_chain *pte_chain_ptr(unsigned long pte_chain_addr)
{
	return (struct pte_chain *)(pte_chain_addr & (~NRPTE));
}

static inline int pte_chain_idx(struct pte_chain *pte_chain)
{
	return pte_chain->next_and_idx & (NRPTE);
}

static inline unsigned long
pte_chain_encode(struct pte_chain *pte_chain, int idx)
{
	return (unsigned long)pte_chain | idx;
}

/**
 * __pte_chain_free - free pte_chain structure
 * @pte_chain: pte_chain struct to free
 */
void __pte_chain_free(struct pte_chain *pte_chain)
{
	kfree(pte_chain);
}

/**
 * page_referenced - test if the page was referenced
 * @page: the page to test
 *
 * Quick test_and_clear_referenced for all mappings to a page,
 * returns the number of processes which referenced the page.
 * Caller needs to hold the pte_chain_lock.
 *
 * If the page has a single-entry pte_chain, collapse that back to a PageDirect
 * representation.  This way, it's only done under memory pressure.
 */
// int page_referenced(page_t *page){
//   pte_chain_t *pc;
//   int referenced = 0;

//   if (TestClearPageReferenced(page))
// 		referenced++;

//   /* 映射了一次，清掉valid位 */
//   if (PageDirect(page)) {
//     pte_t *pte = (pte_t *)(page->pte.direct);
// 		/* 对应access位 */
//     if(ptep_test_and_clear_access(pte))
// 			referenced++;
// 	} else {
//     int nr_chains = 0;
//     /* Check all the page tables mapping this page. */
//     for (pc = page->pte.chain; pc; pc = pte_chain_next(pc)) {
//       int i;
      
//       for(int i = NRPTE-1; i >= 0; i--){
//         pte_addr_t pte_paddr = pc->ptes[i];
//         pte_t *p;

//         if(!pte_paddr)
//           break;
//         p = (pte_t *)pte_paddr;
//         if(ptep_test_and_clear_valid(p))
//           referenced++;

//         nr_chains++;
//       }
//     }
//     if (nr_chains == 1) {
// 			pc = page->pte.chain;
//       /* 在pte_chain中时倒放的，从NRPTE开始 */
// 			page->pte.direct = pc->ptes[NRPTE-1];
// 			SetPageDirect(page);
// 			pc->ptes[NRPTE-1] = 0;
// 			kfree(pc);
// 		}
//   } 

// 	return referenced;
// }

/**
 * try_to_unmap_one - worker function for try_to_unmap
 * @page: page to unmap
 * @ptep: page table entry to unmap from page
 * 问题：不知道包含pte的页表属于哪个address space（mm_strcut），也不知道其
 * 对应的虚拟地址，这样子不知道是否需要刷新当前的tlb及其对应的地址，但是是否有必要
 * 保存这两项？
 * 
 * 最好的做法是得到这两项，然后在这里刷新对应地址的tlb
 */
static int try_to_unmap_one(page_t *page, pte_addr_t paddr)
{
  pte_t *ptep = (pte_t *)(paddr);
	pte_t pte;

	/* clear pte */
	pte = *ptep;
	*ptep = 0;

	/* Move the dirty bit to the physical page now the pte is gone. */
	if(pte_dirty(pte))
		SetPageDirty(page);

	/* 如果paddr位于当前页表，本该刷掉paddr在当前页表中的va，但是考虑到查找的效率可能不高，还不如在shrink_list中
		刷掉整个页表 */
	// put_page(page);
	return 0;
}

/**
 * try_to_unmap - try to remove all page table mappings to a page
 * @page: the page to get unmapped
 *
 * Tries to remove all the page table entries which are mapping this
 * page, used in the pageout path.  Caller must hold the page lock
 * and its pte chain lock.  Return values are:
 */
int try_to_unmap(page_t * page)
{
	struct pte_chain *pc, *next_pc, *start;
	// int victim_i = -1;

	// /* This page should not be on the pageout lists. */
	// if (PageReserved(page))
	// 	BUG();
	if (!PageLocked(page))
    ER();
	/* We need backing store to swap out a page. */
	if (!page->mapping)
    ER();

	/* one mappig */
	if (PageDirect(page)) {
		try_to_unmap_one(page, page->pte.direct);
		page->pte.direct = 0;
		ClearPageDirect(page);
		goto out;
	}

	/* many mappings */
	start = page->pte.chain;
	for(pc = start; pc; pc = next_pc){
		int i;

		next_pc = pte_chain_next(pc);
		for(i = pte_chain_idx(pc); i < NRPTE; i++){
			pte_addr_t pte_addr = pc->ptes[i];

			if(!pte_addr)
				continue;
			
			try_to_unmap_one(page, pte_addr);
		}

		page->pte.chain = pte_chain_next(start);
		kfree(start);
		start = page->pte.chain;
	}

out:
	if (!page_mapped(page))
		dec_page_state(nr_mapped);
	return 0;
}


/**
 * page_add_rmap - add reverse mapping entry to a page
 * @page: the page to add the mapping to
 * @ptep: the page table entry mapping this page
 *
 * Add a new pte reverse mapping to a page.
 * The caller needs to hold the mm->page_table_lock.
 */
void page_add_rmap(page_t *page, pte_t *ptep) {
	pte_chain_t *cur_pte_chain;

	pte_chain_lock(page);

	/* 之前没有rmap */
	if(page->pte.direct == 0){
		page->pte.direct = ptep;
		SetPageDirect(page);
		inc_page_state(nr_mapped);
		goto out;
	}

	/* 之前仅有一个rmap */
	if(PageDirect(page)){
		/* convert a direct pointer into a pte_chain */
		ClearPageDirect(page);
		cur_pte_chain = (pte_chain_t *)kzalloc(sizeof(pte_chain_t));

		/* 从尾往头放 */
		cur_pte_chain->ptes[NRPTE-1] = page->pte.direct;
		cur_pte_chain->ptes[NRPTE-2] = ptep;
		cur_pte_chain->next_and_idx = pte_chain_encode(NULL, NRPTE - 2);
		page->pte.direct = 0;
		page->pte.chain = cur_pte_chain;
		cur_pte_chain = NULL;
		goto out;
	}

	cur_pte_chain = page->pte.chain;
	/* full */
	if(cur_pte_chain->ptes[0]){
		pte_chain_t *new_pte_chain = kzalloc(sizeof(pte_chain_t));

		new_pte_chain->next_and_idx = pte_chain_encode(cur_pte_chain, NRPTE - 1);
		page->pte.chain = new_pte_chain;
		new_pte_chain->ptes[NRPTE-1] = ptep;
		new_pte_chain = NULL;
		goto out;
	}

	cur_pte_chain->ptes[pte_chain_idx(cur_pte_chain) - 1] = ptep;
	cur_pte_chain->next_and_idx--;
out:
	pte_chain_unlock(page);
	return;
}

/**
 * page_remove_rmap - take down reverse mapping to a page
 * @page: page to remove mapping from
 * @ptep: page table entry to remove
 *
 * Removes the reverse mapping from the pte_chain of the page,
 * after that the caller can clear the page table entry and free
 * the page.
 * Caller needs to hold the mm->page_table_lock.
 */
void page_remove_rmap(page_t *page, pte_t *ptep){
	pte_addr_t pte_paddr = ptep_to_paddr(ptep);
	pte_chain_t *pc;

	pte_chain_lock(page);

	if(!page_mapped(page))
		goto out_unlock;

	if(PageDirect(page)){
		if(page->pte.direct == pte_paddr){
			page->pte.direct = 0;
			ClearPageDirect(page);
			goto out;
		}
	}
	else{
		pte_chain_t *start = page->pte.chain;
		pte_chain_t *next;
		int victim_i = -1;

		for(pc = start; pc; pc = next){
			int i;
			
			next = pte_chain_next(pc);

			/* i不能直接从0开始，因为是从NRPTE往前倒着放的，所以第一个chain可能没有放满(新申请的chain放在最前面，见page_add_rmap) */
			for(i = pte_chain_idx(pc); i < NRPTE; i++){
				pte_addr_t pa = pc->ptes[i];

				if(victim_i == -1)
					victim_i = i;
				if(pa != pte_paddr)
					continue;
				pc->ptes[i] = start->ptes[victim_i];
				start->ptes[victim_i] = 0;

				if(victim_i == NRPTE-1){
					/* free the start chain */
					page->pte.chain = pte_chain_next(start);
					__pte_chain_free(start);
				}
				else{
					start->next_and_idx++;
				}
				goto out;
			}
		}
	}

out:
	if (!page_mapped(page))
		dec_page_state(nr_mapped);
out_unlock:
	pte_chain_unlock(page);
	return;
}

void print_page_rmap(page_t *page){
  if(page->pte.direct == 0)
		goto out;

  printf(grn("%d rmaps of the page: "), page->mapcount);
  if(PageDirect(page)) {
    printf("%x\t", page->pte.direct);
  }
  else{
		pte_chain_t *start = page->pte.chain;
		pte_chain_t *next;
		pte_chain_t *pc;

    for(pc = start; pc; pc = next){
      int i;
      
      next = pte_chain_next(pc);

			for(i = pte_chain_idx(pc); i < NRPTE; i++){
				pte_addr_t pa = pc->ptes[i];
				printf("%x\t", pa);
			}
    }
  }

	printf("\n");
out:
	return;
}








#endif