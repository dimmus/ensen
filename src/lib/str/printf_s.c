#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "safe_str_lib.h"

int printf_s(const char *format, ...) {
    va_list args;
    va_start(args, format);

    int status = 0;
    size_t format_len = strlen(format);
    for (size_t i = 0; i < format_len; i++) {
        if (format[i] == '%' && isspace(format[i+1])) {
            status = -1;
            fprintf(stderr, "Error: Invalid format string\n");
            break;
        }
    }

    if (!status) {
        if (vprintf(format, args) < 0) {
            status = -1;
            fprintf(stderr, "Error: printf failed\n");
        }
    }

    va_end(args);
    
    return status;
}
