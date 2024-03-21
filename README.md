# DIY-32x86OS

## 操作系统的启动
### BIOS通过INT10显示字符
+ AH=功能号 03读取光标位置 13显示字符串
+ AL=字符
+ BL=前景色
+ BH=页码
+ ES:BP是串地址


### BIOS通过INT13中断读取磁盘
BIOS提供了磁盘操作的服务中断，其具体使用方法如下:
+ AH=02H
+ AL=扇区数
+ CH=柱面
+ CL=扇面
+ DH=磁头
+ DL=驱动器， 00H~7FH：软盘；80H~0FFH：硬盘
+ ES:BX=缓冲区地址
+ 出口参数: CF=0——操作成功，AH=00H，AL=传输扇区数；否则，AH=状态码

### 内存检测的方法
+ 设置`ES:DI`存储读取的信息存储的位置
+ 清除`EBX`，设置为0
+ `EDX`设置为 `0x534D4150`
+ `EAX`设置为 `0xE820`
+ `ECX`设置为 `24`
+ 执行`INT 0x15`
返回结果: `EAX=0x534D4150`，`CF`标志清0，`CL`=实际字节数

后续调用:
+ `EDX`设置为`0x534D4150`
+ `EAX`重设为`0xE820`
+ `ECX`重设为`24`
+ 执行`INT 0x15`
返回结果：`EAX=0X534d4150`,`CF`标志清0，如果`EBX=0`，则表明读取完毕，当前条目有效

### 内嵌汇编语法
```
asm(
    内嵌汇编指令
    ：输出操作数
    ：输入操作数
    ：破坏描述
)
```
### LBA模式对磁盘文件的读


### ELF文件含义

## 零碎的知识
### i386和x86的区别
+ x86是Intel开发的一种32复杂指令集，早期的Intel的CPU和AMD的CPU都支持这种指令集，官方文档称之为"IA-32"
+ i386是基于x86的32位架构
+ x86_64是x86指令集的64位架构

### QEMU相关指令
```bash
# -daemonize：以守护进程模式运行 QEMU。这意味着 QEMU 将在后台运行，不会占用当前的终端会话
# -m 128M：为模拟的系统分配 128MB 的内存
# -s：启动一个 GDB 服务器监听在默认的端口（1234）上
# -S：在启动时暂停 CPU，等待从 GDB 的连接后再开始执行。
# -drive file=disk1.img,index=0,media=disk,format=raw 
# -drive file=disk2.img,index=1,media=disk,format=raw 这两个选项分别指定了两个磁盘映像文件（disk1.img 和 disk2.img）作为原始格式（raw）的磁盘驱动器添加到模拟的系统中。index=0 和 index=1 分别指定了它们的驱动器顺序
# -d pcall,page,mmu,cpu_reset,guest_errors,page,trace:ps2_keyboard_set_translation：启用不同的调试输出选项。这些选项包括打印系统调用（pcall）、页面相关的事件（page）、内存管理单元（MMU）事件、CPU 重置事件、来自虚拟客户机的错误以及跟踪 PS/2 键盘的设置转换函数
# -nographic：关闭图形输出，所有的输出都通过终端进行
# -append "console=ttyS0"：将给定的字符串附加到模拟机的内核命令行参数中。"console=ttyS0" 告诉内核使用第一个串行端口作为主控制台
# -serial mon:stdio：将虚拟机的串行端口重定向到标准输入输出
# -monitor pty: 指定串口作为监控器的通讯接口，在通过screen /dev/pts/<n> 可访问该监控器
qemu-system-i386 -m 128M -s -S  -drive file=disk1.img,index=0,media=disk,format=raw -nographic -monitor pty -d pcall,page,mmu,cpu_reset,guest_errors,trace:ps2_keyboard_set_translation 

#查看寄存器
info registers

#qemu界面退出
ctrl + a 后再按下x

# screen 会话退出
ctrl + d 或者 ctrl + a 后再按下k
# 重新连接screen会话
screen -ls # 列出所有当前运行的会话
screen -r <id> # 重新连接会话
# screen 强制关闭
screen -S <id> -X quit
```

### 代码注释生成
参考 https://zhuanlan.zhihu.com/p/647274361

## 记录一些神奇的bug
### cmake链接的标志跑到gcc编译里去了，导致编译通不过
+ 添加空的.c和.h文件后解决

### 运行脚本时提示"syntax error: unexpected end of file"
+ .sh脚本文件在linux下需要以LF结尾

