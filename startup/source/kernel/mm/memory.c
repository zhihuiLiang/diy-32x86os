#include "mm/memory.h"

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
            (*page_e) += addr + PAGE_ENTRY_WE_ATTR;
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

// 复制页面
#define copy_page(from, to) __asm__("cld ; rep ; movsl" ::"S"(from), "D"(to), "c"(1024) : "cx", "di", "si")

// 刷新页变换高速缓冲。
// 为了提高地址转换的效率，CPU 将最近使用的页表数据存放在芯片中高速缓冲中。在修改过页表
// 信息之后，就需要刷新该缓冲区。这里使用重新加载页目录基址寄存器 cr3 的方法来进行刷新。
#define invalidate() __asm__("movl %%eax,%%cr3" ::"a"(0))

/**
 * @brief 复制进程的页目录页表
 *
 * @param from 源地址的内存偏移
 * @param to 目的地址的内存偏移
 * @param size 需要复制的内存大小
 * @return int
 */
int copy_page_tables(unsigned long from, unsigned long to, long size) {
    // 进程1创建时 from = 0, to = 64M，  size = 640k或160个页面
    unsigned long* from_page_table;
    unsigned long* to_page_table;
    unsigned long this_page;
    unsigned long *from_dir, *to_dir;
    unsigned long nr;

    if((from & 0x3fffff) || (to & 0x3fffff))  // 源地址和目的地址都需要是 4Mb 的倍数。否则出错，死机
        panic("copy_page_tables called with wrong alignment");
    // 取得源地址和目的地址的目录项(from_dir 和 to_dir)	 分别是0和64
    from_dir = (unsigned long*)((from >> 20) & 0xffc); /* _pg_dir = 0 */
    to_dir = (unsigned long*)((to >> 20) & 0xffc);
    size =
        ((unsigned)(size + 0x3fffff)) >> 22;  // 计算要复制的内存块占用的页表数  4M（1个页目录项管理的页面大小 = 1024*4K）的数量
    for(; size-- > 0; from_dir++, to_dir++) {
        if(1 & *to_dir)  // 如果目的目录项指定的页表已经存在，死机
            panic("copy_page_tables: already exist");
        if(!(1 & *from_dir))  // 如果源目录项未被使用，不用复制，跳过
            continue;
        from_page_table = (unsigned long*)(0xfffff000 & *from_dir);  // 从页目录项中取出页表项
        if(!(to_page_table = (unsigned long*)get_free_page()))  // // 为目的页表取一页空闲内存. 关键！！是目的页表存储的地址
            return -1;                                          /* Out of memory, see freeing */
        *to_dir = ((unsigned long)to_page_table) | 7;  // 设置目的目录项信息。7 是标志信息，表示(Usr, R/W, Present)
        nr = (from == 0) ? 0xA0 : 1024;  // 如果是进程0复制给进程1，则复制160个页面；否则将1024个页面全部复制
        for(; nr-- > 0; from_page_table++, to_page_table++) {
            this_page = *from_page_table;  // 复制！
            if(!(1 & this_page))
                continue;
            this_page &= ~2;             // 010， 代表用户，只读，存在
            *to_page_table = this_page;  // 复制！
            if(this_page > LOW_MEM) {  // 如果该页表项所指页面的地址在 1M 以上，则需要设置内存页面映射数组 mem_map[]
                *from_page_table = this_page;
                this_page -= LOW_MEM;
                this_page >>= 12;
                mem_map[this_page]++;
            }
        }
    }
    invalidate();  // 刷新页变换高速缓冲
    return 0;
}

//// 取空闲页面。如果已经没有内存了，则返回 0。
// 输入：%1(ax=0) - 0；%2(LOW_MEM)；%3(cx=PAGING PAGES)；%4(di=mem_map+PAGING_PAGES-1)。
// 输出：返回%0(ax=页面号)。
// 从内存映像末端开始向前扫描所有页面标志（页面总数为 PAGING_PAGES），如果有页面空闲（对应
// 内存映像位为 0）则返回页面地址。
uint32_t get_free_page(void) {
    // 在mem_map 中反向扫描得到空闲页面，故从16M高地址开始分配页面
    register unsigned long __res asm("ax");

    __asm__("std ; repne ; scasb\n\t"
            "jne 1f\n\t"
            "movb $1,1(%%edi)\n\t"
            "sall $12,%%ecx\n\t"
            "addl %2,%%ecx\n\t"
            "movl %%ecx,%%edx\n\t"
            "movl $1024,%%ecx\n\t"
            "leal 4092(%%edx),%%edi\n\t"
            "rep ; stosl\n\t"
            "movl %%edx,%%eax\n"
            "1:"
            : "=a"(__res)
            : "0"(0), "i"(LOW_MEM), "c"(PAGING_PAGES), "D"(mem_map + PAGING_PAGES - 1));
    return __res;
}

void free_page(unsigned long addr) {
    if(addr < LOW_MEM)
        return;
    if(addr >= HIGH_MEMORY)
        panic("trying to free nonexistent page");
    addr -= LOW_MEM;  // 物理地址-低端内存位置，再除以 4KB，得页面号
    addr >>= 12;
    if(mem_map[addr]--)
        return;  // 如果对应内存页面映射字节不等于 0，则减 1 返回
    mem_map[addr] = 0;
    // panic("trying to free free page");
}

/**
 * @brief 释放页表连续的内存块
 *
 * @param from 起始基地址
 * @param size 释放的长度
 * @return int 0-成功
 */
int free_page_tables(unsigned long from, unsigned long size) {
    unsigned long* pg_table;
    unsigned long *dir, nr;

    if(from & 0x3fffff)
        panic("free_page_tables called with wrong alignment");
    if(!from)
        panic("Trying to free up swapper memory space");
    size = (size + 0x3fffff) >> 22;
    dir = (unsigned long*)((from >> 20) & 0xffc); /* _pg_dir = 0 */
    for(; size-- > 0; dir++) {
        if(!(1 & *dir))
            continue;
        pg_table = (unsigned long*)(0xfffff000 & *dir);
        for(nr = 0; nr < 1024; nr++) {
            if(1 & *pg_table)
                free_page(0xfffff000 & *pg_table);
            *pg_table = 0;
            pg_table++;
        }
        free_page(0xfffff000 & *dir);
        *dir = 0;
    }
    invalidate();  // 刷新页变换高速缓冲
    return 0;
}