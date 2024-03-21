#ifndef IRQ_H 
#define IRQ_H 

#include "comm/types.h"

#define IDE_TABLE_NR 128

typedef struct _gate_desc_t {
    uint16_t offset15_0;
    uint16_t selector;
    uint16_t attr;
    uint16_t offset31_16;

} gate_desc_t;

#define GATE_TYPE_INT (0xE << 8)
#define GATE_P_PRESENT (1 << 15)
#define GATE_DPL0 (0 << 13)
#define DATE_DPL3 (3 << 13)

void irq_init(void);
void gate_desc_set(gate_desc_t *desc, uint16_t selector, uint32_t offset, uint16_t attr);

#endif	/* IRQ_H */
