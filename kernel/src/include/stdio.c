#include "stdio.h"

void putc(char c) {
    flanterm_write(ft_ctx, &c, sizeof(c));
}

void puts(const char* str) {
    while (*str != '\0') {
        putc(*str);
        str++;
    }
}