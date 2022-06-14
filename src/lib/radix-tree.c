// #include "types.h"
// #include "atomic/sleeplock.h"
// #include "atomic/atomic.h"
// #include "fs/fat.h"
// #include "fs/stat.h"
// #include "param.h"
// #include "radix-tree.h"
// #include "mm/alloc.h"

// #include "debug.h"
#include "atomic/sleeplock.h"
#include "atomic/spinlock.h"
#include "defs.h"
#include "fs/fcntl.h"
#include "fs/file.h"
#include "fs/fs.h"
#include "fs/stat.h"
#include "kernel/proc.h"
#include "mm/vm.h"
#include "param.h"
#include "riscv.h"
#include "types.h"
#include "debug.h"
#include "utils.h"
#include "mm/mm.h"
#include "radix-tree.h"
#include "debug.h"
#include "bitops.h"

#include "fs/mpage.h"
//基数树(radix tree, rdt)相关

// struct radix_tree_node {
//   uint8 height;
//   uint8 count;
//   void *slots[RADIX_TREE_MAP_SIZE];
//   uint64 tags[RADIX_TREE_MAX_TAGS];
// };



/** 根据rdt的高度返回其最大的id值。注意height为0，最大id不为0，
 * id为0，则至少有一个节点，高度至少为1。所以
 * return (1<<(height * RADIX_TREE_MAP_SHIFT)) - 1;
 * 的表达是错的(要排除掉height为0的情况）。前面使用这个表达式，把pa存在了rdt root指向node的指针中
 * 歪打正着也能读出来。
 * 如果height为0选择返回一个负值，还要考虑有符号数和无符号数比较时的转化，所以我们选择特殊处理height=0
 * 的情况。
 */
uint64
radix_tree_maxindex(uint height){
	if(height == 0)
		return 0;
		// panic("rdt maxindex: height is 0!");
		// return -1;
  return (1<<(height * RADIX_TREE_MAP_SHIFT)) - 1;
}

/**
 *	radix_tree_lookup    -    perform lookup operation on a radix tree
 *	@root:		radix tree root
 *	@index:		index key
 *
 *	Lookup them item at the position @index in the radix tree @root.
 */
void *radix_tree_lookup(struct radix_tree_root *root, unsigned long index)
{
	unsigned int height, shift;
	struct radix_tree_node **slot;

	height = root->height;
	/* 当要查的页号大于当前rdt所能表示的最大页号，返回null，在写时可能会出现 */
	if (index > radix_tree_maxindex(height))
		return NULL;

	// printf(rd("index: %d height: %d\n"), index, root->height);
	
	shift = (height-1) * RADIX_TREE_MAP_SHIFT;
	slot = &root->rnode;

	while (height > 0) {
		if (*slot == NULL)
			return NULL;

		slot = (struct radix_tree_node **)
			((*slot)->slots + ((index >> shift) & RADIX_TREE_MAP_MASK));
		shift -= RADIX_TREE_MAP_SHIFT;
		height--;
	}

  /* we get a pointer to struct page or null. */
	return (void *) *slot;
}

/**
 *	radix_tree_insert    -    insert into a radix tree
 *	@root:		radix tree root
 *	@index:		index key
 *	@item:		item to insert
 *
 *	Insert an item into the radix tree at position @index.
 */
int radix_tree_insert(struct radix_tree_root *root, unsigned long index, void *item)
{
	struct radix_tree_node *node = NULL, *tmp, **slot;
	uint32 height, shift;
	// int error;


	/* make sure the tree is high enough */
	if(root->height == 0 || index > radix_tree_maxindex(root->height)){
		if(radix_tree_extend(root, index))	
			panic("rdt insert 1");
	}
	slot = &root->rnode;
	height = root->height;
	shift = (height-1) * RADIX_TREE_MAP_SHIFT;	

	while (height > 0)
	{
		if(*slot == NULL){
			/* add a child node */
			if(!(tmp = radix_tree_node_alloc())){
				panic("rdt insert 2");
			}
			*slot = tmp;
			if(node)
				node->count++;
		}

		/* Go a level down. */
		node = *slot;
		slot = (struct radix_tree_node **)
						(node->slots + ((index >> shift) & RADIX_TREE_MAP_MASK));
		shift -= RADIX_TREE_MAP_SHIFT;
		height--;
	}
	/* slot should be null 当打印之后，*slot就不为0了，很奇怪, 莫非分配节点时应该把指针初始化为NULL？*/
	if(*slot != NULL)
		panic("rdt insert 3");
	if(node)
		node->count++;

	*slot = item;	
	
	return 0;
}

/**
 * @brief Set the extend node tags object
 * the child is parent->slots[0] when extend a tree.
 * 
 * @param parent 
 * @param child 
 */
static void set_extend_node_tags(radix_tree_node_t *parent, radix_tree_node_t *child){
	for(int i = 0; i < RADIX_TREE_MAX_TAGS; i++){
		for(int j = 0; j < RADIX_TREE_TAG_LONGS; i++){
			if(child->tags[i][j] > 0){
				parent->tags[i][0] = 1;
				break;
			}
		}
	}	
}

/*
 *	Extend a radix tree so it can store key @index.
 */
static int radix_tree_extend(struct radix_tree_root *root, unsigned long index)
{
	struct radix_tree_node *node;
	unsigned int height;

	/* Figure out what the height should be.  */
	height = root->height + 1;
	while (height == 0 || index > radix_tree_maxindex(height))
		height++;
	
	/* 如果root->rnode为NULL，在插入函数的while循环中会分配 */
	if (root->rnode) {
		do {
			if (!(node = radix_tree_node_alloc()))
				panic("rdt extend");
				// return -ENOMEM;

			/* Increase the height.  */
			node->slots[0] = root->rnode;
			node->count = 1;
			root->rnode = node;
			root->height++;
			set_extend_node_tags(node, node->slots[0]);
		} while (height > root->height);
	} else{
		// for(;;);
		root->height = height;
	}
	return 0;
}

static struct radix_tree_node *
radix_tree_node_alloc(){
	struct radix_tree_node *ret;
	ret = kzalloc(sizeof(struct radix_tree_node));
	if(ret == NULL)
		panic("rdt alloc");
	
	/* 这里需要初始化一下内容，否则似乎里面不为0 */
	ret->count = 0;
	for(int i = 0; i < RADIX_TREE_MAP_SIZE; i++)
		ret->slots[i] = NULL;
	return ret;
}


static inline void tag_set(radix_tree_node_t *node, uint32_t tag_type, int offset){
	__set_bit(offset, node->tags[tag_type]);
}

static inline void tag_clear(struct radix_tree_node *node, unsigned int tag,
		int offset)
{
	__clear_bit(offset, node->tags[tag]);
}

static inline int tag_get(struct radix_tree_node *node, unsigned int tag, int offset)
{
	return test_bit(offset, node->tags[tag]);
}

/*
 * Returns 1 if any slot in the node has this tag set.
 * Otherwise returns 0.
 */
static inline int any_tag_set(struct radix_tree_node *node, unsigned int tag)
{
	int idx;
	for (idx = 0; idx < RADIX_TREE_TAG_LONGS; idx++) {
		if (node->tags[tag][idx])
			return 1;
	}
	return 0;
}


void radix_tree_tag_set(radix_tree_root_t *root, uint64_t pg_id, uint tag_type){
	uint height, shift;
	radix_tree_node_t *slot;

	height = root->height;
	if(pg_id > radix_tree_maxindex(height))
		panic("rdt tag set");

	shift = (height-1)*RADIX_TREE_MAP_SHIFT;
	slot = root->rnode;
	
	while (height > 0)
	{
		int slot_num;
		if(slot == NULL)
			panic("rdt tag set");
			// return;
		
		slot_num = (pg_id >> shift) & RADIX_TREE_MAP_MASK;
		tag_set(slot, tag_type, slot_num);
		slot = (radix_tree_node_t *)(slot->slots[slot_num]);
		shift -= RADIX_TREE_MAP_SHIFT;
		height--;
	}

	/* the index of page to set is empty */
	if(slot == NULL)
		panic("rdt tag set 2");

	return;
}


void radix_tree_tag_clear(radix_tree_root_t *root, uint64_t pg_id, uint tag_type){
	/*
	 * The radix tree path needs to be one longer than the maximum path
	 * since the "list" is null terminated.
	 * path[0]作为结束符？
	 */
	radix_tree_path_t path[RADIX_TREE_MAX_PATH + 1], *pathp = path;
	radix_tree_node_t *slot = NULL;
	uint32_t height, shift;

	height = root->height;
	if(pg_id > radix_tree_maxindex(height))
		panic("rdt tag clear");

	shift = (height - 1) * RADIX_TREE_MAP_SHIFT;
	pathp->node = NULL;
	slot = root->rnode;

	while (height > 0)
	{
		int offset;

		if(slot == NULL)
			goto out;
		
		offset = (pg_id >> shift) & RADIX_TREE_MAP_MASK;
		pathp[1].offset = offset;
		pathp[1].node = slot;

		slot = slot->slots[offset];
		pathp++;
		shift -= RADIX_TREE_MAP_SHIFT;
		height--;
	}

	if (slot == NULL)
	{
		goto out;
	}
	
	while(pathp->node) {
		if(!tag_get(pathp->node, tag_type, pathp->offset))
			goto out;
		tag_clear(pathp->node, tag_type, pathp->offset);
		if(any_tag_set(pathp->node, tag_type))
			goto out;
		pathp--;
	}

out:
	return;
}

/**
 * @brief recursive lookup tagged page
 * 
 */
void lookup_tag(radix_tree_node_t *node, uint32_t tag, pages_be_found_head_t *pg_head, int height, uint64_t pg_id_base){
	int i, shift;
	shift = (height-1)*RADIX_TREE_MAP_SHIFT;

	for(i=0; i<RADIX_TREE_MAP_SIZE; i++){
		if(tag_get(node, tag, i)){
			if(height == 1){
				pages_be_found_t *page = kzalloc(sizeof(pages_be_found_t));

				page->pa = (uint64_t)node->slots[i];
				page->pg_id = pg_id_base + i;

				if(pg_head->head == NULL && pg_head->tail == NULL){
					pg_head->head = page;
					pg_head->tail = page;
				}
				else{
					pg_head->tail->next = page;
					pg_head->tail = page;
				}

				pg_head->nr_pages++;
			}
			else{

				lookup_tag((radix_tree_node_t *)node->slots[i], tag, pg_head, height - 1, pg_id_base + i * (1<<shift));
			}
		}
	}
}



/**
 * @brief find all pages with tag in the rdt
 * 
 * @param mapping 
 * @param tag 
 * @param pg_head 
 * @return pages_be_found_head_t* 
 */
pages_be_found_head_t *
radix_tree_find_tags(radix_tree_root_t *root, uint32_t tag, pages_be_found_head_t *pg_head){
	/* 打印出来是2，赋值为3试一试。*/
	// root->rnode->tags[0][0] = 3;
	lookup_tag(root->rnode, tag, pg_head, root->height, 0);
	return pg_head;
}