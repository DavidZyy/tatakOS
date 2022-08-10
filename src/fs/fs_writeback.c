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
// #include "mm/.h"

#include "fs/mpage.h"
#include "writeback.h"

extern fat32_t *fat;

extern void buddy_print_free();

void writeback_single_entry(entry_t *entry){
  /* wbc暂时无用 */
  // wbc = NULL;
  if(!entry->dirty)
    return;

  printf(ylw("begin write pages!\n"));
  sych_entry_in_disk(entry);
  // mpage_writepages(mapping);
  printf(ylw("end write pages!\n"));

  printf(ylw("begin free mapping!\n"));
  /* 这里不对，写回不意味着要回收 */
  #ifdef TODO
  todo("");
  #endif
  // free_mapping(entry);
  printf(ylw("end free mapping!\n"));

  list_del(&entry->e_list);
  // buddy_print_free();  
}

extern entry_t pool[NENTRY];

void writeback_single_entry_idx(uint64_t idx){
  writeback_single_entry(&pool[idx]); 
}

/**
 * @brief 我们的文件系统中没有struct inode，对应的数据结构是struct fat_entry
 * linux 中的原型函数为writeback_inodes
 */
void
writeback_entrys_and_free_mapping(struct writeback_control *wbc){
  /* 不仅是dirty的entry占据pagecache，可执行文件也会 */
  while(!list_empty(&fat->fat_dirty)){
    entry_t *entry = list_entry(fat->fat_dirty.prev, entry_t, e_list);
    // address_space_t *mapping = entry->i_mapping;

    /* 只设置一个pdfulsh，不用考虑两条pdflush并发问题 */
    acquiresleep(&entry->lock);
    writeback_single_entry(entry);
    releasesleep(&entry->lock);

    free_mapping(entry);

    // list_del(&entry->e_list);
  }
}
