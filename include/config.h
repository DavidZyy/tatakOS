// 内核配置文件，根据配置选择是否启用某些功能

/****************功能*****************/
/* 是否开启反向映射，因为无论一个page map几次，nr_mapped都算1，
  所以在rmap中统计nr_mapped，关闭会导致数据统计不准确 */
#define RMAP
/* 是否开启swap，如果开启swap，需要先开启rmap(SWAP是建立在RMAP的基础上的，开启了swap，就默认开启了rmap) */
#define SWAP

/***************参数****************/
/* 最大readahead页数占当前空余内存的页数，百分率(%) */
#define READ_AHEAD_RATE 10

/* 可使用的内存百分率(%)(减去预留的内存比率) */
#define AVAILABLE_MEMORY_RATE 95

/* 开启check boundary的情况下，会对一些边界值做更为严格的检查，用于debug，关闭后忽略掉一些
  非必要的边界值问题 */
#define CHECK_BOUNDARY

/* 当entry占有率超过90%， 接下来__eput时释放entry的impping */
#define FREE_ENTRY_MAPPING_RATE 90