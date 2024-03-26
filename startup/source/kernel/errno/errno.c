#include "errno.h"

int errno;

// 该函数用来显示内核中出现的重大错误信息，并运行文件系统同步函数，然后进入死循环 -- 死机。
// 如果当前进程是任务 0 的话，还说明是交换任务出错，并且还没有运行文件系统同步函数
volatile void panic(const char* s) {
    serial_printf("Kernel panic: %s\n\r", s);
    if(current == task[0])
        serial_printf("In swapper task - not syncing\n\r");
    for(;;)
        ;
}