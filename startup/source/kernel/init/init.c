#include "init.h"

void kernel_init(boot_info_t* boot_info) {
    init_gdt();
    serial_init();
    irq_init();
    init_page_table();
    sched_init();
    time_init();

    serial_printf("Kernal Init Successfully");
}

void init_main(void) {
    serial_printf("Ini Main, Vesion:%d", 1);
    // mem_init();

    irq_enable_global();
    move_to_usr_mode();
    for(;;) {
    }
}