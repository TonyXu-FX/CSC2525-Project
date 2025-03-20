#include "Utils.h"

size_t Utils::roundUp(size_t val, size_t mult) {
    return ((val + mult - 1) / mult) * mult;
}