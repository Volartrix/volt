#ifndef __STRING_H__
#define __STRING_H__

#include <ctype.h>
#include <stddef.h>
#include <stdint.h>
#include "ctype.h"

void*  memcpy(void* dest, const void* src, size_t n);
void*  memmove(void* dest, const void* src, size_t n);
char*  strcpy(char* dest, const char* src);
char*  strncpy(char* dest, const char* src, size_t n);
char*  strncpy_safe(char* dest, const char* src, size_t n);
char*  strcat(char* dest, const char* src);
char*  strncat(char* dest, const char* src, size_t n);
int    memcmp(const void* s1, const void* s2, size_t n);
int    strcmp(const char* s1, const char* s2);
int    strncmp(const char* s1, const char* s2, size_t n);
void*  memchr(const void* s, int c, size_t n);
char*  strchr(const char* s, int c);
char*  strchrnul(const char* s, int c);
size_t strcspn(const char* s1, const char* s2);
char*  strpbrk(const char* s1, const char* s2);
char*  strrchr(const char* s, int c);
size_t strspn(const char* s1, const char* s2);
char*  strstr(const char* haystack, const char* needle);
void*  memset(void* s, int c, size_t n);
char*  strtok(char* str, const char* delim);
size_t strtol(const char* str, char** endptr, int base);
size_t strlen(const char* s);
void   strrev(char* str);
void   strlwr(char* str);
void   strupr(char* str);
int    strcaseeq(const char* s1, const char* s2);


#endif    // __STRING_H__
