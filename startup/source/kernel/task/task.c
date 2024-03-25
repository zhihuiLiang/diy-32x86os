#include "task/task.h"

union task_union {
    task_struct task;
    char stack[PAGE_SIZE];
};

static union task_union init_task = {
    INIT_TASK,
};

uint32_t volatile jiffies = 0;
task_struct* current = &(init_task.task);

task_struct* task[NR_TASKS] = {
    &(init_task.task),
};

/*
首先判断任务是否为当前任务;
是，则直接退出;
将新的任务TSS的16位选择符存入__tmp.b中;
current = task[n]，ecx = 被切换出去的任务;
执行长跳转，造成任务切换;
*/
void switch_to(int n) {
    if(task[n] == current) {
        return;
    }
    current = task[n];
    far_jump(_TSS(n), 0);
}

void schedule(void) {
    int i, next, c;
    task_struct** p;

    while(1) {
        c = -1;
        next = 0;
        i = NR_TASKS;
        p = &task[NR_TASKS];
        while(--i) {
            if(!*--p)
                continue;
            if((*p)->state == TASK_RUNNING && (*p)->counter > c)  // get counter_max 值
                c = (*p)->counter, next = i;
        }
        if(c)
            break;
        for(p = &LAST_TASK; p > &FIRST_TASK; --p)  // calculate counter
            if(*p)
                (*p)->counter = ((*p)->counter >> 1) + (*p)->priority;
    }
    switch_to(next);
}

void sched_init(void) {
    int i;
    segment_desc_t* p;

    // todo 104?
    segment_desc_set(FIRST_TSS_ENTRY << 3, (uint32_t) & (init_task.task.tss), 104, SEG_P_PRESENT | SEG_TYPE_CODE | SEG_TYPE_A);
    segment_desc_set(FIRST_LDT_ENTRY << 3, (uint32_t) & (init_task.task.ldt), 104, SEG_P_PRESENT | SEG_TYPE_RW);

    for(int i = 1; i < NR_TASKS; ++i) {
        task[i] = NULL;
    }

    // 清除eflags中的标志位NT，避免硬件级的任务切换机制干扰软件实现的任务管理
    __asm__("pushfl ; andl $0xffffbfff,(%esp) ; popfl");

    ltr(_TSS(0));
    lldt(_LDT(0));
}
