#include "ctype.h"

int isalnum(int c) {
    return (isalpha(c) || isdigit(c));
}

int isalpha(int c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

int isdigit(int c) {
    return (c >= '0' && c <= '9');
}

int islower(int c) {
    return (c >= 'a' && c <= 'z');
}

int isupper(int c) {
    return (c >= 'A' && c <= 'Z');
}

int isspace(int c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f'
            || c == '\r');
}

int ispunct(int c) {
    return ((c >= '!' && c <= '/') || (c >= ':' && c <= '@')
            || (c >= '[' && c <= '`') || (c >= '{' && c <= '~'));
}

int isprint(int c) {
    return (c >= ' ' && c <= '~');
}

int isgraph(int c) {
    return (c >= '!' && c <= '~');
}

int iscntrl(int c) {
    return (c >= 0 && c <= 31) || (c == 127);
}

int tolower(int c) {
    if (isupper(c)) {
        return c + ('a' - 'A');
    } else {
        return c;
    }
}

int toupper(int c) {
    if (islower(c)) {
        return c - ('a' - 'A');
    } else {
        return c;
    }
}
