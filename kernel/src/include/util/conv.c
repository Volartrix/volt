#include "conv.h"


char* uint32_to_bin(uint32_t num) {
    static char binary[33];
    binary[32] = '\0';

    for (int i = 31; i >= 0; i--) {
        binary[i] = (num & 1) ? '1' : '0';
        num >>= 1;
    }

    return binary;
}