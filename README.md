# DIY-32x86OS
本仓库为本人学习如何编写一个基于Intel 32位的x86机构的操作系统的代码，学习过程参考了以下仓库、书籍及视频资料：
+ [osdev](https://wiki.osdev.org/Expanded_Main_Page)
+ [沈lifeng/linux0.11-note](https://gitee.com/shen-lifeng/linux-0.11-note#https://gitee.com/link?target=https%3A%2F%2Fpan.baidu.com%2Fs%2F1E0E7wv2MUkhjpbja2nhtzw)
+ [lishutong/diy-x86os](https://gitee.com/ddd-ggg/diy-x86os)
+ [从0开始手写操作系统](https://www.bilibili.com/video/BV1jV4y1H7Gj/?spm_id_from=333.999.0.0)
+ [Linux内核完全注释](/book/Linux内核完全注释：基于0.11内核(修正版V3.0).pdf)


## 前置知识

## QEMU相关的命令
+ 运行`QEMU`
>qemu-system-i386 -m 128M -s -S  -drive file=disk1.img,index=0,media=disk,format=raw -nographic -monitor pty -d pcall,page,mmu,cpu_reset,guest_errors,trace:ps2_keyboard_set_translation 
> + -daemonize：以守护进程模式运行 QEMU。这意味着 QEMU 将在后台运行，不会占用当前的终端会话
> + -m 128M：为模拟的系统分配 128MB 的内存
> + -s：启动一个 GDB 服务器监听在默认的端口（1234）上
> + -S：在启动时暂停 CPU，等待从 GDB 的连接后再开始执行。
> + -drive file=disk1.img,index=0,media=disk,format=raw 
> + -drive file=disk2.img,index=1,media=disk,format=raw 这两个选项分别指定了两个磁盘映像文件（disk1.img 和 disk2.img）作为原始格式（raw）的磁盘驱动器添加到模拟的系统中。index=0 和 index=1 分别指定了它们的驱动器顺序
> + -d pcall,page,mmu,cpu_reset,guest_errors,page,trace:ps2_keyboard_set_translation：启用不同的调试输出选项。这些选项包括打印系统调用（pcall）、页面相关的事件（page）、内存管理单元（MMU）事件、CPU 重置事件、来自虚拟客户机的错误以及跟踪 PS/2 键盘的设置转换函数
> + -nographic：关闭图形输出，所有的输出都通过终端进行
> + -append "console=ttyS0"：将给定的字符串附加到模拟机的内核命令行参数中。"console=ttyS0" 告诉内核使用第一个串行端口作为主控制台
> + -serial mon:stdio：将虚拟机的串行端口重定向到标准输入输出
> + -monitor pty: 指定串口作为监控器的通讯接口

+ 退出`qemu`

`ctrl + a 后再按下x`

+ screen指令进入监控器
```bash
# 进入监控器
screen /dev/pts/<n>

# 退出screen会话
ctrl + d 或者 ctrl + a 后再按下k

# 重新连接screen会话
screen -ls # 列出所有当前运行的会话
screen -r <id> # 重新连接会话

# screen 强制关闭
screen -S <id> -X qui
```
+ 查看`qemu`寄存器

`info registers`

## 操作系统的启动
### 计算机上电启动
![](/pic/计算机上电启动过程.png)
+ 对于x86架构机器为例，一上电，CPU处于`实模式`，以16位方式寻址，并且只能使用`1M`以内的内存
+ `CS = 0xFFFF，IP = 0x0000`，就是内存在0xFFFF0处ROM BIOS的指令
+ BIOS处的指令首先会检查外设，例如CPU，键盘，显示器，硬盘等
+ 然后会将磁盘0磁道0扇区的内容读到内存的`0x7C00`处
+ 最后设置CS = 0x7C00， IP = 0x0000，开始执行`boot`引导扇区里的代码

### boot引导扇区的功能
boot引导扇区由操作系统开发人员编写，大小要求`512`字节，且最后两个字符由`0x55`和`0xaa`结尾，在boot程序中可以完成以下功能:
+ 清零段寄存器，设置`esp`指针，即栈的位置
+ 显示提示字符
+ 读取`loader`程序，跳转到`loader`执行
#### 显示提示字符
BIOS提供了显示字符的功能，具体使用方法如下：
> INT 0x10功能显示字符
> - AH=功能号 0x03读取光标位置 0x13显示字符串
> - AL=显示模式 01为字符串只包含字符码，显示之后更新光标位置
> - CX=字符串长度
> - ES:BP=字符串的段：偏移地址

#### 通过INT13中断读取磁盘
BIOS提供了磁盘操作的服务中断，其具体使用方法如下:
> INT13 读取磁盘
> + AH=02H
> + AL=扇区数
> + CH=柱面
> + CL=扇面
> + DH=磁头
> + DL=驱动器， 00H~7FH：软盘；80H~0FFH：硬盘
> + ES:BX=缓冲区地址
> + 出口参数: CF=0——操作成功，AH=00H，AL=传输扇区数；否则，AH=状态码

## 进入Loader加载操作系统
本项目`boot`将`loader`的程序加载至`0x8000`处，随后`loader`将完成以下工作：
+ 检测CPU可用的内存容量
+ 加载临时的gdt表，进入保护模式
### 检测内存容量
> 检测内存容量的方法
>+ 设置`ES:DI`存储读取的信息存储的位置
>+ 清除`EBX`，设置为0
>+ `EDX`设置为 `0x534D4150`
>+ `EAX`设置为 `0xE820`
>+ `ECX`设置为 `24`
>+ 执行`INT 0x15`
>返回结果: `EAX=0x534D4150`，`CF`标志清0，`CL`=实际字节数
>
>后续调用:
>+ `EDX`设置为`0x534D4150`
>+ `EAX`重设为`0xE820`
>+ `ECX`重设为`24`
>+ 执行`INT 0x15`
>+返回结果：`EAX=0X534d4150`,`CF`标志清0，如果`EBX=0`，则表明读取完毕，当前条目有效

### 进入保护模式过程
保护模式采用分段的方式来为每个应用规定其能访问的内存区域，从而实现保护的功能。保护模式下，CPU最大寻址访问可达到4GB，并且可以采用分页机制。CPU进入保护模式的步骤如下:
+ 关闭中断, 执行`cli`指令
+ 读`0x92`端口，将`0x92`端口第二位置1，使能`A20`地址线
+ 加载`gdt`表
+ 将`CR0`置1
+ 进入保护模式，让寄存器以32位模式运行
+ 加载内核代码
#### 全局描述符表(gdt)结构
进入保护模式后，段寄存器存储的将不再是段地址的物理基址，而是段选择子。由段选择子在gdt表中查找段基址，再和段内偏移量合成线性地址。gdt表每一项由gdt描述符组成，gdt描述符的格式如下：

![gdt描述符格式](/pic/程序代码段和数据段描述符.png)

本项目进入保护模式时，设置了临时的gdt表，该表由三项组成，定义如下：
```C
uint16_t gdt_table[][4] = {
    {0, 0, 0, 0},
    {0xFFFF, 0x0000, 0x9a00, 0x00cf},
    {0xFFFF, 0x0000, 0x9200, 0x00cf},
};
```
该表由以下三项组成：
+ 第一项全为0，硬件规定
+ 第二项为代码段，基址为`0`,特权级为3，读取允许
+ 第二项为数据段，基址为`0`,特权级为3，写入允许

CPU改用分段形式管理内存时，使用的地址称之为`逻辑地址`，该地址由段描述符和段内偏移量组成，段描述符的格式如下:
![段描述符](/pic/段描述符格式.png)
+ 其中`描述符索引`指定描述符表中的该段的索引编号，该值乘以8，并加上描述符表的基址即可访问指定的段描述符
+ `TI`用于指定选择符所引用的描述符表，0指GDT表，1指LDT表。
+ `RPL`指请求者的特权级，用于保护机制

### 加载内核代码的过程
进入保护模式后，本项目采用LBA模式读取磁盘，将内核代码加载至`1MB`以后的内存空间上
### LBA48模式对磁盘文件的读
LBA48模式将硬盘上所有扇区看成线性排列，没有磁盘、柱面的概念。LBA模式读取磁盘可参考：[LBA模式读取磁盘](https://wiki.osdev.org/ATA_PIO_Mode)

其大致过程如下：
+ 选择硬盘，主盘或者从盘
+ 写扇区数量的高8位
+ 写LBA地址即起始扇区号的高24位
+ 写扇区数量的低8位
+ 写LAB地址的低24位
+ 发送磁盘读取命令`0x24`
+ 读取`0x1F7`端口，判断数据是否就绪
+ 就绪则写入缓存区，否则继续等待
### ELF文件含义
在将内核代码写入缓存区后，由于内核部分的代码被编译成elf格式，需要将缓存区的代码解析成elf格式，elf主要由以下几部分组成:
+ ELF头部（ELF Header）：包含了描述整个文件布局的全局元信息，如ELF文件的类型（可执行文件、共享库或可重定位文件），目标机器类型（例如x86、ARM），入口点地址等。

+ 程序头表（Program Header Table）：对于可执行文件和共享库，程序头表描述了系统加载程序所需的段信息，如可执行代码、数据、堆栈初始化等。

+ 节头表（Section Header Table）：描述了文件中的所有节（Section），每个节包含了不同类型的数据，如程序代码、程序数据、符号表、重定位信息等。编译器和链接器主要使用节信息。

+ 节（Sections）：文件中的实际数据部分，例如.text节包含程序的执行代码，.data节包含初始化的全局变量等。
```C
// ELF头部（ELF Header）
typedef struct {
    unsigned char e_ident[16]; // ELF识别标志
    uint16_t e_type;      // 文件类型
    uint16_t e_machine;   // 目标机器类型
    uint32_t e_version;   // 文件版本
    uint32_t e_entry;     // 程序入口点地址（32位架构）
    uint32_t e_phoff;     // 程序头部表偏移量
    uint32_t e_shoff;     // 节头部表偏移量
    uint32_t e_flags;     // 处理器特定标志
    uint16_t e_ehsize;    // ELF头部的大小
    uint16_t e_phentsize; // 程序头部表中一个入口的大小
    uint16_t e_phnum;     // 程序头部表入口数目
    uint16_t e_shentsize; // 节头部表中一个入口的大小
    uint16_t e_shnum;     // 节头部表入口数目
    uint16_t e_shstrndx;  // 节名称字符串表索引
} Elf32_Ehdr;

// 程序头部表（Program Header Table）
typedef struct {
    uint32_t p_type;   // 段类型
    uint32_t p_offset; // 段的文件偏移
    uint32_t p_vaddr;  // 段的虚拟地址（32位架构）
    uint32_t p_paddr;  // 段的物理地址
    uint32_t p_filesz; // 段在文件中的长度
    uint32_t p_memsz;  // 段在内存中的长度
    uint32_t p_flags;  // 段标志
    uint32_t p_align;  // 段在内存中的对齐
} Elf32_Phdr;

//节头部表（Section Header Table）
typedef struct {
    uint32_t sh_name;      // 节名称（字符串表索引）
    uint32_t sh_type;      // 节类型
    uint32_t sh_flags;     // 节标志
    uint32_t sh_addr;      // 节首地址（32位架构）
    uint32_t sh_offset;    // 节的文件偏移
    uint32_t sh_size;      // 节的长度
    uint32_t sh_link;      // 链接到其他节的索引
    uint32_t sh_info;      // 附加信息
    uint32_t sh_addralign; // 节对齐
    uint32_t sh_entsize;   // 某些节中条目的大小
} Elf32_Shdr;
```
具体内容可参考:[ELF文件格式](https://wiki.osdev.org/ELF)

在本项目的ELF文件中，`Elf_Ehdr`被设置为了`0x10000`，因此`reload_elf_file`会将内核代码加载至`0x10000`，随后跳转至`0x10000`处执行内核代码。加载内核后，内存中的存储规划为:
![存储规划](/pic/存储规划.png)
## 内核启动
在加载内核代码时，本项目利用调用函数时参数入栈的顺序，向内核传递启动的参数`boot_info`。在内核初始化时，将完成以下步骤:
+ 重新初始化gdt表
+ 初始化中断向量表
### 函数调用时，参数入栈的顺序

![栈的作用](/pic/函数调用时栈的使用.png)
在进行函数调用时，将会发生以下过程:
+ 返回地址入栈
+ 调用者函数寄存器`ebp`入栈，该寄存器保存了调用者栈帧尾部的基址
+ 将当前栈指针`esp`值赋予`ebp`，作为该函数栈帧栈尾地址
+ 局部变量和数据入栈
+ 函数参数从右至左依次入栈

### 重新初始化gdt表
本项目采用平坦模型，将内存分内核数据段和内核代码段：
+ 代码段，基址为`0`，段限长为`4GB`，特权级值`DPL`为0，可读
+ 数据段，基址为`0`，段限长为`4GB`，特权级值`DPL`为0，可写

### 初始化中断向量表
对于x86架构的CPU，中断`int0-int31`由Intel公司固定设定或者保留，Intel公司称之为`异常`，在CPU执行指令的时候引起，通常还可分为故障(Fault)和陷阱(traps)两类。中断`int32-int255`由用户自己设定。

中断向量表每一项由中断描述符组成，其格式如下:
![中断描述符](/pic/中断描述符.png)
+ P为段存在标志
+ DPL是描述的特权级
+ 段选择符表示该中断处理程序在内存中的那一段（代码段还是数据段）

当异常发生时，CPU依次将以下信息自动保存一部分信息到栈中：
+ EFlags 状态标志，比如进位标志、零标志
+ CS 当前程序的选择子
+ EIP 当成程序计数器值，指向正在运行的代码
+ Error Code 错误码

此外，使用`pusha`指令，将自动保存以下寄存器值:

`EAX ECX EDX EBX ESP EBP ESI EDI`

在通过`push`指令，单个压入

`DS ES FS GS`
中断处理程序处理完后，恢复栈原本的寄存器值及栈指针

### 初始化中断控制器
早期的8086CPU的中断由两片8295A芯片控制，共可管理15级中断向量。8295A在正常操作之前，必须首先设置初始化命令字ICW(Initialization Command Words)寄存器组的内容。而其在工作时，可以使用写入操作命令字OCW（Operation Command Words）寄存器组来设置和管理8295A的工作方式。

### 设置页表，开启分页机制
本项目的内存分页管理通过页目录表和内存页表所组成的二级表进行。一个页目录表4KB，可寻址1024个页表；每个页表同样4KB，可寻址1024个页；每个页大小4KB，因此最高可映射`1024×1024×4KB=4G`范围的内存。每个页目录表项和页表项结构如下所示:
![页目录表项和页表项结构](/pic/页目录表项和页表项结构.png)
+ P是页面存在于内存标志
+ R/W是读写标志
+ U/S是用户/超级用户标志， 标志为S时，只有`CPL<3`才能访问
+ A 是页面已访问标志
+ 最左边20比特是表项对应页面在物理内存中页面地址的高20比特位, 页面地址低12位总是0（内存页位于4K边界上）


## 零碎的知识

### 代码注释生成
参考 https://zhuanlan.zhihu.com/p/647274361

## 记录一些神奇的bug
### cmake链接的标志跑到gcc编译里去了，导致编译通不过
+ 添加空的.c和.h文件后解决

### 运行脚本时提示"syntax error: unexpected end of file"
+ .sh脚本文件在linux下需要以LF结尾

