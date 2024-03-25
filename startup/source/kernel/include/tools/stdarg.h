#ifndef _STDARG_H
#define _STDARG_H

// stdarg.h 是标准参数头文件。它以宏的形式定义变量参数列表。主要说明了-个类型(va_list)和三个
// 宏(va_start, va_arg 和 va_end)，用于 vsprintf、vprintf、vfprintf 函数

// 具体步骤如下：
// 1.定义一个函数，最后一个参数为省略号，省略号前是可以设置自定义参数的.
// 2.在函数定义中创建一个 va_list 类型变量，该类型是在 stdarg.h 头文件中定义的.
// 3.使用 int 参数和 va_start 宏来初始化 va_list 变量为一个参数列表。宏 va_start 是在 stdarg.h 头文件中定义的.
// 4.使用 va_arg 宏和 va_list 变量来访问参数列表中的每个项.
// 5.使用宏 va_end 来清理赋予 va_list 变量的内存.

typedef char *va_list;  // 字符指针， 指向多个字符

/* Amount of space required in an argument list for an arg of type TYPE.
   TYPE may alternatively be an expression whose type is used.  */

// 计算 TYPE 类型的参数在当前平台上的对齐后的大小
#define __va_rounded_size(TYPE) \
    (((sizeof(TYPE) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))

#ifndef __sparc__
#define va_start(AP, LASTARG) \
    (AP = ((char *)&(LASTARG) + __va_rounded_size(LASTARG)))  // 指向第一个参数
#else
#define va_start(AP, LASTARG) \
    (__builtin_saveregs(),    \
     AP = ((char *)&(LASTARG) + __va_rounded_size(LASTARG)))
#endif

// void va_end(va_list); /* Defined in gnulib */  // 释放va_list
#define va_end(AP)

#define va_arg(AP, TYPE)            \
    (AP += __va_rounded_size(TYPE), \
     *((TYPE *)(AP - __va_rounded_size(TYPE))))  // 不断访问下一个参数

#endif /* _STDARG_H */
