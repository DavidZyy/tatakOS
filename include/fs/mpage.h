#ifndef MPAGE_H
#define MPAGE_H

#include "common.h"
#include "fs/fs.h"

bio_t *get_rw_pages_bio(entry_t *entry, uint64 buff, uint32 pg_id, int pg_cnt, int rw);
int rw_pages(entry_t *entry, rw_page_list_t *pg_list, int rw);
int rw_one_page(entry_t *entry, page_t *page, uint32 index, int rw);

#define read_pages(entry, pg_list) rw_pages(entry, pg_list, READ)
#define write_pages(entry, pg_list) rw_pages(entry, pg_list, WRITE)

#define read_one_page(entry, page, index) rw_one_page(entry, page, index, READ)
#define write_one_page(entry, page, index) rw_one_page(entry, page, index, WRITE)


#endif