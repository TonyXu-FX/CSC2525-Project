#include "Utils.h"

size_t Utils::roundUp(size_t val, size_t mult) {
    return ((val + mult - 1) / mult) * mult;
}

size_t Utils::divPow2(size_t val, size_t pow) {
    return val >> pow;
}

size_t Utils::modPow2(size_t val, size_t divisor) {
    return val & (divisor - 1);
}