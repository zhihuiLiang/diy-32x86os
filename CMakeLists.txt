# 顶层cmakelist文件，以下的设置将被子工程所继承使用
cmake_minimum_required(VERSION 3.0.0)

# 保存一些通用的配置
set(CMAKE_VERBOSE_MAKEFILE on)   # 开启输出编译详细过程的提示

# gcc工具链前缀

# C编译器与参数配置 
# -c: 只执行编译操作
# -fno-pie： 禁用位置无关执行
# -fno-stack-protector: 禁用栈保护
# -fno-asynchronous-unwind-tables：禁用异步异常展开表的生成
set(CMAKE_C_FLAGS "-g -c -O0 -m32 -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables")

# 汇编器与参数配置
set(CMAKE_ASM_COMPILER "gcc")
set(CMAKE_ASM_FLAGS "-m32 -g")
set(CMAKE_ASM_SOURCE_FILE_EXTENSIONS "asm")

# 其它工具
set(OBJCOPY_TOOL "objcopy")
set(OBJDUMP_TOOL "objdump")
set(READELF_TOOL "readelf")

# 工程，启用C语言和汇编语言
project(os LANGUAGES C)  
enable_language(ASM)

# 头文件搜索路径
include_directories(
    ${PROJECT_SOURCE_DIR}/startup/source
    ${PROJECT_SOURCE_DIR}/startup/source/kernel/include
#     ${PROJECT_SOURCE_DIR}/../newlib/i686-elf/include
)

# 底层的若干子项目：含内核及应用程序
add_subdirectory(./startup/source/boot)
add_subdirectory(./startup/source/loader)
add_subdirectory(./startup/source/kernel)
# add_subdirectory(./source/applib)
# add_subdirectory(./source/shell)
# add_subdirectory(./source/init)
# add_subdirectory(./source/loop)

# 添加编译依赖，先生成app库，再生成kernel和shell
# 不加则cmake则可能先编译shell和kernel，而缺少libapp，导致编译错误
# add_dependencies(init app)       
# add_dependencies(shell app)
# add_dependencies(loop app)
# add_dependencies(kernel init)
