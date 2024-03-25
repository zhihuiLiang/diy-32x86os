/**
 * *****************************************************************************
 * @file        types.h
 * @brief
 * @author      liangzhihui
 * @date        2024-03-19
 * @copyright   lishutong liangzhihui
 * *****************************************************************************
 */

#ifndef TYPES_H
#define TYPES_H

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;

typedef long off_t;  // 用于文件长度（大小）

typedef int (*fn_ptr)();  // 定义函数指针类型

#define NULL ((void*)0)

#endif