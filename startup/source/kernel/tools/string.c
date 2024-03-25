#include "tools/string.h"

int strlen(const char* str) {
    int cnt = 0;
    while (*str++ != '\0') {
        ++cnt;
    }
    return cnt;
}

int vsprintf(char* buf, const char* fmt, va_list args) {
    char* str;
    for (str = buf; *fmt; ++fmt) {
        if (*fmt != '%') {
            *str++ = *fmt;
            continue;
        }

        char* s;
        int len = 0, num = 0;
        ++fmt;
        switch (*fmt) {
        case 'c':
            *str++ = (unsigned char)va_arg(args, int);
            break;
        case 's':
            s = va_arg(args, char*);
            len = strlen(s);
            for (int i = 0; i < len; ++i) {
                *str++ = *s++;
            }
            break;
        case 'd':
            num = va_arg(args, int);
            while (num) {
                *str++ = num % 10 + '0';
                num /= 10;
            }
            break;
        default:
            if (*fmt != '%')
                *str++ = '%';
            if (*fmt)
                *str++ = *fmt;
            else
                --fmt;
            break;
        }

        *str = '\0';
    }
    return str - buf;
}