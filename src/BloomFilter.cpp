#include "BloomFilter.h"
#include "Utils.h"
#include "xxhash.h"

#include <stdlib.h>
#include <new>
#include <cmath>

BloomFilter::BloomFilter(size_t bitsPerEntry, size_t numEntries)
    : bitsPerEntry_(bitsPerEntry)
    , numEntries_(numEntries)
    , size_(Utils::roundUp(bitsPerEntry_ * numEntries_, BITMAP_UNIT))
    , numHashFuncs_(static_cast<size_t>(std::round(std::log(2) * bitsPerEntry_))) {

        if (posix_memalign(reinterpret_cast<void**>(&bitmap_), Utils::CACHE_LINE_BYTES, size_) != 0) {
            throw std::bad_alloc();
        }

        std::memset(bitmap_, 0, size_); 
    }

void BloomFilter::insert(int64_t entry) {
    for (size_t i = 0; i < numHashFuncs_; i++) {
        size_t bit = xxh::xxhash3<64>(&entry, sizeof(int64_t), i);
        size_t index = bit / BITMAP_UNIT;
        size_t offset = bit % BITMAP_UNIT;
        bitmap_[index] |= (1ULL << offset);
    }
}

bool BloomFilter::query(int64_t entry) {
    for (size_t i = 0; i < numHashFuncs_; i++) {
        size_t bit = xxh::xxhash3<64>(&entry, sizeof(int64_t), i);
        size_t index = bit / BITMAP_UNIT;
        size_t offset = bit % BITMAP_UNIT;
        if (bitmap_[index] & (1ULL << offset) != 0) return false;
    }

    return true;
}

BloomFilter::~BloomFilter() {
    free(bitmap_);
}
