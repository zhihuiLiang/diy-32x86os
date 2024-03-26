#ifndef MM_H
#define MM_H

#include "comm/cpu_instr.h"
#include "comm/types.h"
#include "errno.h"

#define PAGE_DIR_ADDR 0      // 页目录首地址
#define PAGE_ENTRY_NUM 1024  // 一个页表含有的页表项
#define PAGE_TABLE_NUM 5  // 本项目的内存为128M，一个页表可寻址1024 * 4KB， 32个页表即可寻址全部内存区域
#define PAGE_SIZE (PAGE_ENTRY_NUM * 4)        // 每一个页表的大小，单位字节
#define PAGE_TABLE_ADDR (PAGE_ENTRY_NUM * 4)  // 页表首地址

#define SET_PAGE_D (1 << 6)  // 页面已修改
#define SET_PAGE_A (1 << 5)  // 页面已访问
#define SET_PAGE_U (1 << 2)  // 页面可被用户态访问
#define SET_PAGE_W (1 << 1)  // 页面是否可写
#define SET_PAGE_P (1 << 0)  // 页面是否存在

#define PAGE_ENTRY_WE_ATTR 0x07  // 页面存在且可读写标志

#define LOW_MEM 0x100000                       // 内存低端（1M）
#define PAGING_MEMORY (15 * 1024 * 1024)       // 分页内存 15M
#define PAGING_PAGES (PAGING_MEMORY >> 12)     // 0xf00 = 3840  分页后的页数
#define MAP_NR(addr) (((addr)-LOW_MEM) >> 12)  // 计算指定地址的页面号
#define USED 100

extern uint32_t const* pg_dir;

void init_page_table();
void mem_init(uint32_t start_mem, uint32_t end_mem);
uint32_t get_free_page();
int copy_page_tables(unsigned long from, unsigned long to, long size);
void free_page(unsigned long addr);
int free_page_tables(unsigned long from, unsigned long size);

#endif  //