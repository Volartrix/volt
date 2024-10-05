#ifndef __STDIO_H__
#define __STDIO_H__

extern struct flanterm_context *fb_ctx;

void putc(char c);
void puts(char *str);

#endif // __STDIO_H__