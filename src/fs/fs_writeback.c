#include "common.h"
#include "platform.h"
#include "mm/alloc.h"
#include "fs/fs.h"
#include "fs/file.h"
#include "mm/vm.h"
#include "kernel/proc.h"
#include "defs.h"
#include "mm/page.h"
#include "radix-tree.h"
// #include "mm/.h"

#include "fs/mpage.h"
#include "writeback.h"

#define QUIET
#define __MODULE_NAME__ WB
#include "debug.h"

extern fat32_t *fat;

extern void buddy_print_free();

/**
 * 只写回，不释放pagecache。
 */
void writeback_single_entry(entry_t *entry){
  /* wbc暂时无用 */
  // wbc = NULL;
  if(!EntryDirty(entry))
    return;

  /* 上层调用函数注意检查entry是否已经在写回状态了。*/
  if(TestSetEntryWriteback(entry))
    ER();

  // printf(ylw("begin write pages!"));
  debug(ylw("begin write pages!"));
  sych_entry_in_disk(entry);
  // mpage_writepages(mapping);
  debug(ylw("end write pages!\n"));
  // printf(ylw("end write pages!\n"));

    // list_del(&entry->e_list); 在运行lat_fs时错误
  // list_del_init(&entry->e_list);
  ClearEntryDirty(entry);
  ClearEntryWriteback(entry);
  // buddy_print_free();  
}

extern entry_t pool[NENTRY];

void writeback_single_entry_idx(uint64_t idx){
  writeback_single_entry(&pool[idx]); 
}

extern void remove_put_pages_in_pagecache(entry_t *entry);
/**
 * @brief 我们的文件系统中没有struct inode，对应的数据结构是struct fat_entry
 * linux 中的原型函数为writeback_inodes
 */
void
writeback_entrys_and_shrink_pagecache(struct writeback_control *wbc){
  /* 不仅是dirty的entry占据pagecache，可执行文件也会 */
  // while(!list_empty(&fat->fat_dirty)){
  //   entry_t *entry = list_entry(fat->fat_dirty.prev, entry_t, e_list);
  //   // address_space_t *mapping = entry->i_mapping;

  //   /* 只设置一个pdfulsh，不用考虑两条pdflush并发问题 */
  //   // acquiresleep(&entry->lock);
  //   writeback_single_entry(entry);
  //   // releasesleep(&entry->lock);

  //   // ERROR("can not free mapping ,we can only free pages!");
  //   // free_mapping(entry);
  //   /* 把pages从pagecache中移除，保留i_mapping */
  //   remove_put_pages_in_pagecache(entry);
  //   // list_del(&entry->e_list);
  // }
}
