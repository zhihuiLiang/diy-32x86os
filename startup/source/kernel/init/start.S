    
    #include "os_cfg.h"
    .text
    .extern kernel_init, init_main
    .global _start, _pg_dir
    # void start(boot_info_t * boot_info)
_start:
    # 
    push %ebp
    mov %esp, %ebp
    mov  0x8(%ebp), %eax


    # 相当于kernel_init(boot_info)
    push 4(%eax)
    call kernel_init

    jmp $KERNEL_SELECTOR_CS , $gdt_reload
gdt_reload:
    mov $KERNEL_SELECTOR_DS, %ax
	mov %ax, %ds
	mov %ax, %ss
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

    mov $(stack + KERNEL_STACK_SIZE), %esp

    pushl $_main
    jmp setup_paging

.org 0x0 
_pg_dir:

.org 0x1000
pg0:

.org 0x2000
pg1:

.org 0x3000
pg2:

.org 0x4000
pg3:

setup_paging:
	movl $1024*5,%ecx		/* 5 pages - pg_dir+4 page tables */
	xorl %eax,%eax
	xorl %edi,%edi			/* pg_dir is at 0x000 */
	cld;rep;stosl
	movl $pg0+7,_pg_dir		/* set present bit/user r/w */
	movl $pg1+7,_pg_dir+4		/*  --------- " " --------- */
	movl $pg2+7,_pg_dir+8		/*  --------- " " --------- */
	movl $pg3+7,_pg_dir+12		/*  --------- " " --------- */
	movl $pg3+4092,%edi
	movl $0xfff007,%eax		/*  16Mb - 4096 + 7 (r/w user,p) */
	std
1:	stosl			/* fill pages backwards - more efficient :-) */
	subl $0x1000,%eax
	jge 1b
	xorl %eax,%eax		/* pg_dir is at 0x0000 */
	movl %eax,%cr3		/* cr3 - page directory start */
	movl %cr0,%eax
	orl $0x80000000,%eax
	movl %eax,%cr0		/* set paging (PG) bit */
    ret

    .bss
.comm stack,KERNEL_STACK_SIZE
    .text
# 定义汇编里面宏
.macro exception_handler  name  num with_err_code
        .extern do_handler_\name
        .global  exception_handler_\name
    exception_handler_\name:
        .if \with_err_code == 0
            push $0
        .endif
        push $\num
        pusha
        push %ds
        push %es
        push %fs
        push %gs

        push %esp
        call do_handler_\name
        add $(1*4), %esp

        pop %gs
        pop %fs
        pop %es 
        pop %ds
        popa
        add $(2*4), %esp

        iret
.endm

exception_handler unknown, -1, 0
exception_handler divider, 0, 0
exception_handler Debug, 1, 0
exception_handler NMI, 2, 0
exception_handler breakpoint, 3, 0
exception_handler overflow, 4, 0
exception_handler bound_range, 5, 0
exception_handler invalid_opcode, 6, 0
exception_handler device_unavailable, 7, 0
exception_handler double_fault, 8, 1
exception_handler invalid_tss, 10, 1
exception_handler segment_not_present, 11, 1
exception_handler stack_segment_fault, 12, 1
exception_handler general_protection, 13, 1
exception_handler page_fault, 14, 1
exception_handler fpu_error, 16, 0
exception_handler alignment_check, 17, 1
exception_handler machine_check, 18, 0
exception_handler smd_exception, 19, 0
exception_handler virtual_exception, 20, 0

# 硬件中断
exception_handler timer, 0x20, 0