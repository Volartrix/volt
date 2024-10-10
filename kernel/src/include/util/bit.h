#ifndef __BIT_H__
#define __BIT_H__

#include <stdbool.h>
#include <stdint.h>

// basically extract a bit with a bitmask
bool get_bit(uint64_t num, int bit_position);

#endif    // __BIT_H__
