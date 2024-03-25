#include "mm/mm.h"

uint32_t const* pg_dir = PAGE_DIR_ADDR;
static uint32_t HIGH_MEMORY = 0;
static unsigned char mem_map[PAGING_PAGES] = {
    0,
};

void init_page_table() {
    // 初始化页目录表
    uint32_t* page_e = PAGE_DIR_ADDR;
    for(int i = 0; i < PAGE_ENTRY_NUM; ++i) {
        *page_e = 0;
        if(i < PAGE_TABLE_NUM) {
            uint32_t addr = PAGE_TABLE_ADDR + i * PAGE_ENTRY_NUM * 4;
            (*page_e) += (addr << 12) + PAGE_ENTRY_WE_ATTR;
        }
        page_e++;
    }
    // 初始化页表
    for(int i = 0; i < PAGE_TABLE_NUM * PAGE_ENTRY_NUM; ++i) {
        *page_e = (i << 12) + PAGE_ENTRY_WE_ATTR;
        page_e++;
    }

    write_cr3((uint32_t)PAGE_DIR_ADDR);
    uint32_t cr0 = read_cr0();
    write_cr0(cr0 | (1 << 31));
}

void mem_init(uint32_t start_mem, uint32_t end_mem) {
    int i = 0;

    HIGH_MEMORY = end_mem;
    for(i; i < PAGING_PAGES; ++i) {
        mem_map[i] = USED;
    }
    i = MAP_NR(start_mem);
    end_mem -= start_mem;
    end_mem >> 12;
    while(end_mem-- > 0) {
        mem_map[i++] = 0;
    }
}