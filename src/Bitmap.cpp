#include "Bitmap.h"
#include "Utils.h"

#include <stdlib.h>
#include <new>
#include <cmath>
#include <cstring>

Bitmap::Bitmap(size_t size)
    : size_(size) {
        if (posix_memalign(reinterpret_cast<void**>(&bitmap_), Utils::CACHE_LINE_BYTES, size_) != 0) {
            throw std::bad_alloc();
        }

        std::memset(bitmap_, 0, size_); 
    }

void Bitmap::set(size_t index) {
    size_t arrIndex = index / Utils::BITMAP_UNIT;
    size_t offset = index % Utils::BITMAP_UNIT;
    bitmap_[arrIndex] |= (1ULL << offset);
}

bool Bitmap::read(size_t index) {
    size_t arrIndex = index / Utils::BITMAP_UNIT;
    size_t offset = index % Utils::BITMAP_UNIT;
    return (bitmap_[arrIndex] & (1ULL << offset)) != 0;
}

void Bitmap::setSector(size_t index, uint64_t mask) {
    bitmap_[index] |= mask;
}

bool Bitmap::readSector(size_t index, uint64_t mask) {
    return (bitmap_[index] & mask) == mask;
}

Bitmap::~Bitmap() {
    free(bitmap_);
}
