#include "system_call.h"

uint32_t last_pid = 0;

int copy_mem(int nr, task_struct* p) {
    uint32_t old_data_base, new_data_base, data_limit;
    uint32_t old_code_base, new_code_base, code_limit;

    code_limit = get_limit(0x0f);  // 0x0f = 00001111: 代码段，LDT，3特权级
    data_limit = get_limit(0x17);  // 0x17 = 00010111: 数据段，LDT，3特权级
    old_code_base = get_base(&current->ldt[1]);
    old_data_base = get_base(&current->ldt[2]);

    if(old_data_base != old_code_base)
        panic("We don't support separate I&D");
    if(data_limit < code_limit)
        panic("Bad data_limit");
    new_data_base = new_code_base = nr * 0x4000000;  // 新基址=任务号*64Mb(任务大小)
    p->start_code = new_code_base;
    set_base(&p->ldt[1], new_code_base);                              // 设置代码段描述符中基址域
    set_base(&p->ldt[2], new_data_base);                              // 设置数据段描述符中基址域
    if(copy_page_tables(old_data_base, new_data_base, data_limit)) {  // 复制代码和数据段
        free_page_tables(new_data_base, data_limit);
        return -ENOMEM;
    }
    return 0;
}

int copy_process(int nr, uint32_t ebp, uint32_t edi, uint32_t esi, uint32_t gs, uint32_t none, uint32_t ebx, uint32_t ecx,
                 uint32_t edx, uint32_t fs, uint32_t es, uint32_t ds, uint32_t eip, uint32_t cs, uint32_t eflags, uint32_t esp,
                 uint32_t ss) {
    task_struct* p;
    int i;
    // struct file* f;

    p = (task_struct*)get_free_page();
    if(!p)
        return -EAGAIN;
    task[nr] = p;
    *p = *current; /* NOTE! this doesn't copy the supervisor stack */  // 完全复制之前的进程的 task_struct
    p->state = TASK_UNINTERRUPTIBLE;  // 很重要，此时进程还未复制完成，不能被进程调度打断
    p->pid = last_pid;
    p->father = current->pid;
    p->counter = p->priority;
    p->signal = 0;
    // p->alarm = 0;
    p->leader = 0; /* process leadership doesn't inherit */
    // p->utime = p->stime = 0;
    // p->cutime = p->cstime = 0;
    // p->start_time = jiffies;
    // p->tss.back_link = 0;
    p->tss.esp0 = PAGE_SIZE + (uint32_t)p;
    p->tss.ss0 = 0x10;
    p->tss.eip = eip;
    p->tss.eflags = eflags;
    p->tss.eax = 0;
    p->tss.ecx = ecx;
    p->tss.edx = edx;
    p->tss.ebx = ebx;
    p->tss.esp = esp;
    p->tss.ebp = ebp;
    p->tss.esi = esi;
    p->tss.edi = edi;
    p->tss.es = es & 0xffff;
    p->tss.cs = cs & 0xffff;
    p->tss.ss = ss & 0xffff;
    p->tss.ds = ds & 0xffff;
    p->tss.fs = fs & 0xffff;
    p->tss.gs = gs & 0xffff;
    p->tss.ldt = _LDT(nr);
    p->tss.trace_bitmap = 0x80000000;

    if(copy_mem(nr, p)) {
        task[nr] = NULL;
        free_page((uint32_t)p);
        return -EAGAIN;
    }

    SET_TSS_DESC(nr, (uint32_t) & (p->tss));
    SET_LDT_DESC(nr, (uint32_t) & (p->ldt));
    p->state = TASK_RUNNING; /* do this last, just in case */  // 复制完成之后置为可运行状态
    return last_pid;
}

// 为新进程取得不重复的进程号 last_pid
// 并返回在任务数组中空的一项的索引号
int find_empty_process(void) {
    int i;

    while(1) {
        if((++last_pid) < 0) {
            last_pid = 1;
        }
        for(i = 0; i < NR_TASKS; ++i) {
            if(task[i] && task[i]->pid == last_pid) {
                break;
            }
        }
        break;
    }
    for(i = 1; i < NR_TASKS; ++i) {
        if(!task[i]) {
            return i;
        }
    }
    return -EAGAIN;
}