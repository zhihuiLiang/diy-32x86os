project(boot LANGUAGES C)

# -m 指定特定的目标文件格式或平台
# -Ttext=0x7c00 告诉链接器，程序的文本段（通常包含可执行代码）应该从内存地址 0x7c00 开始加载
# --section-start boot_end=0x7dfe 指定名为 boot_end 的段的起始地址为 0x7dfe。
set(CMAKE_EXE_LINKER_FLAGS "-m elf_i386 -Ttext=0x7c00  --section-start boot_end=0x7dfe")
set(CMAKE_C_LINK_EXECUTABLE "ld <OBJECTS> ${CMAKE_EXE_LINKER_FLAGS} -o ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.elf")

# 将所有的汇编、C文件加入工程
# 注意保证start.asm在最前头，这样对应的程序会放在bin文件开头，从而在加载到0x7c00时能在开头处
file(GLOB C_LIST "*.c" "*.h")
add_executable(${PROJECT_NAME} start.S ${C_LIST})

target_link_libraries(${PROJECT_NAME} <OBJECTS> ${CMAKE_EXE_LINKER_FLAGS})
# bin文件生成，写入到image目录下
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${OBJCOPY_TOOL} -O binary ${PROJECT_NAME}.elf ${CMAKE_SOURCE_DIR}/image/${PROJECT_NAME}.bin
                   COMMAND ${OBJDUMP_TOOL} -x -d -S -m i8086 ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.elf > ${PROJECT_NAME}_dis.txt
                   COMMAND ${READELF_TOOL} -a ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.elf > ${PROJECT_NAME}_elf.txt
)