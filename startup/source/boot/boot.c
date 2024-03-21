__asm__(".code16gcc");

#include "boot.h"

#define LOADER_START_ADDR 0x8000

/**
 * @brief Boot的C入口函数, 跳转到LOADER_START_ADDR      
 * 
 */
void boot_entry(void) {
    ((void (*)(void))LOADER_START_ADDR)();
}
