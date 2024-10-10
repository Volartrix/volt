#include "bit.h"

bool get_bit(uint64_t num, int bit_position) {
    return (num >> bit_position) & 1;
}