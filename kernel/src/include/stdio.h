#ifndef __STDIO_H__
#define __STDIO_H__

#include "flanterm/flanterm.h"
#include "flanterm/backends/fb.h"

extern struct flanterm_context *ft_ctx;

void putc(char c);
void puts(const char *str);

#endif // __STDIO_H__