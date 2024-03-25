#ifndef TASK_H
#define TASK_H

#include "comm/cpu_instr.h"
#include "comm/types.h"
#include "cpu/cpu.h"
#include "mm/mm.h"

#define NR_TASKS 64

// 创建、就绪、执行、阻塞、终止 是进程的一般5种理论状态
// linux有7中状态
// 1.运行状态， 包括就绪态和运行态
// 2.可中断睡眠状态， 也就是阻塞状态，收到信号后可以执行信号处理函数
// 3.不可中断的睡眠状态， 磁盘IO时会出现这种状态，进程无法被中断，不能响应信号
// 4.停止状态
// 5.退出状态
// 6.僵尸状态， 子进程先于父进程退出，并且父进程没有调用wait或waitpid回收子进程。此时子进程即处于僵尸状态
// 7.跟踪状态， 当利用gdb调试某个程序，程序停留在某个断点处时，就处于跟踪状态
// ps -aux 查看进程状态
#define TASK_RUNNING 0          // 进程正在运行或已准备就绪，就绪态+运行态
#define TASK_INTERRUPTIBLE 1    // 进程处于可中断等待状态
#define TASK_UNINTERRUPTIBLE 2  // 进程处于不可中断等待状态，主要用于 I/O 操作等待
#define TASK_ZOMBIE 3           // 进程处于僵死状态，已经停止运行，但父进程还没发信号
#define TASK_STOPPED 4          // 进程已停止

#define FIRST_TASK task[0]            // 任务 0 比较特殊，所以特意给它单独定义一个符号
#define LAST_TASK task[NR_TASKS - 1]  // 任务数组中的最后一项任务
/*
 * Entry into gdt where to find first TSS. 0-nul, 1-cs, 2-ds, 3-syscall
 * 4-TSS0, 5-LDT0, 6-TSS1 etc ...
 */
#define FIRST_TSS_ENTRY 4                      // 全局表中第 1 个任务状态段(TSS)描述符的选择符索引号
#define FIRST_LDT_ENTRY (FIRST_TSS_ENTRY + 1)  // 全局表中第 1 个局部描述符表(LDT)描述符的选择符索引号
#define _TSS(n) \
    ((((uint32_t)n) << 4) + (FIRST_TSS_ENTRY << 3))  // 宏定义，计算在全局表中第 n 个任务的 TSS 描述符的索引号（选择符）
#define _LDT(n) ((((uint32_t)n) << 4) + (FIRST_LDT_ENTRY << 3))  // 宏定义，计算在全局表中第 n 个任务的 LDT 描述符的索引号

// 任务状态段(Task State Segment)，104字节，保存任务信息，任务（进程/线程）切换用，由TR（任务寄存器）寻址。字段构成
// 1. 寄存器保存区域
// 2. 内核堆栈指针区域   一个任务可能具有四个堆栈，对应四个特权级。四个堆栈需要四个堆栈指针，3级属于用户态，没有后缀
// 3. 地址映射寄存器    用于分页寻址，似乎线程切换不需要
// 4. 链接字段     前一任务的TSS描述符的选择子
// 5. 其他字段     I/O许可位图
#pragma pack(1)
typedef struct _tss_t {
    uint32_t pre_task_link;
    uint32_t esp0, ss0;  // 特权级0时的栈指针
    uint32_t esp1, ss1;  // 特权级1时的栈指针
    uint32_t esp2, ss2;  // 特权级2时的栈指针
    uint32_t cr3;
    uint32_t eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint32_t es, cs, ss, ds, fs, gs;
    uint32_t ldt;
    uint32_t trace_bitmap; /* bits: trace 0, bitmap 16-31 */  //  当任务进行切换时导致 CPU 产生一个调试(debug)异常的
                                                              //  T-比特位（调试跟踪位）；I/O 比特位图基地址
} tss_t;
#pragma pack()

typedef struct _task_sturct_t {
    uint32_t state;     // 任务的运行状态（-1 不可运行，0 可运行(就绪)，>0 已停止）
    uint32_t counter;   // counter值的计算方式为 counter = counter /2 + priority,  优先执行counter最大的任务;
                        // 任务运行时间计数(递减)（滴答数）(时间片)
    uint32_t priority;  // 运行优先级
    uint32_t signal;    // 信号位图，每一个比特位代表一种信号，信号值=位偏移量 + 1;

    int exit_code;
    uint32_t start_code, end_code, end_data, brk, start_stack;  // 代码段地址,代码长度，数据长度，总长度，栈段地址
    uint32_t pid, father, pgrp, session, leader;  // 进程标识号，父进程号，父进程组号，会话号，会话首领
    uint16_t uid, euid, suid;                     // 用户标识号， 有效用户id，保存的用户id
    uint16_t gid, egid, sgid;                     // 组标识号，有效组号，保存的组号

    segment_desc_t ldt[3];  // 局部描述符段， 0-空，1-代码段 cs，2-数据和堆栈段 ds&ss

    tss_t tss;  // 本进程的任务状态段信息结构

} task_struct;

// clang-format off
#define INIT_TASK \
/* state etc */	{ 0,15,15, \
/* signals */	0, \
/* ec,brk... */	0,0,0,0,0,0, \
/* pid etc.. */	0,-1,0,0,0, \
/* uid etc */	0,0,0,0,0,0, \
	{ \
		{0,0, 0, 0, 0}, \
/* ldt*/{0x009f,0x0000, 0x00, 0xc0fa, 0x00}, \
		{0x009f,0x0000, 0x00, 0xc0f2, 0x00}, \
	}, \
/*tss*/	{0,PAGE_SIZE+(uint32_t)&init_task,0x10,0,0,0,0, (uint32_t)PAGE_DIR_ADDR,\
	 0,0,0,0,0,0,0,0, \
	 0,0,0x17,0x17,0x17,0x17,0x17,0x17, \
	 _LDT(0),0x80000000, \
	}, \
}
// 0x9f,0xc0fa00 代码长 640K，基址 0x0，G=1，D=1，DPL=3，P=1 TYPE=0x0a
// 0x9f,0xc0f200 数据长 640K，基址 0x0，G=1，D=1，DPL=3，P=1 TYPE=0x02
// clang-format on

extern uint32_t volatile jiffies;
extern task_struct* current;

void switch_to(int n);
void schedule(void);
void sched_init(void);

#endif