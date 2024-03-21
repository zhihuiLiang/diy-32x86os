    .code16
    .text
    .global _start
    .extern boot_entry
_start:
	mov $0, %ax
	mov %ax, %ds
	mov %ax, %ss
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

    mov $_start, %esp

    mov	$37, %cx      # 将字符串的长度（37个字符）加载到 CX
    mov	$0x0007, %bx  # 设置属性（前景色和背景色），这里是亮白色前景和黑色背景
    mov	$msg, %bp     # 将字符串的内存地址加载到 BP（ES:BP=字符地址）
    mov	$0x1301, %ax  # 设置 AH 为 0x13， AL为0x01
    int	$0x10         # 调用显示字符 BIOS 中断，执行功能号在 AH 中指定的操作
    jmp .

read_loader:
	mov $0x8000, %bx        
	mov $0x2, %ah
	mov $0x2, %cx
	mov $64, %al
	mov $0x0080, %dx
	int $0x13
	jc read_loader
	jmp boot_entry
	jmp .

msg:
    .byte 13, 10 # 回车、换行的ASCII码
    .ascii "Loading lzh study lst_os system"
    .byte 13, 10, 13, 10

    # 引导结束段，"ax"为ELF格式允许的段标志，"a"可分配，"x"可执行
    .section boot_end, "ax"
boot_sig: .byte 0x55, 0xaa