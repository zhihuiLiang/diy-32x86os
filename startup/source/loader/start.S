    .code16
    .text
    .global _start
    .extern loader_entry

_start:
    jmp loader_entry

    .code32
    .text
    .global protect_mode_entry
    .extern load_kernel
protect_mode_entry:
    mov $16, %ax        # 让寄存器以32位模式运行
    mov %ax, %ds
	mov %ax, %ss
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
    jmp $8, $load_kernel