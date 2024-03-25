#include "init.h"

void kernel_init(boot_info_t* boot_info) {
    init_gdt();
    serial_init();
    irq_init();
    sched_init();
    init_page_table();
    time_init();

    serial_printf("Kernal Init Successfully");
}

void init_main(void) {
    serial_printf("Ini Main, Vesion:%d", 1);
    irq_enable_global();
    for(;;) {
    }
}