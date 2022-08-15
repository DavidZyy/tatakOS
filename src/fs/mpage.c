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

// int read_one_page(entry_t *entry, uint64 buff, uint32 index){ 

//   bio_t *bio = get_rw_pages_bio(entry, buff, index, 1, READ);
//   if(bio)
//     submit_bio(bio);
  
//   return 0;
// }

// /**
//  * 把一个页写回disk。
//  */
// int write_one_page(entry_t *entry, uint64_t buff, uint32_t index){
  
//   bio_t *bio = get_rw_pages_bio(entry, buff, index, 1, WRITE);
//   if(bio)
//     submit_bio(bio);
  
//   return 0;
// }
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

// /**
//  * @brief 找出所有要读的磁盘块号，记录在bio中，把一个页(page)按照扇区(sector)连续的原则分为段(segment)
//  * 
//  */
// bio_t *do_readpage(entry_t *entry, uint64 buff, uint32 index){
//   bio_t *bio = kzalloc(sizeof(bio_t));
//   struct bio_vec *first_bio_vec;
//   /* 用来统计扇区总数是否符合条件 */
//   // int sect_num = 0;
//   uint32 bps = entry->fat->bytes_per_sec;

//   first_bio_vec = fat_get_sectors(entry->fat, entry->clus_start, index*PGSIZE, PGSIZE);
//   // cur_bio_vec = first_bio_vec;
//   // while(cur_bio_vec != NULL){
//   //   cur_bio_vec->bv_buff = (void *)buff;
//   //   buff += cur_bio_vec->bv_count * bps;

//   //   cur_bio_vec = cur_bio_vec->bv_next;
//   // }
//   bio_vec_buff_assignment(first_bio_vec, buff, bps);

//   // if(PGSIZE / bps != sect_num)
//   //   panic("do_readpage: sector num is wrong!");
//   bio->bi_io_vec = first_bio_vec; 
//   bio->bi_rw = READ;
//   bio->bi_dev = entry->fat->dev;
//   // print_bio_vec(bio);
//   return bio;
// }


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
  // int sect_num = 0;
  uint32 bps = entry->fat->bytes_per_sec;

  // printf(grn("pg_id: %d\tpg_cnt: %d\n"), pg_id, pg_cnt);
  first_bio_vec = fat_get_sectors(entry->fat, entry->clus_start, pg_id*PGSIZE, pg_cnt*PGSIZE);
  // cur_bio_vec = first_bio_vec;
  // while(cur_bio_vec != NULL){
  //   cur_bio_vec->bv_buff = (void *)buff;
  //   buff += cur_bio_vec->bv_count * bps;

  //   cur_bio_vec = cur_bio_vec->bv_next;
  // }
  bio_vec_buff_assignment(first_bio_vec, buff, bps);

  // if(PGSIZE / bps != sect_num)
  //   panic("do_readpage: sector num is wrong!");
  bio->bi_io_vec = first_bio_vec; 
  bio->bi_rw = rw;
  bio->bi_dev = entry->fat->dev;

  // print_bio_vec(bio);
  // printf("\n");

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

  // rw_page_t *pg = pg_list->head;
  // while(pg){
  //   rw_page_t *tmp = pg->next;
  //   kfree((void*)pg);
  //   pg = tmp;
  // }
  kfree(pg_list);
}

// /**
//  * @brief get dirty pages from mapping, and get corresponding sectors,
//  * write back to disk.
//  * 
//  */
// int mpage_writepages(address_space_t *mapping){
//   entry_t *entry = mapping->host;
//   pages_be_found_head_t *pg_list;
//   pages_be_found_t *cur_page, *next_page;
//   uint32_t nr_continuous_pages;
  

//   // printf_radix_tree(&mapping->page_tree);

//   pg_list = find_pages_tag(mapping, PAGECACHE_TAG_DIRTY);
  
//   // print_pages_be_found(pg_list);

//   /* no page in mapping is dirty */
//   if(pg_list == NULL)
//     return 0;
//   // if(pg_list->head == NULL){
//     /* free the pg_list !!!*/
//     // kfree(pg_list);
//     // return 0;
//   // }
//   /** 
//    * 合并pg_id连续的页, 一批连续的页调用一次get_sectors, 这样可以使得得到的一个bio_vec
//    * 包含的sectors尽可能多。因为在lookup_tag递归查询时，是按照页index递增的顺序查询的，所以
//    * 在pg_head链表中的页index是递增的。
//    * 
//    * 补充：合并的要求比较严格，不仅要求页的pg_id是连续的，还要要求页的pa是连续的才行，要求比较严格。
//    */
//   cur_page = pg_list->head;
//   while(cur_page){
//     nr_continuous_pages = 1;

//     /* find max counts of continuous page */
//     next_page = cur_page->next;
//     pages_be_found_t *tmp = cur_page;
//     #ifdef TODO
//     todo("modify the allocated page address, make the later allocated one bigger than the former one");
//     #endif
//     // printf("start merge\n");
//     while(next_page){
//       if((next_page->pg_id == tmp->pg_id+1) && (next_page->pa == tmp->pa + PGSIZE)){
//         nr_continuous_pages++;
//         next_page = next_page->next;
//         tmp = tmp->next;
//       }
//       else
//         break;
//     }
//     // printf("merge end\n");
//     // printf(rd("cur_page->pa: %p\t cur_page->pg_id: %d\t nr continuous pages: %d\n"), cur_page->pa, cur_page->pg_id, nr_continuous_pages);
    
//     bio_t *bio = get_rw_pages_bio(entry, cur_page->pa, cur_page->pg_id, nr_continuous_pages, WRITE);
//     // printf("bio get\n");
//     // print_bio_vec(bio);
//     // printf("submit...\n");
//     submit_bio(bio);
//     // printf("submit end...\n");
//     cur_page = next_page;
//   }

//   /* 这里别忘了释放pghead相关的结构体！ */
//   free_rw_page_list(pg_list);
//   // pages_be_found_t *pg = pg_list->head;
//   // while(pg){
//   //   pages_be_found_t *tmp = pg->next;
//   //   kfree((void*)pg);
//   //   pg = tmp;
//   // }
//   // kfree(pg_list);

//   return 0;

// }

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
    #ifdef TODO
    todo("modify the allocated page address, make the later allocated one bigger than the former one");
    #endif
    // printf("start merge\n");
    while(next_page){
      if((next_page->pg_id == tmp->pg_id+1) && (next_page->pa == tmp->pa + PGSIZE)){
        nr_continuous_pages++;
        next_page = next_page->next;
        tmp = tmp->next;
      }
      else
        break;
    }
    // printf("merge end\n");
    // printf(rd("cur_page->pa: %p\t cur_page->pg_id: %d\t nr continuous pages: %d\n"), cur_page->pa, cur_page->pg_id, nr_continuous_pages);
    
    bio_t *bio = get_rw_pages_bio(entry, cur_page->pa, cur_page->pg_id, nr_continuous_pages, rw);
    // printf("bio get\n");
    // print_bio_vec(bio);
    // printf("submit...\n");
    submit_bio(bio);
    // printf("submit end...\n");
    cur_page = next_page;
  }

  /* 这里别忘了释放pghead相关的结构体！ */
  free_rw_page_list(pg_list, rw);
  // pages_be_found_t *pg = pg_list->head;
  // while(pg){
  //   pages_be_found_t *tmp = pg->next;
  //   kfree((void*)pg);
  //   pg = tmp;
  // }
  // kfree(pg_list);

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

    write_pages(vma->map_file->ep, rwlist);

    rest_cnts -= cnts;
  }
}

zone_t *zone = &memory_zone;

void __get_all_putable_pages_in_pagecache(list_head_t *page_head, radix_tree_node_t *node, int height){
// void __get_all_putable_pages_in_pagecache(entry_t *entry){
  if(height == 0){
    page_t *page = (page_t *)node;

    /* 在这里锁住页，那么解锁后其他进程不是又可以获取该页了吗？但是此时页已经被释放了，再去获取是错误的，
    从这个角度看，锁好像没用 */
    if(!TestSetPageLocked(page) && !PageWriteback(page)){
#ifdef RMAP
      if(page_mapped(page))
        return;
#endif
      /* 别忘了这一步 */
      // list_del_init(&page->lru);
      // TestClearPageLRU(page);
      /* put_page也会把page从lru上取下，这里为了利用page的链表，提前取下 */
      spin_lock(&zone->lru_lock);
      if(TestClearPageLRU(page))
          del_page_from_lru(zone, page);
      spin_unlock(&zone->lru_lock);
      INIT_LIST_HEAD(&page->lru);

      list_add_tail(&page->lru, page_head);
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
  // // address_space_t *mapping = entry->i_mapping;
  // // rw_page_list_t *pg_list;
  // // rw_page_t *rm_page;

  // // /* 找到entry所有的dirty page */
  // // pg_list = find_pages_tag(mapping, PAGECACHE_TAG_DIRTY);


  // // if(pg_list == NULL)
  // //   return;
  
  // // for(rm_page = pg_list->head; rm_page; rm_page=rm_page->next){
  // //   page_t *page = PATOPAGE(rm_page->pa);
  // //   /* 映射过的页通过页回收算法回收 */
  // //   if(page_mapped(page))
  // //     continue;
  // //   if(TestSetPageLocked(page))
  // //     continue;
  // //   remove_from_page_cache(page);
  // //   /* page已经被释放掉了，unlock_page唤醒似乎也无效了 */
  // //   if (!TestClearPageLocked(page))
  // //     ER();
  // //   put_page(page);
  // // }

  // free_rw_page_list(pg_list, READ);
  page_t *cur_page;
  page_t *prev_page = NULL;
  LIST_HEAD(page_head);

  get_all_putable_pages_in_pagecache(entry, &page_head);

  if(list_empty(&page_head))
    return;


  list_for_each_entry(cur_page, &page_head, lru){
    if(prev_page){
      list_del(&prev_page->lru);
      put_page(prev_page);
    }

    if(!PageLocked(cur_page) || PageWriteback(cur_page)) 
      ER();

#ifdef RMAP
    if(page_mapped(cur_page))
      ER();
#endif

    remove_from_page_cache(cur_page);
    if(!TestSetPageLocked(cur_page))
      ER();

    // /* 一边遍历一边修改链表 */
    // list_del(&cur_page->lru);
    // put_page(cur_page);
    /* can't put cur_page in current loop, because put page will release it's lru, which we need 
      to find the next page */
    prev_page = cur_page;
  }

  list_del(&prev_page->lru);
  put_page(prev_page);

}