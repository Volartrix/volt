#include "string.h"

void* memcpy(void* dest, const void* src, size_t n) {
    char*       d = (char*)dest;
    const char* s = (const char*)src;
    while (n-- > 0) {
        *d++ = *s++;
    }
    return dest;
}

void* memmove(void* dest, const void* src, size_t n) {
    char*       d = (char*)dest;
    const char* s = (const char*)src;

    if (d == s) {
        return dest;
    }

    if (d < s) {
        while (n-- > 0) {
            *d++ = *s++;
        }
    } else {
        d += n;
        s += n;
        while (n-- > 0) {
            *--d = *--s;
        }
    }

    return dest;
}

char* strcpy(char* dest, const char* src) {
    char*       d = dest;
    const char* s = src;
    while ((*d++ = *s++) != '\0');
    return dest;
}

char* strncpy(char* dest, const char* src, size_t n) {
    char*       d = dest;
    const char* s = src;
    while (n-- > 0 && (*d++ = *s++) != '\0');
    if (n > 0) {
        *d = '\0';
    }

    return dest;
}

char* strncpy_safe(char* dest, const char* src, size_t n) {
    strncpy(dest, src, n);
    dest[n - 1] = '\0';
    return dest;
}

char* strcat(char* dest, const char* src) {
    char*       d = dest;
    const char* s = src;
    while (*d != '\0') {
        d++;
    }
    while ((*d++ = *s++) != '\0');
    return dest;
}

char* strncat(char* dest, const char* src, size_t n) {
    char*       d = dest;
    const char* s = src;
    while (*d != '\0') {
        d++;
    }
    while (n-- > 0 && (*d++ = *s++) != '\0');
    if (n > 0) {
        *d = '\0';
    }
    return dest;
}

int memcmp(const void* s1, const void* s2, size_t n) {
    const unsigned char* p1 = (const unsigned char*)s1;
    const unsigned char* p2 = (const unsigned char*)s2;
    while (n-- > 0) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }

    return 0;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 != '\0' && *s1 == *s2) {
        s1++;
        s2++;
    }

    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        ++s1;
        ++s2;
        --n;
    }
    if (n == 0) {
        return 0;
    } else {
        return (*(uint8_t*)s1 - *(uint8_t*)s2);
    }
}

void* memchr(const void* s, int c, size_t n) {
    const unsigned char* p = (const unsigned char*)s;
    while (n-- > 0) {
        if (*p == (unsigned char)c) {
            return (void*)p;
        }
        p++;
    }
    return NULL;
}

char* strchr(const char* s, int c) {
    while (*s != '\0') {
        if (*s == (char)c) {
            return (char*)s;
        }
        s++;
    }
    return NULL;
}

char* strchrnul(const char* s, int c) {
    while (*s && *s != c) {
        s++;
    }
    return (char*)s;
}

size_t strcspn(const char* s1, const char* s2) {
    size_t len = 0;
    while (*s1 != '\0') {
        if (strchr(s2, *s1) != NULL) {
            return len;
        }
        s1++;
        len++;
    }
    return len;
}

char* strpbrk(const char* s1, const char* s2) {
    while (*s1 != '\0') {
        if (strchr(s2, *s1) != NULL) {
            return (char*)s1;
        }
        s1++;
    }
    return NULL;
}

char* strrchr(const char* s, int c) {
    const char* last = NULL;
    while (*s != '\0') {
        if (*s == (char)c) {
            last = s;
        }
        s++;
    }
    return (char*)last;
}

size_t strspn(const char* s1, const char* s2) {
    size_t len = 0;
    while (*s1 != '\0' && strchr(s2, *s1) != NULL) {
        s1++;
        len++;
    }
    return len;
}

char* strstr(const char* haystack, const char* needle) {
    size_t needle_len = strlen(needle);
    while (*haystack != '\0') {
        if (strncmp(haystack, needle, needle_len) == 0) {
            return (char*)haystack;
        }
        haystack++;
    }
    return NULL;
}

void* memset(void* s, int c, size_t n) {
    unsigned char* p = (unsigned char*)s;
    while (n-- > 0) {
        *p++ = (unsigned char)c;
    }
    return s;
}

char* strtok(char* str, const char* delim) {
    static char* last_token = NULL;
    if (str != NULL) {
        last_token = str;
    } else if (last_token == NULL) {
        return NULL;
    }
    char* token_start = last_token + strspn(last_token, delim);
    if (*token_start == '\0') {
        last_token = NULL;
        return NULL;
    }
    char* token_end = token_start + strcspn(token_start, delim);
    if (*token_end != '\0') {
        *token_end = '\0';
        last_token = token_end + 1;
    } else {
        last_token = NULL;
    }
    return token_start;
}

size_t strtol(const char* str, char** endptr, int base) {
    const char *ptr = str;
    long result = 0;
    int sign = 1;

    // Skip whitespace
    while (isspace((unsigned char)*ptr)) {
        ptr++;
    }

    // Handle optional sign
    if (*ptr == '-') {
        sign = -1;
        ptr++;
    } else if (*ptr == '+') {
        ptr++;
    }

    // Determine base if not specified
    if (base == 0) {
        if (*ptr == '0') {
            if (*(ptr + 1) == 'x' || *(ptr + 1) == 'X') {
                base = 16;
                ptr += 2;
            } else {
                base = 8;
                ptr++;
            }
        } else {
            base = 10;
        }
    }

    // Convert string to long
    while (*ptr) {
        int digit;

        if (isdigit((unsigned char)*ptr)) {
            digit = *ptr - '0';
        } else if (isalpha((unsigned char)*ptr)) {
            digit = tolower((unsigned char)*ptr) - 'a' + 10;
        } else {
            break; // Invalid character
        }

        if (digit >= base) {
            break; // Invalid digit for base
        }

        result = result * base + digit;
        ptr++;
    }

    // Set endptr if requested
    if (endptr) {
        *endptr = (char *)ptr;
    }

    return sign * result;
}

size_t strlen(const char* s) {
    const char* p = s;
    while (*p != '\0') {
        p++;
    }
    return (size_t)(p - s);
}

void strrev(char* str) {
    int length = strlen(str);
    for (int i = 0; i < length / 2; i++) {
        char temp           = str[i];
        str[i]              = str[length - i - 1];
        str[length - i - 1] = temp;
    }
}

void strlwr(char* str) {
    while (*str) {
        *str = tolower((unsigned char)*str);
        str++;
    }
}

void strupr(char* str) {
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

int strcaseeq(const char* s1, const char* s2) {
    while (*s1 && tolower((unsigned char)*s1) == tolower((unsigned char)*s2)) {
        s1++;
        s2++;
    }
    return (unsigned char)tolower((unsigned char)*s1)
           - (unsigned char)tolower((unsigned char)*s2);
}
