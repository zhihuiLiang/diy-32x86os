# 适用于Linux
# -daemonize：以守护进程模式运行 QEMU。这意味着 QEMU 将在后台运行，不会占用当前的终端会话
# -m 128M：为模拟的系统分配 128MB 的内存
# -s：启动一个 GDB 服务器监听在默认的端口（1234）上
# -S：在启动时暂停 CPU，等待从 GDB 的连接后再开始执行。
# -drive file=disk1.img,index=0,media=disk,format=raw 
# -drive file=disk2.img,index=1,media=disk,format=raw 这两个选项分别指定了两个磁盘映像文件（disk1.img 和 disk2.img）作为原始格式（raw）的磁盘驱动器添加到模拟的系统中。index=0 和 index=1 分别指定了它们的驱动器顺序
# -d pcall,page,mmu,cpu_reset,guest_errors,page,trace:ps2_keyboard_set_translation：启用不同的调试输出选项。这些选项包括打印系统调用（pcall）、页面相关的事件（page）、内存管理单元（MMU）事件、CPU 重置事件、来自虚拟客户机的错误以及跟踪 PS/2 键盘的设置转换函数
# -nographic：关闭图形输出，所有的输出都通过终端进行
# -append "console=ttyS0"：将给定的字符串附加到模拟机的内核命令行参数中。"console=ttyS0" 告诉内核使用第一个串行端口作为主控制台
# -serial mon:stdio：将虚拟机的串行端口重定向到标准输入输出。当与-nographic
qemu-system-i386 -m 128M -s -S  -drive file=disk1.img,index=0,media=disk,format=raw -nographic -monitor pty -d pcall,page,mmu,cpu_reset,guest_errors,trace:ps2_keyboard_set_translation 
# qemu-system-i386 -m 128M -drive file=disk1.img,index=0,media=disk,format=raw -nographic -serial mon:stdio