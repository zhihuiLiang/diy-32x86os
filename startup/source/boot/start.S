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

    # movb $0x03, %ah    # read cursor position
    # xorb %bh, %bh
    # int $0x10

    # mov	$34, %cx      # 将字符串的长度加载到 CX
    # mov   $0x0007, %bx  # 设置页号和字符属性
    # mov	$msg, %bp     # 将字符串的内存地址加载到 BP（ES:BP=字符地址）
    # mov	$0x1301, %ax  # 设置 AH 为 0x13， AL为0x01
    # int	$0x10         # 调用显示字符 BIOS 中断，执行功能号在 AH 中指定的操作

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

    .data                     # 数据段
msg:
    .string "Loading lzh study lst_os system..."   # 字符串定义，包括回车换行

    # 引导结束段，"ax"为ELF格式允许的段标志，"a"可分配，"x"可执行
    .section boot_end, "ax"
boot_sig: .byte 0x55, 0xaa