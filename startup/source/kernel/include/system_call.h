#ifndef SYSTEM_CALL_H
#define SYSTEM_CALL_H

#include "errno.h"

#define __NR_setup	0	/* used only by init, to get system going */
#define __NR_exit	1
#define __NR_fork	2
#define __NR_read	3
#define __NR_write	4
#define __NR_open	5
#define __NR_close	6
#define __NR_waitpid	7
#define __NR_creat	8
#define __NR_link	9
#define __NR_unlink	10
#define __NR_execve	11
#define __NR_chdir	12
#define __NR_time	13
#define __NR_mknod	14
#define __NR_chmod	15
#define __NR_chown	16
#define __NR_break	17
#define __NR_stat	18
#define __NR_lseek	19
#define __NR_getpid	20
#define __NR_mount	21
#define __NR_umount	22
#define __NR_setuid	23
#define __NR_getuid	24
#define __NR_stime	25
#define __NR_ptrace	26
#define __NR_alarm	27
#define __NR_fstat	28
#define __NR_pause	29
#define __NR_utime	30
#define __NR_stty	31
#define __NR_gtty	32
#define __NR_access	33
#define __NR_nice	34
#define __NR_ftime	35
#define __NR_sync	36
#define __NR_kill	37
#define __NR_rename	38
#define __NR_mkdir	39
#define __NR_rmdir	40
#define __NR_dup	41
#define __NR_pipe	42
#define __NR_times	43
#define __NR_prof	44
#define __NR_brk	45
#define __NR_setgid	46
#define __NR_getgid	47
#define __NR_signal	48
#define __NR_geteuid	49
#define __NR_getegid	50
#define __NR_acct	51
#define __NR_phys	52
#define __NR_lock	53
#define __NR_ioctl	54
#define __NR_fcntl	55
#define __NR_mpx	56
#define __NR_setpgid	57
#define __NR_ulimit	58
#define __NR_uname	59
#define __NR_umask	60
#define __NR_chroot	61
#define __NR_ustat	62
#define __NR_dup2	63
#define __NR_getppid	64
#define __NR_getpgrp	65
#define __NR_setsid	66
#define __NR_sigaction	67
#define __NR_sgetmask	68
#define __NR_ssetmask	69
#define __NR_setreuid	70
#define __NR_setregid	71

// 以下定义系统调用嵌入式汇编宏函数。 
 // 不带参数的系统调用宏函数。type name(void)。 
 // %0 - eax(__res)，%1 - eax(__NR_##name)。其中 name 是系统调用的名称，与 __NR_ 组合形成上面 
 // 的系统调用符号常数，从而用来对系统调用表中函数指针寻址。 
 // 返回：如果返回值大于等于 0，则返回该值，否则置出错号 errno，并返回-1。
#define _syscall0(type,name) \
type name(void) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name)); \
if (__res >= 0) \
	return (type) __res; \
errno = -__res; \
return -1; \
}

#define _syscall1(type,name,atype,a) \
type name(atype a) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name),"b" ((long)(a))); \
if (__res >= 0) \
	return (type) __res; \
errno = -__res; \
return -1; \
}

#define _syscall2(type,name,atype,a,btype,b) \
type name(atype a,btype b) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name),"b" ((long)(a)),"c" ((long)(b))); \
if (__res >= 0) \
	return (type) __res; \
errno = -__res; \
return -1; \
}

#define _syscall3(type,name,atype,a,btype,b,ctype,c) \
type name(atype a,btype b,ctype c) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name),"b" ((long)(a)),"c" ((long)(b)),"d" ((long)(c))); \
if (__res>=0) \
	return (type) __res; \
errno=-__res; \
return -1; \
}

int sys_setup();
int sys_exit();
int sys_fork();
int sys_read();
int sys_write();
// int sys_open();
// int sys_close();
// int sys_waitpid();
// int sys_creat();
// int sys_link();
// int sys_unlink();
// int sys_execve();
// int sys_chdir();
// int sys_time();
// int sys_mknod();
// int sys_chmod();
// int sys_chown();
// int sys_break();
// int sys_stat();
// int sys_lseek();
// int sys_getpid();
// int sys_mount();
// int sys_umount();
// int sys_setuid();
// int sys_getuid();
// int sys_stime();
// int sys_ptrace();
// int sys_alarm();
// int sys_fstat();
// int sys_pause();
// int sys_utime();
// int sys_stty();
// int sys_gtty();
// int sys_access();
// int sys_nice();
// int sys_ftime();
// int sys_sync();
// int sys_kill();
// int sys_rename();
// int sys_mkdir();
// int sys_rmdir();
// int sys_dup();
// int sys_pipe();
// int sys_times();
// int sys_prof();
// int sys_brk();
// int sys_setgid();
// int sys_getgid();
// int sys_signal();
// int sys_geteuid();
// int sys_getegid();
// int sys_acct();
// int sys_phys();
// int sys_lock();
// int sys_ioctl();
// int sys_fcntl();
// int sys_mpx();
// int sys_setpgid();
// int sys_ulimit();
// int sys_uname();
// int sys_umask();
// int sys_chroot();
// int sys_ustat();
// int sys_dup2();
// int sys_getppid();
// int sys_getpgrp();
// int sys_setsid();
// int sys_sigaction();
// int sys_sgetmask();
// int sys_ssetmask();
// int sys_setreuid();
// int sys_setregid();

#endif // !SYSTEM_CALL_H

