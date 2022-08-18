#ifndef _H__PAGE_FLAGS
#define _H__PAGE_FLAGS
/* from linux 2.6.11 page-flags.h*/
#define PG_locked	 	 0	/* Page is locked. Don't touch. */
#define PG_error		 1
#define PG_referenced	 2
#define PG_uptodate		 3

#define PG_dirty	 	 4
#define PG_lru			 5
#define PG_active		 6
#define PG_slab			 7	/* slab debug (Suparna wants this) 用作slob */

#define PG_writeback		13	/* Page is under writeback */
#define PG_chainlock		15	/* lock bit for ->pte_chain */
#define PG_direct		16	/* ->pte_chain points directly at pte */

/**
 * lock_page时使用，防止sleep还没完成，就被wakeup了。
 */
#define PG_spinlock 17
#define PG_swapcache 18

/*
 * Global page accounting.  One instance per CPU.  Only unsigned longs are
 * allowed.
 * 记录页的状态，每个cpu一个
 */
struct page_state {
	uint64_t nr_dirty;		/* Dirty writeable pages */
	uint64_t nr_writeback;	/* Pages under writeback */
	uint64_t nr_mapped;	/* mapped into pagetables(包括所有的匿名页和部分的pagecache页，所以小于这两者之和) */


	/* 下面五个互不相交，加起来等于总的页？ */
	uint64_t nr_page_table_pages;/* Pages used for pagetables */
	uint64_t nr_slob;		/* In slob */
	uint64_t nr_buddy; /* (nr_free)在buddy中的free的页 */
	uint64_t nr_pagecache; /* in pagecache */	
	uint64_t nr_anonymous; /* 匿名页，stack, heap, private mmap */
};

typedef struct page_state page_state_t;

uint64_t __read_page_state(uint64_t offset);
void __mod_page_state(uint64_t offset, uint64_t delta);

#define read_page_state(member) \
	__read_page_state(offsetof(struct page_state, member))

#define mod_page_state(member, delta)	\
	__mod_page_state(offsetof(struct page_state, member), (delta))

#define inc_page_state(member)	mod_page_state(member, 1UL)
#define dec_page_state(member)	mod_page_state(member, 0UL - 1)
#define add_page_state(member,delta) mod_page_state(member, (delta))
#define sub_page_state(member,delta) mod_page_state(member, 0UL - (delta))

/*
 * Manipulation of page state flags
 * set_bit 已经被命名过了，更改为__set_bit。
 */
#define PageLocked(page)		\
		test_bit(PG_locked, &(page)->flags)
#define SetPageLocked(page)		\
		__set_bit(PG_locked, &(page)->flags)
#define TestSetPageLocked(page)		\
		test_and_set_bit_lock(PG_locked, &(page)->flags)
#define ClearPageLocked(page)		\
		clear_bit_unlock(PG_locked, &(page)->flags)
#define TestClearPageLocked(page)	\
		test_and_clear_bit(PG_locked, &(page)->flags)

#define PageError(page)		test_bit(PG_error, &(page)->flags)
#define SetPageError(page)	__set_bit(PG_error, &(page)->flags)
#define ClearPageError(page)	__clear_bit(PG_error, &(page)->flags)

#define PageReferenced(page)	test_bit(PG_referenced, &(page)->flags)
#define SetPageReferenced(page)	__set_bit(PG_referenced, &(page)->flags)
#define ClearPageReferenced(page)	__clear_bit(PG_referenced, &(page)->flags)
#define TestClearPageReferenced(page) test_and_clear_bit(PG_referenced, &(page)->flags)

#define PageUptodate(page)	test_bit(PG_uptodate, &(page)->flags)
#ifndef SetPageUptodate
#define SetPageUptodate(page)	__set_bit(PG_uptodate, &(page)->flags)
#endif
#define ClearPageUptodate(page)	__clear_bit(PG_uptodate, &(page)->flags)

#define PageDirty(page)		test_bit(PG_dirty, &(page)->flags)
#define SetPageDirty(page)	__set_bit(PG_dirty, &(page)->flags)
#define TestSetPageDirty(page)	test_and_set_bit(PG_dirty, &(page)->flags)
#define ClearPageDirty(page)	__clear_bit(PG_dirty, &(page)->flags)
#define TestClearPageDirty(page) test_and_clear_bit(PG_dirty, &(page)->flags)

#define SetPageLRU(page)	__set_bit(PG_lru, &(page)->flags)
#define PageLRU(page)		test_bit(PG_lru, &(page)->flags)
#define TestSetPageLRU(page)	test_and_set_bit(PG_lru, &(page)->flags)
#define TestClearPageLRU(page)	test_and_clear_bit(PG_lru, &(page)->flags)

#define PageActive(page)	test_bit(PG_active, &(page)->flags)
#define SetPageActive(page)	__set_bit(PG_active, &(page)->flags)
#define ClearPageActive(page)	__clear_bit(PG_active, &(page)->flags)
#define TestClearPageActive(page) test_and_clear_bit(PG_active, &(page)->flags)
#define TestSetPageActive(page) test_and_set_bit(PG_active, &(page)->flags)

/* slob */
#define PageSlab(page)		test_bit(PG_slab, &(page)->flags)
#define SetPageSlab(page)	__set_bit(PG_slab, &(page)->flags)
#define ClearPageSlab(page)	__clear_bit(PG_slab, &(page)->flags)
#define TestClearPageSlab(page)	test_and_clear_bit(PG_slab, &(page)->flags)
#define TestSetPageSlab(page)	test_and_set_bit(PG_slab, &(page)->flags)

#define PageWriteback(page)	test_bit(PG_writeback, &(page)->flags)
#define SetPageWriteback(page)						\
	do {								\
		if (!test_and_set_bit(PG_writeback,			\
				&(page)->flags))			\
			inc_page_state(nr_writeback);			\
	} while (0)
#define TestSetPageWriteback(page)					\
	({								\
		int ret;						\
		ret = test_and_set_bit(PG_writeback,			\
					&(page)->flags);		\
		if (!ret)						\
			inc_page_state(nr_writeback);			\
		ret;							\
	})
#define ClearPageWriteback(page)					\
	do {								\
		if (test_and_clear_bit(PG_writeback,			\
				&(page)->flags))			\
			dec_page_state(nr_writeback);			\
	} while (0)
#define TestClearPageWriteback(page)					\
	({								\
		int ret;						\
		ret = test_and_clear_bit(PG_writeback,			\
				&(page)->flags);			\
		if (ret)						\
			dec_page_state(nr_writeback);			\
		ret;							\
	})

#define PageDirect(page)	test_bit(PG_direct, &(page)->flags)
#define SetPageDirect(page)	__set_bit(PG_direct, &(page)->flags)
#define TestSetPageDirect(page)	test_and_set_bit(PG_direct, &(page)->flags)
#define ClearPageDirect(page)		__clear_bit(PG_direct, &(page)->flags)
#define TestClearPageDirect(page)	test_and_clear_bit(PG_direct, &(page)->flags)

#define PageSpinlock(page) test_bit(PG_spinlock, &(page)->flags)
#define SetPageSpinlock(page)	__set_bit(PG_spinlock, &(page)->flags)
#define TestSetPageSpinlock(page)	test_and_set_bit(PG_spinlock, &(page)->flags)
#define ClearPageSpinlock(page)		__clear_bit(PG_spinlock, &(page)->flags)
#define TestClearPageSpinlock(page)	test_and_clear_bit(PG_spinlock, &(page)->flags)


#define PageSwapCache(page)		test_bit(PG_swapcache, &(page)->flags)
#define SetPageSwapCache(page)	__set_bit(PG_swapcache, &(page)->flags)
#define ClearPageSwapCache(page)	__clear_bit(PG_swapcache, &(page)->flags)
#endif