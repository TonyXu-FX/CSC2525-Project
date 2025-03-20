#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

namespace Utils {
    const static size_t CACHE_LINE_BYTES = 64;
    const static size_t REGISTER_BYTES = 8;

    size_t roundUp(size_t val, size_t mult);
}

#endif