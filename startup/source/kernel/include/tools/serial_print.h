#ifndef SERIAL_PRINT_H
#define SERIAL_PRINT_H

#include "comm/cpu_instr.h"
#include "tools/string.h"

#define COM1_PORT 0x3F8

void serial_init();
void serial_printf(const char* fmt, ...);

#endif  //