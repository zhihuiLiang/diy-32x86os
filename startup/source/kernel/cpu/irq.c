#include "cpu/irq.h"

#include "comm/cpu_instr.h"
#include "os_cfg.h"

static gate_desc_t idt_table[IDE_TABLE_NR];

void irq_init(void) {
    for (int i = 0; i < IDE_TABLE_NR; i++) {
        gate_desc_set(idt_table + i, KERNEL_SELECTOR_CS, 0,
                      GATE_P_PRESENT | GATE_DPL0 | GATE_TYPE_INT);
    }

    lidt((uint32_t)idt_table, sizeof(idt_table));
}

void gate_desc_set(gate_desc_t *desc, uint16_t selector, uint32_t offset, uint16_t attr) {
    desc->offset15_0 = offset & 0xFFFF;  // 低十六位
    desc->selector = selector;
    desc->attr = attr;
    desc->offset31_16 = (offset >> 16) & 0xFFFF;  // 高十六位
}