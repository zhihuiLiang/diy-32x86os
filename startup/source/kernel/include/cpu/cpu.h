#ifndef CPU_H
#define CPU_H

#include "comm/types.h"

#include "comm/cpu_instr.h"
#include "os_cfg.h"

#pragma pack(1)
typedef struct _segment_desc_t {
    uint16_t limit15_0;
    uint16_t base15_0;
    uint8_t base23_16;
    uint16_t attr;
    uint8_t base31_24;
} segment_desc_t;

#pragma pack()

#define SEG_G (1 << 15)
#define SEG_D (1 << 14)
#define SEG_P_PRESENT (1 << 7)  // 设置P 段存在标志位

#define SEG_DPL0 (0 << 5)      // 设置特权级0
#define SEG_DPL1 (3 << 5)      // 设置特权级3
#define SEG_S_SYSTEM (0 << 4)  // 设置为系统段
#define SEG_S_NORMAL (1 << 4)  // 设置为普通段

#define SEG_TYPE_CODE (1 << 3)  // 设置代码段
#define SEG_TYPE_DATA (0 << 3)  // 设置数据段

#define SEG_TYPE_RW (1 << 1)  // 设置读写权限
#define SEG_TYPE_A (1 << 0)   // 设置已访问

extern segment_desc_t gdt_table[GDT_TABLE_SIZE];

void segment_desc_set(int selector, uint32_t base, uint32_t limit, uint16_t attr);
void init_gdt();

static inline uint32_t get_base(segment_desc_t* seg) {
    return seg->base15_0 + (seg->base23_16 << 16) + (seg->base31_24 << 24);
}

static inline void set_base(segment_desc_t* seg, uint32_t base) {
    seg->base15_0 = base & 0xFFFF;
    seg->base23_16 = (base >> 16) & 0xFF;
    seg->base31_24 = (base >> 24) & 0xFF;
}

#endif