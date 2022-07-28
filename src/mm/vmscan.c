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
#define __MODULE_NAME__ VMSCAN
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

struct scan_control {
	/* Ask refill_inactive_zone, or shrink_cache to scan this many pages */
	unsigned long nr_to_scan;

	/* Incremented by the number of inactive pages that were scanned */
	unsigned long nr_scanned;

	/* Incremented by the number of pages reclaimed */
	unsigned long nr_reclaimed;

	unsigned long nr_mapped;	/* From page_state */

	/* How many pages shrink_cache() should reclaim */
	int nr_to_reclaim;

	// /* Ask shrink_caches, or shrink_zone to scan at this priority */
	// unsigned int priority;

	// /* This context's GFP mask */
	// unsigned int gfp_mask;

	int may_writepage;
};

typedef struct scan_control scan_control_t;

#define lru_to_page(_head) (list_entry((_head)->prev, page_t, lru))

// /* Must be called with page's pte_chain_lock held. */
// static inline int page_mapping_inuse(page_t *page)
// {
// 	struct address_space *mapping = page->mapping;

// 	/* Page is in somebody's page tables. */
// 	if (page_mapped(page))
// 		return 1;

// 	/* XXX: does this happen ? */
// 	if (!mapping)
// 		return 0;

// 	/* File is mmap'd by somebody. */
// 	if (!list_empty(&mapping->i_mmap))
// 		return 1;
// 	if (!list_empty(&mapping->i_mmap_shared))
// 		return 1;

// 	return 0;
// }

extern bio_t *get_rw_pages_bio(entry_t *entry, uint64 buff, uint32 pg_id, int pg_cnt, int rw);
/*
 * pageout is called by shrink_list() for each dirty page. Calls ->writepage().
 */
static void pageout(page_t *page, struct address_space *mapping)
{
	bio_t *bio = get_rw_pages_bio(mapping->host, NUM2PAGE(page - pages), page->index, 1, WRITE);
	submit_bio(bio);	
}
/*
 * shrink_list adds the number of reclaimed pages to sc->nr_reclaimed
 * free的页有位于page cache的页；
 * 没有swap机制的情况下，可以释放的位于用户空间的只有mmap系统调用映射的页；(mapped pages)
 * 有swap机制的情况下，可以free匿名页（anonymous page）
 */
static int shrink_list(struct list_head *page_list, struct scan_control *sc){
  LIST_HEAD(ret_pages);
	struct pagevec freed_pvec;
	int pgactivate = 0;
	int reclaimed = 0;

  pagevec_init(&freed_pvec, 1);
  while(!list_empty(page_list)){
    address_space_t *mapping;
    page_t *page;
    int referenced;

    page = lru_to_page(page_list);
    list_del(&page->lru);
		mapping = page->mapping;

    if(TestSetPageLocked(page))
      goto keep;

    if(PageActive(page))
      ER();

    sc->nr_scanned++;

    // /* Double the slab pressure for mapped and swapcache pages */
		// if (page_mapped(page) || PageSwapCache(page))
		// 	sc->nr_scanned++;

		if (PageWriteback(page))
			goto keep_locked;

    // todo("add bit lock");
    pte_chain_lock(page);
    // referenced = page_referenced(page);
		// /* In active use or really unfreeable?  Activate it. */
		// if (referenced && page_mapping_inuse(page)){
    //   pte_chain_unlock(page);
		// 	goto activate_locked;
    // }

    /*
		 * The page is mapped into the page tables of one or more
		 * processes. Try to unmap it here.
		 * 映射到用户页表的页（目前只考虑mmap），先解映射
		 */
		if (page_mapped(page)) {
			try_to_unmap(page);
		}
		pte_chain_unlock(page);

		/* 写回dirty页 */
		if(PageDirty(page)){
			pageout(page, mapping);
		}

		/* 如果现在页引用数为2（page/swap cache + shrink cache 引用的，那么可以回收 */
		if(page->refcnt != 2)
			goto keep_locked;

		/* 从pagecache中移除 */
		spin_lock(&mapping->tree_lock);
		__remove_from_page_cache(page);
		spin_unlock(&mapping->tree_lock);
		put_page(page);

free_it:
		unlock_page(page);
		reclaimed++;
		put_page(page);
		continue;

activate_locked:
		SetPageActive(page);
		pgactivate++;
keep_locked:
		unlock_page(page);
keep:
		list_add(&page->lru, &ret_pages);
		if(PageLRU(page))
			ER();
  }

	/**
	 * 在try_to_ummap中更改了pte，可能是当前pagetable的pte。
	 * 所以执行当前代码的进程在上个while执行的过程中被调度的话，
	 * 有一定的机率出现bug。
	 */
	sfence_vma();
	/* 把ret_pages合并到page_list上面， 前者是局部变量，函数结束后会释放；以后者
		作为新的链表头。 */
	list_splice(&ret_pages, page_list);
	mod_page_state(pgactivate, pgactivate);
	sc->nr_reclaimed += reclaimed;
	return reclaimed;
}


/**
 * @brief 从zone的inactive list上回收page，交给shrink list函数。
 * 
 */
static void shrink_cache(zone_t *zone, struct scan_control *sc){
  LIST_HEAD(page_list);
	struct pagevec pvec;
	int max_scan = sc->nr_to_scan;

	pagevec_init(&pvec, 1);

	lru_add_drain();
	spin_lock(&zone->lru_lock);
  while(max_scan > 0){
    page_t *page;
		int nr_taken = 0;
		int nr_scan = 0;
		int nr_freed;

    while (nr_scan++ < SWAP_CLUSTER_MAX && !list_empty(&zone->inactive_list)) {
      page = lru_to_page(&zone->inactive_list);
      if(!TestClearPageLRU(page))
        ER();
      list_del(&page->lru);
      /* 说明ref前为0 */
      if (ref_page(page) == 1) {
				/*
				 * It is being freed elsewhere
				 */
				put_page(page);
				SetPageLRU(page);
				list_add(&page->lru, &zone->inactive_list);
				continue;
			}
      list_add(&page->lru, &page_list);
      nr_taken++;
    }
    zone->nr_inactive -= nr_taken;
    zone->pages_scanned += nr_scan;
    spin_unlock(&zone->lru_lock);

    if(nr_taken == 0)
      goto done;
  
    max_scan -= nr_scan;

    nr_freed = shrink_list(&page_list, sc);

    sc->nr_to_reclaim -= nr_freed;

    spin_lock(&zone->lru_lock);
    /*
		 * Put back any unfreeable pages.
		 * 放回shrink list不能释放的页。
		 */
    while(!list_empty(&page_list)){
      page = lru_to_page(&page_list);
      if (TestSetPageLRU(page))
        ER();
      list_del(&page->lru);
      if (PageActive(page))
				add_page_to_active_list(zone, page);
			else
				add_page_to_inactive_list(zone, page);
      if (!pagevec_add(&pvec, page)) {
				spin_unlock(&zone->lru_lock);
				__pagevec_release(&pvec);
				spin_lock(&zone->lru_lock);
			}    
    }
  }
  spin_unlock(&zone->lru_lock);
done:
  pagevec_release(&pvec);
}

/*
 * This moves pages from the active list to the inactive list.
 *
 * We move them the other way if the page is referenced by one or more
 * processes, from rmap.
 *
 * If the pages are mostly unmapped, the processing is fast and it is
 * appropriate to hold zone->lru_lock across the whole operation.  But if
 * the pages are mapped, the processing is slow (page_referenced()) so we
 * should drop zone->lru_lock around each page.  It's impossible to balance
 * this, so instead we remove the pages from the LRU while processing them.
 * It is safe to rely on PG_active against the non-LRU pages in here because
 * nobody will play with that bit on a non-LRU page.
 *
 * The downside is that we have to touch page->_count against each page.
 * But we had to alter page->flags anyway.
 */
static void
refill_inactive_zone(zone_t *zone, struct scan_control *sc){
  int pgmoved;
	int pgdeactivate = 0;
	int pgscanned = 0;
	int nr_pages = sc->nr_to_scan;
	LIST_HEAD(l_hold);	/* The pages which were snipped off */
	LIST_HEAD(l_inactive);	/* Pages to go onto the inactive_list */
	LIST_HEAD(l_active);	/* Pages to go onto the active_list */
	page_t *page;
	struct pagevec pvec;
	int reclaim_mapped = 0;
	long mapped_ratio;
	long distress;
	long swap_tendency;

  lru_add_drain();
  spin_lock(&zone->lru_lock);
  /* 第一次遍历active list， 把一些页移到l_hold上 */
  while(pgscanned < nr_pages && !list_empty(&zone->active_list)){
    page = lru_to_page(&zone->active_list);
    if(!TestClearPageLRU(page))
      ER();
    list_del(&page->lru);
    if(page->refcnt == 0){
      /*
			 * It was already free!  release_pages() or put_page()
			 * are about to remove it from the LRU and free it. So
			 * put the refcount back and put the page back on the
			 * LRU
			 */
      SetPageLRU(page);
      list_add(&page->lru, &zone->active_list);
    }
    else{
      /* 引用数加1 */
      get_page(page);
      list_add(&page->lru, &l_hold);
      pgmoved++;
    }
    pgscanned++;
  }
  zone->pages_scanned += pgscanned;
  zone->nr_active -= pgmoved;
  spin_unlock(&zone->lru_lock);

  /* linux中用到了一些启发式（heuristic）方法来计算swap tendency，从而决定是否回收mmaped页
			(reclaim_mmaped) */
  
  /* 第二次遍历l_hold，把页移到l_active或者l_inactive */
  while(!list_empty(&l_hold)){
    page = lru_to_page(&l_hold);
    list_del(&page->lru);
    /* 目前没有实现swap的功能，采取的回收策略是映射到用户空间的页(page->_mapcount > 0)一律不回收 */
    if(page_mapped(page)){
			if(!reclaim_mapped)
      	list_add(&page->lru, &l_active);
      continue;
    }
    list_add(&page->lru, &l_inactive);
  }

  pagevec_init(&pvec, 1);
  pgmoved = 0;
  spin_lock(&zone->lru_lock);
  /* 第三次遍历，把l_inactive移到全局inactive list */
  while(!list_empty(&l_inactive)){
    page = lru_to_page(&l_inactive);
    if (TestSetPageLRU(page))
      ER();
		if (!TestClearPageActive(page))
      ER();
		list_move(&page->lru, &zone->inactive_list);
		pgmoved++;
		if (!pagevec_add(&pvec, page)) {
			zone->nr_inactive += pgmoved;
			spin_unlock(&zone->lru_lock);
			pgdeactivate += pgmoved;
			pgmoved = 0;
			// if (buffer_heads_over_limit)
				// pagevec_strip(&pvec);
			/* 前面使用了get_page，所以这里应该使用release page进行释放 */
			__pagevec_release(&pvec);
			spin_lock(&zone->lru_lock);
		}
  }
  zone->nr_inactive += pgmoved;
	pgdeactivate += pgmoved;

  pgmoved = 0;
  /* 第四次遍历 */
  while (!list_empty(&l_active)) {
		page = lru_to_page(&l_active);
		if (TestSetPageLRU(page))
      ER();
		if(!PageActive(page))
      ER();  
		list_move(&page->lru, &zone->active_list);
		pgmoved++;
		if (!pagevec_add(&pvec, page)) {
			zone->nr_active += pgmoved;
			pgmoved = 0;
			spin_unlock(&zone->lru_lock);
			__pagevec_release(&pvec);
			spin_lock(&zone->lru_lock);
		}
	}
	zone->nr_active += pgmoved;
	spin_unlock(&zone->lru_lock);
	pagevec_release(&pvec);

  /* 修改一个zone的页的状态 */
	// mod_page_state_zone(zone, pgrefill, pgscanned);
	// mod_page_state(pgdeactivate, pgdeactivate);
}

/**
 * @brief 从inactive list上回收32个页
 * 
 */
static void
shrink_zone(struct zone *zone, struct scan_control *sc){
	/* 从active list回收页到inactive list */
	refill_inactive_zone(zone, sc);
	/* 从inactive list回收页到buddy */
	shrink_cache(zone, sc);

  // unsigned long nr_active;
	// unsigned long nr_inactive;

	// /*
	//  * Add one to `nr_to_scan' just to make sure that the kernel will
	//  * slowly sift through the active list.
	//  */
	// zone->nr_scan_active += (zone->nr_active >> sc->priority) + 1;
	// nr_active = zone->nr_scan_active;
	// if (nr_active >= SWAP_CLUSTER_MAX)
	// 	zone->nr_scan_active = 0;
	// else
	// 	nr_active = 0;

	// zone->nr_scan_inactive += (zone->nr_inactive >> sc->priority) + 1;
	// nr_inactive = zone->nr_scan_inactive;
	// if (nr_inactive >= SWAP_CLUSTER_MAX)
	// 	zone->nr_scan_inactive = 0;
	// else
	// 	nr_inactive = 0;

	// sc->nr_to_reclaim = SWAP_CLUSTER_MAX;

	// while (nr_active || nr_inactive) {
	// 	if (nr_active) {
	// 		sc->nr_to_scan = min(nr_active,
	// 				(unsigned long)SWAP_CLUSTER_MAX);
	// 		nr_active -= sc->nr_to_scan;
	// 		refill_inactive_zone(zone, sc);
	// 	}

	// 	if (nr_inactive) {
	// 		sc->nr_to_scan = min(nr_inactive,
	// 				(unsigned long)SWAP_CLUSTER_MAX);
	// 		nr_inactive -= sc->nr_to_scan;
	// 		shrink_cache(zone, sc);
	// 		if (sc->nr_to_reclaim <= 0)
	// 			break;
	// 	}
	// }
}

/*
 * This is the main entry point to direct page reclaim.
 *
 * If a full scan of the inactive list fails to free enough memory then we
 * are "out of memory" and something needs to be killed.
 * 扫描inactive list，如果多次扫描后仍然失败了，就out of memory
 */
int try_to_free_pages(){
  zone_t *zone = &memory_zone;
  // int priority;
	int ret = 0;
	// int total_scanned = 0, total_reclaimed = 0;
	// struct reclaim_state *reclaim_state = current->reclaim_state;
	struct scan_control sc;
	unsigned long lru_pages = 0;
	int i;

  sc.may_writepage = 0;
  lru_pages = zone->nr_active + zone->nr_inactive;

  /* 接下来的步骤在linxu中是一个循环，priority递减 */
  sc.nr_mapped = read_page_state(nr_mapped);
	sc.nr_scanned = 0;
	sc.nr_reclaimed = 0;
	// sc.priority = priority;
	// shrink_caches(zone, &sc);
	shrink_zone(zone, &sc);
  // shrink_slab(sc.nr_scanned, gfp_mask, lru_pages);
  if (sc.nr_reclaimed >= SWAP_CLUSTER_MAX) {
			ret = 1;
			goto out;
	}
	// total_scanned += sc.nr_scanned;
	// total_reclaimed += sc.nr_reclaimed;

out:
}

/*
 * Kick pdflush then try to free up some ZONE_NORMAL memory.
 */
static void free_more_memory(void)
{
  /* 启动bdflush写回 */
	/* 是否会出现两个线程写回一个页的情况？ */
	// wakeup_bdflush(1024);
	// yield();

  try_to_free_pages();
}