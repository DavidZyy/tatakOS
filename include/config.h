// 内核配置文件，根据配置选择是否启用某些功能

/****************功能*****************/
/* 是否开启反向映射 */
#define RMAP
/* 是否开启swap，如果开启swap，需要先开启rmap(SWAP是建立在RMAP的基础上的，开启了swap，就默认开启了rmap) */
#define SWAP

/* 把页映射到pagecache上，按需加载，占用内存更小，但是速度慢 */
// #define LAZY_LOAD

/***************参数****************/
/* 最大readahead页数占当前空余内存的页数 */
#define READ_AHEAD_RATE 10

/* 可使用的内存比率(减去预留的内存比率) */
#define AVAILABLE_MEMORY_RATE 95