#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

namespace Utils {
    const static size_t CACHE_LINE_BYTES = 64;
    const static size_t REGISTER_BYTES = 8;
    // Bitmap is array of 8 bits each
    const static size_t BITMAP_UNIT = 64;

    size_t roundUp(size_t val, size_t mult);
    size_t divPow2(size_t val, size_t pow);
    // If divisor is not power 2, this will not work properly
    size_t modPow2(size_t val, size_t divisor);
}

#endif