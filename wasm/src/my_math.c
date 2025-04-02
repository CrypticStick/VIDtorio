#include <limits.h>
#include "my_math.h"

uint32_t digits(uint32_t n) {
    static uint32_t powers[10] = {
        0, 10, 100, 1000, 10000, 100000, 1000000,
        10000000, 100000000, 1000000000,
    };
    static unsigned maxdigits[33] = {
        1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 4, 5, 5,
        5, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10, 
    };
    unsigned bits = sizeof(n) * CHAR_BIT - __builtin_clz(n);
    unsigned digits = maxdigits[bits];
    if (n < powers[digits - 1]) {
        --digits;
    }
    return digits;
}