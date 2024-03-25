#include "system_call.h"

#include "system_call.h"
#include "comm/types.h"

_syscall3(int,write,int,fd,const char *,buf,off_t,count);

fn_ptr sys_call_table[] = { sys_setup, sys_exit, sys_fork, sys_read,
sys_write};

//todo 
int sys_setup(){
    return 0;
}

//todo
int sys_exit(){
     return 0;
}

//todo
int sys_fork(){
    return 0;
}

//todo
int sys_read(){
    return 0;
}

//todo
int sys_write(){
    return 0;
}