#include "common.h"
#include "platform.h"
#include "mm/alloc.h"
#include "fs/fs.h"
#include "fs/file.h"
#include "mm/vm.h"
#include "kernel/proc.h"
#include "defs.h"
#include "debug.h"
#include "mm/page.h"
#include "radix-tree.h"
#include "mm/mm.h"
#include "bio.h"
#include "utils.h"

#include "fs/mpage.h"
/**
 * 这个文件里定义了关于页page的操作函数
 * linux mpage.c
 */

/**
 * @brief 从entry中读取或写回一个page到buff。
 * 得到bio结构体，记录了此次读写操作涉及到的sector信息。
 * 
 * linux readpage。
 * 
 * @param entry the entry of the file to read
 * @param buff the physical address of the page to store the result
 * @param index the index of page in the file
 * @return int 
 */
int rw_one_page(entry_t *entry, page_t *page, uint32 index, int rw){
  bio_t *bio;
  uint64_t buff = PAGETOPA(page);

  if(rw == WRITE){
    if(TestSetPageWriteback(page))
      ER();
  }

  bio = get_rw_pages_bio(entry, buff, index, 1, rw);
  if(bio)
    submit_bio(bio);
  
  if(rw == WRITE){
    if(!TestClearPageWriteback(page))
      ER();
  }
  
  return 0;
}


/**
 * @brief give a list of bio_vec and a start address buff, assign to 
 * bio_vec buff with the value of "buff", and plus offset for it.
 * 
 * @param first_bio_vec 
 * @param buff 
 */
void bio_vec_buff_assignment(bio_vec_t *first_bio_vec, uint64 buff, uint32_t bps){
  bio_vec_t *cur_bio_vec = first_bio_vec;
  while(cur_bio_vec != NULL){
    cur_bio_vec->bv_buff = (void *)buff;
    buff += cur_bio_vec->bv_count * bps;

    cur_bio_vec = cur_bio_vec->bv_next;
  }
}


/**
 * @brief Get the rw pages bio object
 *  找出所有要读的磁盘块号，记录在bio中，把一个页(page)按照扇区(sector)连续的原则分为段(segment)
 * @param entry 
 * @param buff 
 * @param pg_id 页号
 * @param pg_cnt 页数
 * @param rw 
 * @return bio_t* 
 */
bio_t *get_rw_pages_bio(entry_t *entry, uint64 buff, uint32 pg_id, int pg_cnt, int rw){
  bio_t *bio = kzalloc(sizeof(bio_t));
  struct bio_vec *first_bio_vec;
  /* 用来统计扇区总数是否符合条件 */
  uint32 bps = entry->fat->bytes_per_sec;

  first_bio_vec = fat_get_sectors(entry->fat, entry->clus_start, pg_id*PGSIZE, pg_cnt*PGSIZE);

  bio_vec_buff_assignment(first_bio_vec, buff, bps);

  bio->bi_io_vec = first_bio_vec; 
  bio->bi_rw = rw;
  bio->bi_dev = entry->fat->dev;

  return bio;
}

/**
 * 清除对应页的writeback位，释放数据结构
 */
void free_rw_page_list(rw_page_list_t *pg_list, int rw){
  rw_page_t *rw_page;

  for(rw_page = pg_list->head; rw_page; rw_page = rw_page->next){
    if(rw == WRITE){
      if(!TestClearPageWriteback(PATOPAGE(rw_page->pa)))
        ER();
    }
    kfree((void*)rw_page);
  }

  kfree(pg_list);
}


/**
 * 将由pg_head串联起来的页写回磁盘。
 * linux: mpage_writepages
 */
int rw_pages(entry_t *entry, rw_page_list_t *pg_list, int rw){
  rw_page_t *cur_page, *next_page;
  uint32_t nr_continuous_pages;

  if(rw == WRITE){
    rw_page_t *rw_page;
    for(rw_page = pg_list->head; rw_page; rw_page = rw_page->next){
      if(TestSetPageWriteback(PATOPAGE(rw_page->pa)))
        ER();
    }
  }
  /** 
   * 合并pg_id连续的页, 一批连续的页调用一次get_sectors, 这样可以使得得到的一个bio_vec
   * 包含的sectors尽可能多。因为在lookup_tag递归查询时，是按照页index递增的顺序查询的，所以
   * 在pg_head链表中的页index是递增的。
   * 
   * 补充：合并的要求比较严格，不仅要求页的pg_id是连续的，还要要求页的pa是连续的才行，要求比较严格。
   */
  cur_page = pg_list->head;
  while(cur_page){
    nr_continuous_pages = 1;

    /* find max counts of continuous page */
    next_page = cur_page->next;
    rw_page_t *tmp = cur_page;

    while(next_page){
      if((next_page->pg_id == tmp->pg_id+1) && (next_page->pa == tmp->pa + PGSIZE)){
        nr_continuous_pages++;
        next_page = next_page->next;
        tmp = tmp->next;
      }
      else
        break;
    }
    
    bio_t *bio = get_rw_pages_bio(entry, cur_page->pa, cur_page->pg_id, nr_continuous_pages, rw);
    submit_bio(bio);
    cur_page = next_page;
  }

  /* 这里别忘了释放pghead相关的结构体！ */
  free_rw_page_list(pg_list, rw);
  return 0;
}

void msync(uint64_t addr, uint64_t length, int flags){
  length = PGROUNDUP(length);
  uint64_t rest_cnts = length >> PGSHIFT;

  while(rest_cnts > 0) {
    struct proc *p = myproc();
    vma_t *vma = __vma_find_strict(p->mm, addr);

    if(!vma)
      ER();

    uint64_t vma_len_pg_cnts = vma->len >> PGSHIFT;
    uint64_t cnts = min(vma_len_pg_cnts, rest_cnts);
    int begin_id = vma->offset;
    rw_page_list_t *rwlist = kzalloc(sizeof(rw_page_list_t));

    for(int i = 0; i < cnts; i++){
      rw_page_t *rwpage = kzalloc(sizeof(rw_page_t));         
      rwpage->pa = addr;
      addr += PGSIZE;
      rwpage->pg_id = begin_id++;

      if(rwlist->head == NULL){
        rwlist->head = rwpage;
        rwlist->tail = rwpage;
      }
      else{
        rwlist->tail->next = rwpage;
        rwlist->tail = rwpage;
      }
    }

    write_pages(vma->map_entry, rwlist);

    rest_cnts -= cnts;
  }
}

zone_t *zone = &memory_zone;

/**
 * 仔细想想，使用递归在系统中不是一个很好的编程方式，如果规模很大，可能会爆栈？
 * @param page_head 
 * @param node 
 * @param height 
 */
void __get_all_putable_pages_in_pagecache(list_head_t *page_head, radix_tree_node_t *node, int height){
  if(height == 0){
    if(!node)
      return;

    page_t *page = (page_t *)node;

    /* 在这里锁住页，那么解锁后其他进程不是又可以获取该页了吗？但是此时页已经被释放了，再去获取是错误的，
    从这个角度看，锁好像没用 */
    if(!TestSetPageLocked(page) && !PageWriteback(page)){
      if(page_mapped(page)){
        /*forget the below line, 通过swap in换入的页在swap文件中，
          且有映射，所以这里会返回，但是没有清掉locked bit*/
        ClearPageLocked(page);
        return;
      }

      /* 别忘了这一步 */
      if(PageLRU(page))
        del_page_from_lru_list(page);
      INIT_LIST_HEAD(&page->lru);

      list_add_tail(&page->lru, page_head);
      page = NULL;
    }
  }
  else{
    for(int i = 0; i < RADIX_TREE_MAP_SIZE; i++){
      if(node->slots[i]){
        __get_all_putable_pages_in_pagecache(page_head, node->slots[i], height-1);
      }
    }
  }
}

/**
 * 得到一个pageecache中所有可释放页的链表，从lru上取下来，并使用page的lru字段串联起来，上锁。
 * 不要取下已经映射过的页，上锁的页，正在写回的页。
 * 
 */
void get_all_putable_pages_in_pagecache(entry_t *entry, list_head_t *page_head){
  radix_tree_root_t *root = &entry->i_mapping->page_tree;

  __get_all_putable_pages_in_pagecache(page_head, root->rnode, root->height);
  return;
}

/* 把所有的页都移除entry的pagecache，释放页 */
void remove_put_pages_in_pagecache(entry_t *entry){
  /*遍历并删除链表节点，因为链表不能直接删除当前节点cur，
    所以这里使用了cur和prev，但是感觉这种方式不好*/
  page_t *cur_page;
  page_t *prev_page = NULL;
  LIST_HEAD(page_head);

  get_all_putable_pages_in_pagecache(entry, &page_head);

  if(list_empty(&page_head))
    return;

  /* 从shrink_list中学来，把链表中的元素取下来操作 */
//   while(!list_empty(&page_head)){
// 
//   }

  list_for_each_entry(cur_page, &page_head, lru){
    if(prev_page){
      list_del(&prev_page->lru);
      free_one_pagecache_page(prev_page);
    }

    if(!PageLocked(cur_page) || PageWriteback(cur_page)) 
      ER();

#ifdef CHECK_BOUNDARY
    if(page_mapped(cur_page))
      ER();
#endif

    remove_from_page_cache(cur_page);
    if(!TestSetPageLocked(cur_page))
      ER();

    /* 一边遍历一边修改链表 */
    /* can't put cur_page in current loop, because put page will release it's lru, which we need 
      to find the next page */
    prev_page = cur_page;
  }

  if(prev_page){
    list_del(&prev_page->lru);
    free_one_pagecache_page(prev_page);
  }

#ifdef CHECK_BOUNDARY
  /* 链表没有释放完，说明释放错误 */
  if(!list_empty(&page_head))
    ER();
#endif
}