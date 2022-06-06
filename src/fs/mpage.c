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
 * @brief 这个文件里定义了关于页page的操作函数
 * 
 */


/**
 * @brief 得到bio结构体，记录了此次读操作涉及到的sector信息。
 * 
 * @param entry the entry of the file to read
 * @param buff the physical address of the page to store the result
 * @param flpgnum the index of page in the file
 * @return int 
 */
int readpage(entry_t *entry, uint64 buff, uint32 flpgnum){
  
  // bio_t *bio = do_readpage(entry, buff, flpgnum);
  bio_t *bio = get_rw_pages_bio(entry, buff, flpgnum, 1, READ);
  if(bio)
    submit_bio(bio);
  
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

// /**
//  * @brief 找出所有要读的磁盘块号，记录在bio中，把一个页(page)按照扇区(sector)连续的原则分为段(segment)
//  * 
//  */
// bio_t *do_readpage(entry_t *entry, uint64 buff, uint32 flpgnum){
//   bio_t *bio = kzalloc(sizeof(bio_t));
//   struct bio_vec *first_bio_vec;
//   /* 用来统计扇区总数是否符合条件 */
//   // int sect_num = 0;
//   uint32 bps = entry->fat->bytes_per_sec;

//   first_bio_vec = fat_get_sectors(entry->fat, entry->clus_start, flpgnum*PGSIZE, PGSIZE);
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
  return bio;
}

/**
 * @brief get dirty pages from mapping, and get corresponding sectors,
 * write back to disk.
 * 
 * @param mapping 
 * @return int 
 */
int mpage_writepages(address_space_t *mapping){
  entry_t *entry = mapping->host;
  pages_be_found_head_t *pg_head;
  pages_be_found_t *cur_page, *next_page;
  uint32_t nr_continuous_pages;
  

  pg_head = find_pages_tag(mapping, PAGECACHE_TAG_DIRTY);
  /* no page in mapping is dirty */
  if(pg_head->head == NULL)
    return 0;
  /** 
   * 合并pg_id连续的页, 一批连续的页调用一次get_sectors, 这样可以使得得到的一个bio_vec
   * 包含的sectors尽可能多。因为在lookup_tag递归查询时，是按照页index递增的顺序查询的，所以
   * 在pg_head链表中的页index是递增的。
   * 
   * 补充：合并的要求比较严格，不仅要求页的pg_id是连续的，还要要求页的pa是连续的才行，要求比较严格。
   */
  cur_page = pg_head->head;
  while(cur_page){
    nr_continuous_pages = 1;

    /* find max counts of continuous page */
    next_page = cur_page->next;
    pages_be_found_t *tmp = cur_page;
    while(next_page){
      if((next_page->pg_id == tmp->pg_id+1) && (next_page->pa = tmp->pa + PGSIZE)){
        nr_continuous_pages++;
        next_page = next_page->next;
        tmp = tmp->next;
      }
      else
        break;
    }

    bio_t *bio = get_rw_pages_bio(entry, cur_page->pa, cur_page->pg_id, nr_continuous_pages, WRITE);

    submit_bio(bio);

    cur_page = next_page;
  }
  return 0;

}